#include "utils.h"
#include "xboxinternals.h"

#include <xtl.h>
#include <stdio.h>
#include <string>
#include <cmath>
#include <conio.h>

// Creates a "hexdump -Cv" style string - caller free()'s returned string
char* utils::hexDump(void* data, uint32_t size) {
    char ascii[17];
    size_t i, j;
    ascii[16] = '\0';

    uint32_t rows = (uint32_t)ceil((float)(size / 16));
    uint32_t bufSize = (rows + 1) * 79 + 2;

    char* buf = (char*)malloc(bufSize);
    char* retBuf = buf;

    _snprintf(buf, 10, "00000000  ");
    buf += 10;

    for (i = 0; i < size; ++i) {
        _snprintf(buf, 3, "%02x ", ((unsigned char*)data)[i]);
        buf += 3;
        if (((unsigned char*)data)[i] >= ' ' && ((unsigned char*)data)[i] <= '~') {
            ascii[i % 16] = ((unsigned char*)data)[i];
        } else {
            ascii[i % 16] = '.';
        }
        if ((i + 1) % 8 == 0 || i + 1 == size) {
            _snprintf(buf++, 1, " ");
            if ((i + 1) % 16 == 0) {
                _snprintf(buf, 29, "|%s|\n%08lx  ", ascii, (i + 1));
                buf += 29;
            } else if (i + 1 == size) {
                ascii[(i + 1) % 16] = '\0';
                if ((i + 1) % 16 <= 8) {
                    _snprintf(buf++, 1, " ");
                }
                for (j = (i + 1) % 16; j < 16; ++j) {
                    _snprintf(buf, 3, "   ");
                    buf += 3;
                }
                _snprintf(buf, 19, "|%s|\n", ascii);
                buf += 19;
            }
        }
    }

    _snprintf(buf++, 1, "\n");
    *buf = 0;

    return retBuf;
}

char* utils::x3NibbleToBinary(uint8_t nibble) {
    char* result = (char*)mallocWithTerminator(4);

    nibble &= 0x0F;
    for (uint8_t i = 0; i < 4; i++) {
        result[i] = ((nibble >> i) & 1) ? '0' : '1';
    }

    return result;
}

void utils::hexDumpDebug(void* data, uint32_t size) {
#ifdef _DEBUG
    char* buf = hexDump(data, size);
    debugPrint(buf);
    free(buf);
#endif
}

void utils::uSleep(uint64_t timeout) {
    timeout *= -10; // 100ns units
    KeDelayExecutionThread(1, 0, (PLARGE_INTEGER)(&timeout));
}

void utils::byteSwap16(uint8_t* buf, uint32_t size) {
    uint8_t tmp;

    for (uint32_t i = 0; i < size; i += 2) {
        tmp = buf[i];
        buf[i] = buf[i + 1];
        buf[i + 1] = tmp;
    }
}

void utils::debugPrint(const char* format, ...) {
#ifdef _DEBUG
    va_list args;
    va_start(args, format);

    uint32_t length = _vsnprintf(NULL, 0, format, args);

    char* message = (char*)malloc(length + 1);
    _vsnprintf(message, length, format, args);
    message[length] = 0;

    va_end(args);

    OutputDebugStringA(message);
    free(message);
#endif
}

void* utils::mallocWithTerminator(uint32_t size) {
    char* result = (char*)malloc(size + 1);
    result[size] = 0;
    return (void*)result;
}

void* utils::mallocCopyWithTerminator(const void* source, uint32_t size, uint32_t copySize) {
    void* result = mallocWithTerminator(size);
    memcpy(result, source, copySize);
    return result;
}

void utils::warmreboot() {
    HalWriteSMBusValue(SMBDEV_PIC16L, PIC16L_CMD_POWER, 0, POWER_SUBCMD_RESET);
}

void utils::reboot() {
    HalWriteSMBusValue(SMBDEV_PIC16L, PIC16L_CMD_POWER, 0, POWER_SUBCMD_CYCLE);
}

void utils::shutdown() {
    HalWriteSMBusValue(SMBDEV_PIC16L, PIC16L_CMD_POWER, 0, POWER_SUBCMD_POWER_OFF);
}

uint32_t utils::getFreePhysicalMemory() {
    uint32_t result;
    MEMORYSTATUS status;
    memset(&status, 0, sizeof(status));
    GlobalMemoryStatus(&status);
    result = (uint32_t)status.dwAvailPhys;
    return result;
}

uint32_t utils::getTotalPhysicalMemory() {
    uint32_t result;
    MEMORYSTATUS status;
    memset(&status, 0, sizeof(status));
    GlobalMemoryStatus(&status);
    result = (uint32_t)status.dwTotalPhys;
    return result;
}

uint8_t utils::ioInputByte(uint16_t port) {
    return (uint8_t)_inp(port);
}

uint16_t utils::ioInputShort(uint16_t port) {
    return (uint16_t)_inpw(port);
}

uint32_t utils::ioInputInt(uint16_t port) {
    return (uint32_t)_inpd(port);
}

void utils::ioOutputByte(uint16_t port, uint8_t value) {
    _outp(port, value);
}

void utils::ioOutputShort(uint16_t port, uint16_t value) {
    _outpw(port, value);
}

void utils::ioOutputInt(uint16_t port, uint32_t value) {
    _outpd(port, value);
}

USHORT WINAPI utils::FindFirstSetRightMember(ULONG Set) {
    __asm {
        bsf     eax, Set
    }
}

void utils::setLedStates(uint32_t ledStates) {
    HalWriteSMBusByte(SMC_SLAVE_ADDRESS, SMC_COMMAND_LED_STATES, ledStates);
    HalWriteSMBusByte(SMC_SLAVE_ADDRESS, SMC_COMMAND_LED_OVERRIDE, SMC_LED_OVERRIDE_USE_REQUESTED_LED_STATES);
}

