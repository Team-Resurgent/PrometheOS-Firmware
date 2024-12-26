#include "hddFormat.h"

#include "..\xboxInternals.h"
#include "..\stringUtility.h"
#include "..\harddrive.h"

namespace
{
	HANDLE mThread;
	hddFormat::hddFormatData mData;
}

bool hddFormat::startThread(int32_t harddrive)
{
	memset(&mData, 0, sizeof(hddFormatData));
	mData.harddrive = harddrive;
	  
	InitializeCriticalSection(&mData.mutex);

	mThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)process, (void*)&mData, 0, NULL);
	if (mThread == NULL) {
        return false;
    }
	SetThreadPriority(mThread, THREAD_PRIORITY_HIGHEST);
	return true;
}

bool hddFormat::completed()
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

hddFormat::hddFormatResponse hddFormat::getResponse()
{
	hddFormatResponse response;
	EnterCriticalSection(&mData.mutex);
	response = mData.response;
	LeaveCriticalSection(&mData.mutex);
	return response;
}

void hddFormat::closeThread()
{
	CloseHandle(mThread);
    DeleteCriticalSection(&mData.mutex);
}

uint64_t WINAPI hddFormat::process(void* param) 
{
	Sleep(100);
	hddFormatData* data = (hddFormatData*)param;

	bool success = harddrive::formatDrive(data->harddrive);
	EnterCriticalSection(&data->mutex);
	data->response = success ? hddFormatResponseDone : hddFormatResponseFailure;
	LeaveCriticalSection(&data->mutex);

	return 0;
}
