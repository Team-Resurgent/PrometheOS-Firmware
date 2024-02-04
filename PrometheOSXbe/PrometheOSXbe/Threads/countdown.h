#pragma once

#include "..\xboxInternals.h"

class countdown
{
public:

	typedef struct countdownData {
		uint32_t milliseconds;
		uint64_t startTime;
		uint64_t elapsed;
		CRITICAL_SECTION mutex;
	} countdownData;

	static bool startThread(uint32_t milliseconds);
	static bool completed();
	static uint64_t getTimeRemaining();
	static void closeThread();

private:

	static uint64_t WINAPI process(void* param);
};
