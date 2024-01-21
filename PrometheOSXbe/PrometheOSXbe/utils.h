#pragma once

#include "xboxinternals.h"
#include <string>

class utils
{
public:

	typedef struct dataContainer 
	{

		char* data;
		uint32_t size;

		dataContainer() : data(NULL), size(0) {}

		dataContainer(uint32_t size)
		{
			this->data = (char*)utils::mallocWithTerminator(size);
			this->size = size; 
		}

		dataContainer(char* data, uint32_t size, uint32_t copySize)
		{
			this->data = (char*)utils::mallocCopyWithTerminator(data, size, copySize);
			this->size = size; 
		}

		~dataContainer()
		{
			free(this->data);
		}

	} dataContainer;

	typedef struct intContainer 
	{
		int32_t value;

		intContainer(int32_t value)
		{
			this->value = value;
		}

	} intContainer;

	static void debugPrint(const char* format, ...);
	static void* mallocWithTerminator(uint32_t size);
	static void* mallocCopyWithTerminator(void* source, uint32_t size, uint32_t copySize);
	static void reboot();
	static void shutdown();
	static uint32_t getFreePhysicalMemory();
	static uint32_t getTotalPhysicalMemory();

	static uint8_t ioInputByte(uint16_t port);
	static uint16_t ioInputShort(uint16_t port);
	static uint32_t ioInputInt(uint16_t port);
	static void ioOutputByte(uint16_t port, uint8_t value);
	static void ioOutputShort(uint16_t port, uint16_t value);
	static void ioOutputInt(uint16_t port, uint32_t value);
	static void setLedStates(uint32_t ledStates);
	static bool smcTransmitByteAndRecieve(unsigned char picAddressI2cFormat, uint8_t data, uint8_t& result);
};
