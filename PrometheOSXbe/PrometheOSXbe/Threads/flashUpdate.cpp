#include "flashUpdate.h"

#include "..\xboxInternals.h"
#include "..\stringUtility.h"
#include "..\fileSystem.h"
#include "..\utils.h"
#include "..\settingsManager.h"
#include "..\xenium.h"

namespace
{
	HANDLE mThread;
	flashUpdate::flashUpdateData mData;
}

bool flashUpdate::startThread(const char* filePath)
{
	memset(&mData, 0, sizeof(flashUpdateData));
	mData.filePath = strdup(filePath);
	mData.currentStep = 0;
	mData.totalSteps = 7;

	InitializeCriticalSection(&mData.mutex);

	mThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)process, (void*)&mData, 0, NULL);
	if (mThread == NULL) {
        return false;
    }
	SetThreadPriority(mThread, THREAD_PRIORITY_NORMAL);
	return true;
}

bool flashUpdate::completed()
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

flashUpdate::flashUpdateResponse flashUpdate::getResponse()
{
	flashUpdateResponse response;
	EnterCriticalSection(&mData.mutex);
	response = mData.response;
	LeaveCriticalSection(&mData.mutex);
	return response;
}

void flashUpdate::getProgress(uint32_t& currentStep, uint32_t& totalSteps)
{
	EnterCriticalSection(&mData.mutex);
	currentStep = mData.currentStep;
	totalSteps = mData.totalSteps;
	LeaveCriticalSection(&mData.mutex);
}

void flashUpdate::closeThread()
{
	CloseHandle(mThread);
    DeleteCriticalSection(&mData.mutex);
	free(mData.filePath);
}

uint64_t WINAPI flashUpdate::process(void* param) 
{
	flashUpdateData* data = (flashUpdateData*)param;

	setResponse(data, flashUpdateProcessing);

	utils::dataContainer* flashData;

	uint32_t fileHandle;
	if (fileSystem::fileOpen(data->filePath, fileSystem::FileModeRead, fileHandle))
	{
		uint32_t fileSize;
		if (fileSystem::fileSize(fileHandle, fileSize))
		{
			char* buffer = (char*)malloc(fileSize);
			uint32_t bytesRead;
			if (fileSystem::fileRead(fileHandle, buffer, fileSize, bytesRead) && bytesRead == fileSize)
			{
				flashData = new utils::dataContainer(buffer, fileSize, fileSize);
			}
			free(buffer);
		}
		fileSystem::fileClose(fileHandle);
	}

	if (flashData == NULL)
	{
		setResponse(data, flashUpdateFailedToLoadFile);
		return 0;
	}

	for (uint32_t i = 0; i < 7; i++)
	{
		EnterCriticalSection(&data->mutex);
		data->currentStep = i + 1;
		LeaveCriticalSection(&data->mutex);

		xenium::bankEnum currentBank = xenium::bank1_256k;
		if (i == 1)
		{
			currentBank = xenium::bank2_256k;
		}
		else if (i == 2)
		{
			currentBank = xenium::bank3_256k;
		}
		else if (i == 3)
		{
			currentBank = xenium::bank3_256k;
		}
		else if (i == 4)
		{
			currentBank = xenium::bankBootloader;
		}
		else if (i == 5)
		{
			currentBank = xenium::bankXeniumOS;
		}
		else if (i == 6)
		{
			currentBank = xenium::bankRecovery;
		}

		uint32_t bankSize = xenium::getBankSize(currentBank);
		uint32_t memOffset = xenium::getBankMemOffset(currentBank);

		xenium::setLedColor(xenium::ledColorAmber);
		setResponse(data, flashUpdateErasing);
		xenium::eraseBank(currentBank, 0, bankSize);
		
		xenium::setLedColor(xenium::ledColorBlue);
		setResponse(data, flashUpdateWriting);
		xenium::writeBank(currentBank, 0, flashData, memOffset, bankSize);

		xenium::setLedColor(xenium::ledColorPurple);
		setResponse(data, flashUpdateVerifying);
		if (xenium::verifyBank(currentBank, 0, flashData, memOffset, bankSize) == false)
		{
			xenium::setLedColor((xenium::ledColorEnum)settingsManager::getLedColor());
			setResponse(data, flashUpdateVerificationFailed);
			delete(flashData);
			return 0;
		}
	}

	xenium::setLedColor((xenium::ledColorEnum)settingsManager::getLedColor());
	setResponse(data, flashUpdateDone);
	delete(flashData);

	return 0;
}

void flashUpdate::setResponse(flashUpdateData* data, flashUpdateResponse response)
{
	EnterCriticalSection(&data->mutex);
	data->response = response;
	LeaveCriticalSection(&data->mutex);
}
