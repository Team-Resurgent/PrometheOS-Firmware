#include "rtcManager.h"
#include "xboxinternals.h"

#define RTC_ADDRESS 0xD0
#define EXTRACT_DATE_VALUE(value) ((((value & 0xf0) >> 4) * 10) + (value & 0x0f))
#define COMPOSE_DATE_VALUE(value) ((((value - (value % 10)) / 10) << 4) | (value % 10))

// DS3231 Control Register (0x0E) bit settings for square wave
#define SQW_1HZ_MODE 0x10  // Set square wave to 1Hz

uint8_t rtcManager::readByte(uint8_t reg)
{
	const int rtcAddess = 0x68;
	uint32_t temp = 0;
	HalReadSMBusByte(RTC_ADDRESS, reg, &temp);
	return (uint8_t)temp;
}

void rtcManager::writeByte(uint8_t reg, uint8_t value)
{
	HalWriteSMBusByte(RTC_ADDRESS, reg, value);
}

void rtcManager::setDateTime(rtcDateTime dateTime)
{
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

	writeByte(0, COMPOSE_DATE_VALUE(utcSystemTime.wSecond));
	writeByte(1, COMPOSE_DATE_VALUE(utcSystemTime.wMinute));
	writeByte(2, COMPOSE_DATE_VALUE(utcSystemTime.wHour));
	writeByte(3, (uint8_t)utcSystemTime.wDayOfWeek);
	writeByte(4, COMPOSE_DATE_VALUE(utcSystemTime.wDay));
	writeByte(5, COMPOSE_DATE_VALUE(utcSystemTime.wMonth));
	writeByte(6, COMPOSE_DATE_VALUE(utcSystemTime.wYear % 100));

    writeByte(0x0E, SQW_1HZ_MODE);  // Set square wave output to 1Hz
}

rtcDateTime rtcManager::getDateTime()
{
	SYSTEMTIME utcSystemTime;
	uint8_t value;

	utcSystemTime.wMilliseconds = 0;
	value = readByte(0);
	utcSystemTime.wSecond = min(EXTRACT_DATE_VALUE(value), 59);
	value = readByte(1);
	utcSystemTime.wMinute = min(EXTRACT_DATE_VALUE(value), 59);
	value = readByte(2);
	utcSystemTime.wHour = min(EXTRACT_DATE_VALUE(value & 0x3f), 23);
	value = readByte(3);
	utcSystemTime.wDayOfWeek = value & 0x7;
	value = readByte(4);
	utcSystemTime.wDay = max(min(EXTRACT_DATE_VALUE(value), 31), 1);
	value = readByte(5);
	utcSystemTime.wMonth = max(min(EXTRACT_DATE_VALUE(value), 12), 1);
	value = readByte(6);
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

void rtcManager::setXboxDateTime(rtcDateTime dateTime)
{
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

rtcDateTime rtcManager::getXboxDateTime()
{
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