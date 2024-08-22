#include "flashBackup.h"

#include "..\xboxInternals.h"
#include "..\context.h"
#include "..\stringUtility.h"
#include "..\fileSystem.h"
#include "..\utils.h"

namespace
{
	HANDLE mThread;
	bool mRecovery;
	flashBackup::flashBackupData mData;
}

bool flashBackup::startThread(bool recovery)
{
	mRecovery = recovery;
	memset(&mData, 0, sizeof(flashBackupData));
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

bool flashBackup::completed()
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

flashBackup::flashBackupResponse flashBackup::getResponse()
{
	flashBackupResponse response;
	EnterCriticalSection(&mData.mutex);
	response = mData.response;
	LeaveCriticalSection(&mData.mutex);
	return response;
}

void flashBackup::getProgress(uint32_t& currentStep, uint32_t& totalSteps)
{
	EnterCriticalSection(&mData.mutex);
	currentStep = mData.currentStep;
	totalSteps = mData.totalSteps;
	LeaveCriticalSection(&mData.mutex);
}

void flashBackup::closeThread()
{
	CloseHandle(mThread);
    DeleteCriticalSection(&mData.mutex);
}

uint64_t WINAPI flashBackup::process(void* param) 
{
	flashBackupData* data = (flashBackupData*)param;
	setResponse(data, flashBackupProcessing);

	if (fileSystem::directoryCreate("HDD0-E:\\PrometheOS") == false || fileSystem::directoryCreate("HDD0-E:\\PrometheOS\\Backup") == false)
	{
		setResponse(data, flashBackupFailedToCreateDirectory);
		return 0;
	}

	char* filePath = NULL;
	if (mRecovery == true)
	{
		filePath = fileSystem::combinePath("HDD0-E:\\PrometheOS\\Backup", "Recovery.bin");
	}
	else if (context::getModchipType() == modchipTypeXenium)
	{
		filePath = fileSystem::combinePath("HDD0-E:\\PrometheOS\\Backup", "PrometheOS-Xenium.bin");
	}
	else if (context::getModchipType() == modchipTypeXecuter)
	{
		filePath = fileSystem::combinePath("HDD0-E:\\PrometheOS\\Backup", "PrometheOS-Xecuter.bin");
	}
	else if (context::getModchipType() == modchipTypeAladdin1mb)
	{
		filePath = fileSystem::combinePath("HDD0-E:\\PrometheOS\\Backup", "PrometheOS-Aladdin1mb.bin");
	}
	else if (context::getModchipType() == modchipTypeAladdin2mb)
	{
		filePath = fileSystem::combinePath("HDD0-E:\\PrometheOS\\Backup", "PrometheOS-Aladdin2mb.bin");
	}
	else if (context::getModchipType() == modchipTypeXchanger)
	{
		filePath = fileSystem::combinePath("HDD0-E:\\PrometheOS\\Backup", "PrometheOS-Xchanger.bin");
	}
	else if (context::getModchipType() == modchipTypeModxo)
	{
		filePath = fileSystem::combinePath("HDD0-E:\\PrometheOS\\Backup", "PrometheOS-Modxo.bin");
	}
	else
	{
		filePath = fileSystem::combinePath("HDD0-E:\\PrometheOS\\Backup", "PrometheOS.bin");
	}

	uint32_t fileHandle;
	if (fileSystem::fileOpen(filePath, fileSystem::FileModeWrite, fileHandle) == false)
	{
		setResponse(data, flashBackupFailedToCreateFile);
		return 0;
	}

	free(filePath);

	uint8_t banks = context::getModchip()->getFlashBankCount(mRecovery);
	for (uint8_t i = 0; i < banks; i++)
	{
		EnterCriticalSection(&data->mutex);
		data->currentStep = i + 1;
		LeaveCriticalSection(&data->mutex);

		uint8_t bank = context::getModchip()->getFlashBank(mRecovery, i);
		uint32_t bankSize = context::getModchip()->getBankSize(bank);
		uint32_t memOffset = context::getModchip()->getBankMemOffset(bank);

		fileSystem::fileSeek(fileHandle, fileSystem::FileSeekModeStart, memOffset);

		setResponse(data, flashBackupReading);
		utils::dataContainer* bankData = context::getModchip()->readBank(bank);

		setResponse(data, flashBackupWriting);
		uint32_t bytesWritten;
		if (fileSystem::fileWrite(fileHandle, bankData->data, bankData->size, bytesWritten) == false)
		{
			setResponse(data, flashBackupFailedToWriteFile);
			fileSystem::fileClose(fileHandle);
			delete(bankData);
			return 0;
		}

		delete(bankData);

		if (mRecovery == true)
		{
			context::getModchip()->disableRecovery();
		}
	}

	fileSystem::fileClose(fileHandle);
	setResponse(data, flashBackupDone);

	return 0;
}

void flashBackup::setResponse(flashBackupData* data, flashBackupResponse response)
{
	EnterCriticalSection(&data->mutex);
	data->response = response;
	LeaveCriticalSection(&data->mutex);
}
