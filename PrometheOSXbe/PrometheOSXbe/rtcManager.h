#pragma once

#include "xboxinternals.h"

typedef struct rtcDateTime 
{
	uint8_t hour;
	uint8_t min;
	uint8_t sec;
	uint8_t year;
	uint8_t month;
	uint8_t day;
	uint8_t dotw;
} rtcDateTime;

class rtcManager 
{
public:
	static uint8_t readByte(uint8_t reg);
	static void writeByte(uint8_t reg, uint8_t value);
	static void setDateTime(rtcDateTime dateTime);
	static rtcDateTime getDateTime();
	static void setXboxDateTime(rtcDateTime dateTime);
	static rtcDateTime getXboxDateTime();
};