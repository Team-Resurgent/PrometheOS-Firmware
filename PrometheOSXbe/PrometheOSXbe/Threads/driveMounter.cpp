#include "driveMounter.h"

#include "..\xboxInternals.h"
#include "..\stringUtility.h"
#include "..\driveManager.h"
#include "..\XKUtils\XKEEPROM.h"
#include "..\XKUtils\XKHDD.h"

namespace
{
	HANDLE mThread;
	driveMounter::driveMounterData mData;
}

bool driveMounter::startThread(bool onlyMountE)
{
	memset(&mData, 0, sizeof(driveMounterData));
	mData.onlyMountE = onlyMountE;
	  
	InitializeCriticalSection(&mData.mutex);

	mThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)process, (void*)&mData, 0, NULL);
	if (mThread == NULL) {
        return false;
    }
	SetThreadPriority(mThread, THREAD_PRIORITY_HIGHEST);
	return true;
}

bool driveMounter::completed()
{
	DWORD exitCode;
    if (GetExitCodeThread(mThread, &exitCode)) 
	{
        if (exitCode != STILL_ACTIVE) 
		{
            return true;
        }
    }
	return false;
}

void driveMounter::closeThread()
{
	CloseHandle(mThread);
    DeleteCriticalSection(&mData.mutex);
}

uint64_t WINAPI driveMounter::process(void* param) 
{
	driveMounterData* data = (driveMounterData*)param;

	if (data->onlyMountE == true)
	{
		driveManager::mountDrive("HDD0-E");
	}
	else
	{
		driveManager::mountAllDrives();
	}

	return 0;
}
