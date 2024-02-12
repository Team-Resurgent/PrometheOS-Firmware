#include "flashBank.h"

#include "..\xboxInternals.h"
#include "..\stringUtility.h"
#include "..\fileSystem.h"
#include "..\utils.h"
#include "..\settingsManager.h"

namespace
{
	HANDLE mThread;
	flashBank::flashBankData mData;
}

bool flashBank::startThread(const char* filePath, const char* bankName, uint8_t ledColor)
{
	memset(&mData, 0, sizeof(flashBankData));
	mData.filePath = strdup(filePath);
	mData.bankName = strdup(bankName);
	mData.ledColor = ledColor;
	  
	InitializeCriticalSection(&mData.mutex);

	mThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)process, (void*)&mData, 0, NULL);
	if (mThread == NULL) {
        return false;
    }
	SetThreadPriority(mThread, THREAD_PRIORITY_NORMAL);
	return true;
}

bool flashBank::completed()
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

flashBank::flashBankResponse flashBank::getResponse()
{
	flashBankResponse response;
	EnterCriticalSection(&mData.mutex);
	response = mData.response;
	LeaveCriticalSection(&mData.mutex);
	return response;
}

void flashBank::closeThread()
{
	CloseHandle(mThread);
    DeleteCriticalSection(&mData.mutex);
	free(mData.filePath);
	free(mData.bankName);
}

uint64_t WINAPI flashBank::process(void* param) 
{
	flashBankData* data = (flashBankData*)param;

	setResponse(data, flashBankProcessing);

	utils::dataContainer* bankData;

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
				bankData = new utils::dataContainer(buffer, fileSize, fileSize);
			}
			free(buffer);
		}
		fileSystem::fileClose(fileHandle);
	}

	if (bankData == NULL)
	{
		setResponse(data, flashBankFailedToLoadFile);
		return 0;
	}

	setResponse(data, flashBankOptimizing);

	uint8_t bankId = 0;

	uint8_t slotsNeeded = (uint8_t)(bankData->size >> 18);
	settingsManager::optimizeBanks(slotsNeeded);
	if (settingsManager::tryGetFreeBank(slotsNeeded, bankId) == false)
	{
		setResponse(data, flashBankNotEnoughSlots);
		delete(bankData);
		return 0;
	}

	setResponse(data, flashBankErasing);
	settingsManager::eraseBank(bankId, bankData->size);
	
	setResponse(data, flashBankWriting);
	settingsManager::writeBank(bankId, bankData, data->bankName, data->ledColor);

	setResponse(data, flashBankVerifying);
	if (settingsManager::verifyBank(bankId, bankData) == false)
	{
		setResponse(data, flashBankVerificationFailed);
		delete(bankData);
		return 0;
	}

	setResponse(data, flashBankDone);
	delete(bankData);

	return 0;
}

void flashBank::setResponse(flashBankData* data, flashBankResponse response)
{
	EnterCriticalSection(&data->mutex);
	data->response = response;
	LeaveCriticalSection(&data->mutex);
}
