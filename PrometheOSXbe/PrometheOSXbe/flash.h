#pragma once

#include "xboxInternals.h"

class flash
{
public:
	
	enum flashResponse
	{ 
		flashResponseNone = 0,
		flashProcessing = 1,
		flashFailedToLoadFile = 2,
		flashOptimizing = 3,
		flashNotEnoughSlots = 4,
		flashErasing = 5,
		flashWriting = 6,
		flashVerifying = 7,
		flashVerificationFailed = 8,
		flashDone = 9
	}; 

	typedef struct flashData {
		flashResponse response;
		char* filePath;
		char* bankName;
		uint8_t ledColor;
		CRITICAL_SECTION mutex;
	} flashData;

	static bool startThread(const char* filePath, const char* bankName, uint8_t ledColor);
	static bool completed();
	static flashResponse getResponse();
	static void closeThread();

private:

	static void setResponse(flashData* data, flashResponse response);

	static uint64_t WINAPI process(void* param);
};
