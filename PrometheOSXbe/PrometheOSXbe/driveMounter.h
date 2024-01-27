#pragma once

#include "xboxInternals.h"

class driveMounter
{
public:

	typedef struct driveMounterData {
		bool onlyMountC;
		CRITICAL_SECTION mutex;
	} driveMounterData;

	static bool startThread(bool onlyMountC);
	static bool completed();
	static void closeThread();

private:

	static uint64_t WINAPI process(void* param);
};
