#include "flash.h"
#include "xboxInternals.h"
#include "stringUtility.h"
#include "fileSystem.h"
#include "utils.h"
#include "settingsManager.h"

namespace
{
	HANDLE mThread;
	flash::flashData mData;
}

bool flash::startThread(const char* filePath, const char* bankName, uint8_t ledColor)
{
	memset(&mData, 0, sizeof(flashData));
	mData.filePath = strdup(filePath);
	mData.bankName = strdup(bankName);
	mData.ledColor = ledColor;
	  
	InitializeCriticalSection(&mData.mutex);

	mThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)process, (void*)&mData, 0, NULL);
	if (mThread == NULL) {
        return false;
    }
	return true;
}

bool flash::completed()
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

flash::flashResponse flash::getResponse()
{
	flashResponse response;
	EnterCriticalSection(&mData.mutex);
	response = mData.response;
	LeaveCriticalSection(&mData.mutex);
	return response;
}

void flash::closeThread()
{
	CloseHandle(mThread);
    DeleteCriticalSection(&mData.mutex);
	free(mData.filePath);
	free(mData.bankName);
}

uint64_t WINAPI flash::process(void* param) 
{
	flashData* data = (flashData*)param;

	setResponse(data, flashProcessing);

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
		setResponse(data, flashFailedToLoadFile);
		return 0;
	}

	setResponse(data, flashOptimizing);

	uint8_t bankId = 0;

	uint8_t slotsNeeded = (uint8_t)(bankData->size >> 18);
	settingsManager::optimizeBanks(slotsNeeded);
	if (settingsManager::tryGetFreeBank(slotsNeeded, bankId) == false)
	{
		setResponse(data, flashNotEnoughSlots);
		delete(bankData);
		return 0;
	}

	setResponse(data, flashErasing);
	settingsManager::eraseBank(bankId, bankData->size);
	
	setResponse(data, flashWriting);
	settingsManager::writeBank(bankId, bankData, data->bankName, data->ledColor);

	setResponse(data, flashVerifying);
	if (settingsManager::verifyBank(bankId, bankData) == false)
	{
		setResponse(data, flashVerificationFailed);
		delete(bankData);
		return 0;
	}

	setResponse(data, flashDone);
	delete(bankData);

	return 0;
}

void flash::setResponse(flashData* data, flashResponse response)
{
	EnterCriticalSection(&data->mutex);
	data->response = response;
	LeaveCriticalSection(&data->mutex);
}
