#pragma once

#include "..\xboxInternals.h"

class hddFormat
{
public:
	
	enum hddFormatResponse
	{ 
		hddFormatResponseNone = 0,
		hddFormatResponseDone = 1,
		hddFormatResponseFailure = 2
	}; 

	typedef struct hddFormatData {
		hddFormatResponse response;
		int32_t harddrive;
		CRITICAL_SECTION mutex;
	} hddFormatData;

	static bool startThread(int32_t harddrive);
	static bool completed();
	static hddFormatResponse getResponse();
	static void closeThread();

private:

	static uint64_t WINAPI process(void* param);
};
