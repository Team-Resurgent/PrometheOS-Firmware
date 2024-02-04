#pragma once

#include "..\xboxInternals.h"

class driveMounter
{
public:

	typedef struct driveMounterData {
		bool onlyMountE;
		CRITICAL_SECTION mutex;
	} driveMounterData;

	static bool startThread(bool onlyMountE);
	static bool completed();
	static void closeThread();

private:

	static uint64_t WINAPI process(void* param);
};
