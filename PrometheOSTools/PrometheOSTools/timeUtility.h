#pragma once

#include "xboxInternals.h"

class timeUtility
{
public:
	static uint64_t getMillisecondsNow();
	static double getDurationSeconds(uint64_t start, uint64_t end);
	static double calculateFramesPerSecond(uint32_t frameCount, double duration);
	static char* getTimeString();
};

