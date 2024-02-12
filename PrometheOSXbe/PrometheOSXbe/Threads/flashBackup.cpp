#include "flashBackup.h"

#include "..\xboxInternals.h"
#include "..\stringUtility.h"
#include "..\fileSystem.h"
#include "..\utils.h"
#include "..\settingsManager.h"
#include "..\xenium.h"

namespace
{
	HANDLE mThread;
	flashBackup::flashBackupData mData;
}

bool flashBackup::startThread()
{
	memset(&mData, 0, sizeof(flashBackupData));
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

	if (fileSystem::directoryCreate("E:\\PrometheOS") == false || fileSystem::directoryCreate("E:\\PrometheOS\\Backup") == false)
	{
		setResponse(data, flashBackupFailedToCreateDirectory);
		return 0;
	}

	char* filePath = fileSystem::combinePath("E:\\PrometheOS\\Backup", "PrometheOS.bin");

	uint32_t fileHandle;
	if (fileSystem::fileOpen(filePath, fileSystem::FileModeWrite, fileHandle) == false)
	{
		setResponse(data, flashBackupFailedToCreateFile);
		return 0;
	}

	free(filePath);

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
		fileSystem::fileSeek(fileHandle, fileSystem::FileSeekModeStart, memOffset);

		setResponse(data, flashBackupReading);
		utils::dataContainer* bankData = xenium::readBank(currentBank, 0, bankSize);

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
