#pragma once

#include "xboxinternals.h"

class crc32
{
public:
	static uint32_t calculate(const uint8_t *buffer, uint32_t size);
	static uint32_t calculate(uint32_t fileHandle);
private:
	static void calculate(const uint8_t *buffer, uint32_t size, uint32_t& crc);
};
