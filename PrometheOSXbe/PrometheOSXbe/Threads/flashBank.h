#pragma once

#include "..\xboxInternals.h"

class flashBank
{
public:
	
	enum flashBankResponse
	{ 
		flashBankResponseNone = 0,
		flashBankProcessing = 1,
		flashBankFailedToLoadFile = 2,
		flashBankOptimizing = 3,
		flashBankNotEnoughSlots = 4,
		flashBankErasing = 5,
		flashBankWriting = 6,
		flashBankVerifying = 7,
		flashBankVerificationFailed = 8,
		flashBankDone = 9
	}; 

	typedef struct flashBankData {
		flashBankResponse response;
		char* filePath;
		char* bankName;
		uint8_t ledColor;
		CRITICAL_SECTION mutex;
	} flashBankData;

	static bool startThread(const char* filePath, const char* bankName, uint8_t ledColor);
	static bool completed();
	static flashBankResponse getResponse();
	static void closeThread();

private:

	static void setResponse(flashBankData* data, flashBankResponse response);

	static uint64_t WINAPI process(void* param);
};
