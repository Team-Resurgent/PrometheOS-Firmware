#include "flashUpdate.h"

#include "..\modchip.h"
#include "..\modchipXenium.h"
#include "..\context.h"
#include "..\xboxInternals.h"
#include "..\stringUtility.h"
#include "..\fileSystem.h"
#include "..\utils.h"

namespace
{
	HANDLE mThread;
	bool mRecovery;
	bool mUpdate;
	flashUpdate::flashUpdateData mData;
}

bool flashUpdate::startThread(bool recovery, bool update, const char* filePath)
{
	mRecovery = recovery;
	mUpdate = update;
	memset(&mData, 0, sizeof(flashUpdateData));
	mData.filePath = strdup(filePath);
	mData.currentStep = 0;
	mData.totalSteps = context::getModchip()->getFlashBankCount(mRecovery);

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

	uint8_t banks = context::getModchip()->getFlashBankCount(mRecovery);
	for (uint8_t i = 0; i < banks; i++)
	{
		EnterCriticalSection(&data->mutex);
		data->currentStep = i + 1;
		LeaveCriticalSection(&data->mutex);

		uint8_t bank = context::getModchip()->getFlashBank(mRecovery, i);
		uint32_t bankSize = context::getModchip()->getBankSize(bank);
		uint32_t memOffset = context::getModchip()->getBankMemOffset(bank);

		if (context::getModchip()->getFlashBankType(mRecovery, i) == bankTypeSystem || mUpdate == false)
		{
			setResponse(data, flashUpdateErasing);
			context::getModchip()->eraseBank(bank);

			if (memOffset < flashData->size)
			{
				utils::dataContainer* bankData = new utils::dataContainer(flashData->data, bankSize, bankSize);
				memcpy(bankData->data, flashData->data + memOffset, bankSize);

				setResponse(data, flashUpdateWriting);
				context::getModchip()->writeBank(bank, bankData);

				setResponse(data, flashUpdateVerifying);
				if (context::getModchip()->verifyBank(bank, bankData) == false)
				{
					setResponse(data, flashUpdateVerificationFailed);
					delete(flashData);
					return 0;
				}
			}
		}

		if (mRecovery == true)
		{
			context::getModchip()->disableRecovery();
		}
	}

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
