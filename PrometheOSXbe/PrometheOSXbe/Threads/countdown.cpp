#include "countdown.h"

#include "..\xboxInternals.h"
#include "..\stringUtility.h"
#include "..\fileSystem.h"
#include "..\utils.h"
#include "..\settingsManager.h"
#include "..\timeUtility.h"

namespace
{
	HANDLE mThread;
	countdown::countdownData mData;
}

bool countdown::startThread(uint32_t milliseconds)
{
	memset(&mData, 0, sizeof(countdownData));
	mData.milliseconds = milliseconds;
	mData.startTime = timeUtility::getMillisecondsNow();
	  
	InitializeCriticalSection(&mData.mutex);

	mThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)process, (void*)&mData, 0, NULL);
	if (mThread == NULL) {
        return false;
    }
	SetThreadPriority(mThread, THREAD_PRIORITY_HIGHEST);
	return true;
}

bool countdown::completed()
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

uint64_t countdown::getTimeRemaining()
{
	uint64_t result = 0;
	EnterCriticalSection(&mData.mutex);
	uint64_t elapsed = mData.elapsed;
	if (elapsed < mData.milliseconds)
	{
		result = mData.milliseconds - elapsed;
	}
	LeaveCriticalSection(&mData.mutex);
	return result;
}

void countdown::closeThread()
{
	EnterCriticalSection(&mData.mutex);
	mData.milliseconds = 0;
	LeaveCriticalSection(&mData.mutex);

	CloseHandle(mThread);
    DeleteCriticalSection(&mData.mutex);
}

uint64_t WINAPI countdown::process(void* param) 
{
	countdownData* data = (countdownData*)param;

	EnterCriticalSection(&data->mutex);
	uint64_t startTime = data->startTime;
	uint64_t milliseconds = data->milliseconds;
	LeaveCriticalSection(&data->mutex);

	uint64_t elapsed = 0;
	while (elapsed < milliseconds)
	{
		uint64_t timeNow = timeUtility::getMillisecondsNow();
		elapsed = (timeNow - startTime);
		EnterCriticalSection(&data->mutex);
		data->elapsed = elapsed;
		LeaveCriticalSection(&data->mutex);
		Sleep(100);
	}

	return 0;
}
