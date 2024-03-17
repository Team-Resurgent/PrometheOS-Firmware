#pragma once

#include "..\xboxInternals.h"
#include "..\modchip.h"

class flashBackup
{
public:
	
	enum flashBackupResponse
	{ 
		flashBackupResponseNone = 0,
		flashBackupProcessing = 1,
		flashBackupFailedToCreateDirectory = 2,
		flashBackupFailedToCreateFile = 3,
		flashBackupFailedToWriteFile = 4,
		flashBackupReading = 5,
		flashBackupWriting = 6,
		flashBackupDone = 7
	}; 

	typedef struct flashBackupData {
		flashBackupResponse response;
		uint32_t currentStep;
		uint32_t totalSteps;
		CRITICAL_SECTION mutex;
	} flashBackupData;

	static bool startThread(bool recovery);
	static bool completed();
	static flashBackupResponse getResponse();
	static void getProgress(uint32_t& currentStep, uint32_t& totalSteps);
	static void closeThread();

private:

	static void setResponse(flashBackupData* data, flashBackupResponse response);

	static uint64_t WINAPI process(void* param);
};
