#pragma once

#include "..\xboxInternals.h"

class hddInfo
{
public:
	
	enum hddInfoResponse
	{ 
		hddInfoResponseNone = 0,
		hddInfoResponseUnlocked = 1,
		hddInfoResponseLocked = 2,
		hddInfoResponseFailureEeprom = 3,
		hddInfoResponseTimeout = 4
	}; 

	typedef struct hddInfoData {
		hddInfoResponse response;
		char* model;
		char* serial;
		CRITICAL_SECTION mutex;
	} hddInfoData;

	static bool startThread(bool harddrive = 0);
	static bool completed();
	static hddInfoResponse getResponse();
	static char* getModel();
	static char* getSerial();
	static void closeThread();

private:

	static uint64_t WINAPI process(void* param);
};
