#pragma once

#include "xboxInternals.h"

class temperatureManager
{
public:
	static void init();
	static int32_t getCpuTemp();
	static int32_t getMbTemp();
	static int32_t getFanSpeed();
	static void setFanSpeed(uint32_t fanspeed);
	static void refresh();
};
