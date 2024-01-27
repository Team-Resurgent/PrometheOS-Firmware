#include "utils.h"
#include "xboxinternals.h"

#include <xtl.h>
#include <stdio.h>
#include <string>

void utils::debugPrint(const char* format, ...)
{
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

void* utils::mallocWithTerminator(uint32_t size)
{
	char* result = (char*)malloc(size + 1);
	result[size] = 0;
	return (void*)result;
}

void* utils::mallocCopyWithTerminator(void* source, uint32_t size, uint32_t copySize)
{
	void* result = mallocWithTerminator(size);
	memcpy(result, source, copySize);
	return result;
}

void utils::reboot()
{
	HalWriteSMBusValue(SMBDEV_PIC16L, PIC16L_CMD_POWER, 0, POWER_SUBCMD_CYCLE);
}

void utils::shutdown()
{
	HalWriteSMBusValue(SMBDEV_PIC16L, PIC16L_CMD_POWER, 0, POWER_SUBCMD_POWER_OFF);
}

uint32_t utils::getFreePhysicalMemory()
{
	uint32_t result;
	MEMORYSTATUS status;
	memset(&status, 0, sizeof(status));
	GlobalMemoryStatus(&status);
	result = (uint32_t)status.dwAvailPhys;
	return result;
}

uint32_t utils::getTotalPhysicalMemory()
{
	uint32_t result;
	MEMORYSTATUS status;
	memset(&status, 0, sizeof(status));
	GlobalMemoryStatus(&status);
	result = (uint32_t)status.dwTotalPhys;
	return result;
}

uint8_t utils::ioInputByte(uint16_t port) 
{
	unsigned char value;
	__asm {
		mov dx, port
		in al, dx
		mov value, al
	}
	return value;
}

uint16_t utils::ioInputShort(uint16_t port) 
{
	unsigned short value;
	__asm {
		mov dx, port
		in ax, dx
		mov value, ax
	}
	return value;
}

uint32_t utils::ioInputInt(uint16_t port) 
{
	unsigned int value;
	__asm {
		mov dx, port
		in eax, dx
		mov value, eax
	}
	return value;
}

void utils::ioOutputByte(uint16_t port, uint8_t value) 
{
	__asm {
		mov dx, port
		mov al, value
		out dx, al
	}
}

void utils::ioOutputShort(uint16_t port, uint16_t value) 
{
	__asm {
		mov  dx, port
		mov  ax, value
		out  dx, ax
	}
}

void utils::ioOutputInt(uint16_t port, uint32_t value) 
{
	__asm {
		mov  dx, port
        mov  eax, value
        out  dx, eax
	}
}

void utils::setLedStates(uint32_t ledStates)
{
	HalWriteSMBusByte(SMC_SLAVE_ADDRESS, SMC_COMMAND_LED_STATES, ledStates);
    HalWriteSMBusByte(SMC_SLAVE_ADDRESS, SMC_COMMAND_LED_OVERRIDE, SMC_LED_OVERRIDE_USE_REQUESTED_LED_STATES);
}

bool utils::smcTransmitByteAndRecieve(uint8_t picAddressI2cFormat, uint8_t data, uint8_t& result)
{
    int32_t retries = 400;
    while(ioInputShort(I2C_IO_BASE + 0) & 0x0800);
    while (retries--)
    {
        ioOutputByte(I2C_IO_BASE + 4, (picAddressI2cFormat << 1) | 1);
        ioOutputByte(I2C_IO_BASE + 8, data);
        ioOutputShort(I2C_IO_BASE + 0, 0xffff); 
        ioOutputByte(I2C_IO_BASE + 2, 0x0a);
        {
            uint8_t b = 0x0;
            while ((b & 0x36) == 0)
            {
                b = ioInputByte(I2C_IO_BASE + 0);
            }
            if ((b & 0x10) == 0x10)
            {
                result = ioInputByte(I2C_IO_BASE + 6);
				return true;
            }
        }
    }
    return false;
}

uint32_t utils::roundUpToNextPowerOf2(uint32_t value) 
{
	value--;
	value |= value >> 1;
	value |= value >> 2;
	value |= value >> 4;
	value |= value >> 8;
	value |= value >> 16;
	value++;
	return value;
}