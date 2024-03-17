#pragma once

#include "..\xboxInternals.h"

class lcdRender
{
public:

	typedef struct lcdRenderData {
		bool requestStop;
		CRITICAL_SECTION mutex;
	} lcdRenderData;

	static bool startThread();
	static bool completed();
	static void closeThread();
	static void waitStop();

private:

	static uint64_t WINAPI process(void* param);
};
