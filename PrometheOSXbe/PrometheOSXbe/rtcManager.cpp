#include "rtcManager.h"
#include "xboxinternals.h"

#define RTC_ADDRESS 0xD0
#define EXTRACT_DATE_VALUE(value) ((((value & 0xf0) >> 4) * 10) + (value & 0x0f))
#define COMPOSE_DATE_VALUE(value) ((((value - (value % 10)) / 10) << 4) | (value % 10))
#define DS3231_SQW_1HZ_MODE 0x10

// Per-device SMBus lock: serialise RTC access so a web-thread date/time read/write can't interleave
// with on-screen UI / boot-time RTC access mid-transaction. Recursive, so setDateTime/getDateTime
// (which call the byte helpers -- also locked) nest safely. Initialised by a global ctor before any
// thread starts.
static CRITICAL_SECTION s_rtcLock;
static struct RtcLockInit {
    RtcLockInit() { InitializeCriticalSection(&s_rtcLock); }
} s_rtcLockInit;

struct RtcI2cLock {
    RtcI2cLock() { EnterCriticalSection(&s_rtcLock); }
    ~RtcI2cLock() { LeaveCriticalSection(&s_rtcLock); }
};

uint8_t rtcManager::readByte(uint8_t reg) {
    RtcI2cLock lock;
    const int rtcAddess = 0x68;
    uint32_t temp = 0;
    HalReadSMBusByte(RTC_ADDRESS, reg, &temp);
    return (uint8_t)temp;
}

void rtcManager::writeByte(uint8_t reg, uint8_t value) {
    RtcI2cLock lock;
    HalWriteSMBusByte(RTC_ADDRESS, reg, value);
}

void rtcManager::setDateTime(rtcDateTime dateTime) {
    RtcI2cLock lock; // hold the bus across the whole multi-register write
    SYSTEMTIME localSystemTime;
    localSystemTime.wMilliseconds = 0;
    localSystemTime.wSecond = dateTime.sec;
    localSystemTime.wMinute = dateTime.min;
    localSystemTime.wHour = dateTime.hour;
    localSystemTime.wDayOfWeek = dateTime.dotw;
    localSystemTime.wDay = dateTime.day;
    localSystemTime.wMonth = dateTime.month;
    localSystemTime.wYear = 2000 + dateTime.year;

    FILETIME localFileTime;
    SystemTimeToFileTime(&localSystemTime, &localFileTime);

    FILETIME utcFileTime;
    LocalFileTimeToFileTime(&localFileTime, &utcFileTime);

    SYSTEMTIME utcSystemTime;
    FileTimeToSystemTime(&utcFileTime, &utcSystemTime);

    writeByte(0x00, COMPOSE_DATE_VALUE(utcSystemTime.wSecond));
    writeByte(0x01, COMPOSE_DATE_VALUE(utcSystemTime.wMinute));
    writeByte(0x02, COMPOSE_DATE_VALUE(utcSystemTime.wHour));
    writeByte(0x03, (uint8_t)utcSystemTime.wDayOfWeek);
    writeByte(0x04, COMPOSE_DATE_VALUE(utcSystemTime.wDay));
    writeByte(0x05, COMPOSE_DATE_VALUE(utcSystemTime.wMonth));
    writeByte(0x06, COMPOSE_DATE_VALUE(utcSystemTime.wYear % 100));
    writeByte(0x0E, DS3231_SQW_1HZ_MODE);
}

rtcDateTime rtcManager::getDateTime() {
    RtcI2cLock lock; // hold the bus across the whole multi-register read
    SYSTEMTIME utcSystemTime;
    uint8_t value;

    utcSystemTime.wMilliseconds = 0;
    value = readByte(0x00);
    utcSystemTime.wSecond = min(EXTRACT_DATE_VALUE(value), 59);
    value = readByte(0x01);
    utcSystemTime.wMinute = min(EXTRACT_DATE_VALUE(value), 59);
    value = readByte(0x02);
    utcSystemTime.wHour = min(EXTRACT_DATE_VALUE(value & 0x3f), 23);
    value = readByte(0x03);
    utcSystemTime.wDayOfWeek = value & 0x7;
    value = readByte(0x04);
    utcSystemTime.wDay = max(min(EXTRACT_DATE_VALUE(value), 31), 1);
    value = readByte(0x05);
    utcSystemTime.wMonth = max(min(EXTRACT_DATE_VALUE(value), 12), 1);
    value = readByte(0x06);
    utcSystemTime.wYear = 2000 + EXTRACT_DATE_VALUE(value);

    FILETIME utcFileTime;
    SystemTimeToFileTime(&utcSystemTime, &utcFileTime);

    FILETIME localFileTime;
    FileTimeToLocalFileTime(&utcFileTime, &localFileTime);

    SYSTEMTIME localSystemTime;
    FileTimeToSystemTime(&localFileTime, &localSystemTime);

    rtcDateTime dateTime;
    dateTime.sec = (uint8_t)localSystemTime.wSecond;
    dateTime.min = (uint8_t)localSystemTime.wMinute;
    dateTime.hour = (uint8_t)localSystemTime.wHour;
    dateTime.dotw = (uint8_t)localSystemTime.wDayOfWeek;
    dateTime.day = (uint8_t)localSystemTime.wDay;
    dateTime.month = (uint8_t)localSystemTime.wMonth;
    dateTime.year = (uint8_t)(localSystemTime.wYear % 100);
    return dateTime;
}

void rtcManager::setXboxDateTime(rtcDateTime dateTime) {
    SYSTEMTIME localSystemTime;
    localSystemTime.wMilliseconds = 0;
    localSystemTime.wSecond = dateTime.sec;
    localSystemTime.wMinute = dateTime.min;
    localSystemTime.wHour = dateTime.hour;
    localSystemTime.wDayOfWeek = dateTime.dotw;
    localSystemTime.wDay = dateTime.day;
    localSystemTime.wMonth = dateTime.month;
    localSystemTime.wYear = 2000 + dateTime.year;

    FILETIME localFileTime;
    SystemTimeToFileTime(&localSystemTime, &localFileTime);

    FILETIME utcFileTime;
    LocalFileTimeToFileTime(&localFileTime, &utcFileTime);
    NtSetSystemTime(&utcFileTime, NULL);
}

rtcDateTime rtcManager::getXboxDateTime() {
    FILETIME utcFileTime;
    KeQuerySystemTime(&utcFileTime);

    FILETIME localFileTime;
    FileTimeToLocalFileTime(&utcFileTime, &localFileTime);

    SYSTEMTIME localSystemTime;
    FileTimeToSystemTime(&localFileTime, &localSystemTime);

    rtcDateTime dateTime;
    dateTime.sec = (uint8_t)localSystemTime.wSecond;
    dateTime.min = (uint8_t)localSystemTime.wMinute;
    dateTime.hour = (uint8_t)localSystemTime.wHour;
    dateTime.dotw = (uint8_t)localSystemTime.wDayOfWeek;
    dateTime.day = (uint8_t)localSystemTime.wDay;
    dateTime.month = (uint8_t)localSystemTime.wMonth;
    dateTime.year = (uint8_t)(localSystemTime.wYear % 100);
    return dateTime;
}
