#pragma once

#include "..\xboxInternals.h"
#include "..\modchip.h"

class flashUpdate
{
public:

	enum flashUpdateResponse
	{ 
		flashUpdateResponseNone = 0,
		flashUpdateProcessing = 1,
		flashUpdateFailedToLoadFile = 2,
		flashUpdateErasing = 3,
		flashUpdateWriting = 4,
		flashUpdateVerifying = 5,
		flashUpdateVerificationFailed = 6,
		flashUpdateDone = 7
	}; 

	typedef struct flashUpdateData {
		flashUpdateResponse response;
		uint32_t currentStep;
		uint32_t totalSteps;
		char* filePath;
		CRITICAL_SECTION mutex;
	} flashUpdateData;

	static bool startThread(bool recovery, bool update, const char* filePath);
	static bool completed();
	static flashUpdateResponse getResponse();
	static void getProgress(uint32_t& currentStep, uint32_t& totalSteps);
	static void closeThread();

private:

	static void setResponse(flashUpdateData* data, flashUpdateResponse response);

	static uint64_t WINAPI process(void* param);
};