uint32_t utils::roundUpToNextPowerOf2(uint32_t value) {
    value--;
    value |= value >> 1;
    value |= value >> 2;
    value |= value >> 4;
    value |= value >> 8;
    value |= value >> 16;
    value++;
    return value;
}

void utils::sleepMicroSecs(LARGE_INTEGER clockFreq, uint16_t microSecs) {
    LARGE_INTEGER performanceCount;
    LARGE_INTEGER currentCount;
    QueryPerformanceCounter(&performanceCount);
    LONGLONG waitTime = microSecs * clockFreq.QuadPart / 1000000;
    do {
        QueryPerformanceCounter(&currentCount);
        Sleep(0);
    } while (currentCount.QuadPart - performanceCount.QuadPart < waitTime);
}

void utils::swapString(char** oldValue, const char* newValue) {
    free(*oldValue);
    *oldValue = strdup(newValue);
}

void utils::freeString(char** value) {
    free(*value);
    *value = NULL;
}

uint8_t utils::smartxxMangleCommand(uint8_t value) {
    uint8_t mangledByte = value & 1;

    if ((value & 2) != 0) {
        mangledByte |= 0x20;
    }

    mangledByte |= value & (4 | 8);

    if ((value & 0x10) != 0) {
        mangledByte |= 0x40;
    }

    if ((value & 0x20) != 0) {
        mangledByte |= 2;
    }

    if ((value & 0x40) != 0) {
        mangledByte |= 0x10;
    }

    return mangledByte | (value & 0x80);
}

uint8_t utils::smartxxUnmangleCommandResponse(uint8_t value) {
    uint8_t unmangledByte = value & 1;

    if ((value & 0x20) != 0) {
        unmangledByte |= 2;
    }

    unmangledByte |= value & (4 | 8);

    if ((value & 0x40) != 0) {
        unmangledByte |= 0x10;
    }

    if ((value & 2) != 0) {
        unmangledByte |= 0x20;
    }

    if ((value & 0x10) != 0) {
        unmangledByte |= 0x40;
    }

    return unmangledByte | (value & 0x80);
}

uint32_t utils::smartxxMangleFlashAddress(uint32_t address) {
    uint32_t modifiedAddress;
    uint32_t tempAddress;

    modifiedAddress = address & 0xffff0003;
    if ((address & 4) != 0) {
        modifiedAddress = ((address >> 8) & 0xFFFFFF) << 8 | (modifiedAddress & 0xFF);
        modifiedAddress = (modifiedAddress & 0xFFFF00FF) | 0x80;
    }
    modifiedAddress |= address & (8 | 0x10 | 0x20);

    if ((address & 0x40) != 0) {
        modifiedAddress |= 4;
    }

    if ((char)address < 0) {
        modifiedAddress |= 0x40;
    }

    tempAddress = modifiedAddress | address & (0x100 | 0x200);
    if ((address & 0x400) != 0) {
        tempAddress = (((modifiedAddress >> 16) & 0xFFFF) << 16) | (((tempAddress >> 8) & 0xFF) << 8) |
            (modifiedAddress & 0xFF) | 0x4000;
    }

    modifiedAddress = tempAddress | address & (0x800 | 0x1000 | 0x2000);
    if ((address & 0x4000) != 0) {
        modifiedAddress = (((tempAddress >> 16) & 0xFFFF) << 16) | (((modifiedAddress >> 8) & 0xFF) << 8) |
            (tempAddress & 0xFF) | 0x400;
    }

    return modifiedAddress | address & 0x8000;
}

void utils::readAmdFlashIds(volatile uint8_t* lpcMemMap, uint8_t& manufacturerId, uint8_t& deviceId) {
    lpcMemMap[0x5555] = 0xAA;
    lpcMemMap[0x2AAA] = 0x55;
    lpcMemMap[0x5555] = 0x90;
    manufacturerId = lpcMemMap[0];
    deviceId = lpcMemMap[1];
    lpcMemMap[0] = 0xF0;
}

void utils::readIntelFlashIds(volatile uint8_t* lpcMemMap, uint8_t& manufacturerId, uint8_t& deviceId) {
    lpcMemMap[0] = 0x98;
    manufacturerId = lpcMemMap[0];
    deviceId = lpcMemMap[1];
    lpcMemMap[0] = 0xFF;
}

void utils::prepareScratch(bool safeMode) {
    DWORD scratch;
    HalReadSMBusByte(SMBDEV_PIC16L, PIC16L_CMD_SCRATCH_REGISTER, &scratch);
    scratch = scratch & ~(SCRATCH_REGISTER_BITVALUE_NO_ANIMATION | SCRATCH_REGISTER_BITVALUE_SAFE_MODE);
    if (safeMode) {
        scratch = scratch | SCRATCH_REGISTER_BITVALUE_SAFE_MODE;
    }
    HalWriteSMBusByte(SMBDEV_PIC16L, PIC16L_CMD_SCRATCH_REGISTER, scratch);
}

char* utils::toSdCardLpcPath(const char* prefixedPath) {
    if (prefixedPath == NULL || _strnicmp(prefixedPath, "SDCARD:", 7) != 0) {
        return NULL;
    }

    const char* remainder = prefixedPath + 7;
    while (*remainder == '\\') {
        remainder++;
    }

    size_t length = strlen(remainder);
    char* result = (char*)malloc(length + 2);
    if (result == NULL) {
        return NULL;
    }

    result[0] = '\\';
    memcpy(result + 1, remainder, length + 1);
    return result;
}

long utils::roundl(double number) {
    int base = (int)number;
    return number < (base + 0.5f) ? base : base + 1;
}