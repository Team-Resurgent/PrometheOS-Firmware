#include "lcdRender.h"

#include "..\xboxInternals.h"
#include "..\stringUtility.h"
#include "..\fileSystem.h"
#include "..\utils.h"
#include "..\context.h"
#include "..\settingsManager.h"

namespace
{
	HANDLE mThread;
	lcdRender::lcdRenderData mData;
}

bool lcdRender::startThread()
{
	memset(&mData, 0, sizeof(lcdRenderData));
	mData.requestStop = false;
	  
	InitializeCriticalSection(&mData.mutex);

	mThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)process, (void*)&mData, 0, NULL);
	if (mThread == NULL) {
        return false;
    }
	SetThreadPriority(mThread, THREAD_PRIORITY_HIGHEST);
	return true;
}

bool lcdRender::completed()
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

void lcdRender::closeThread()
{
	EnterCriticalSection(&mData.mutex);
	mData.requestStop = true;
	LeaveCriticalSection(&mData.mutex);
	CloseHandle(mThread);
    DeleteCriticalSection(&mData.mutex);
}

void lcdRender::waitStop()
{
	EnterCriticalSection(&mData.mutex);
	mData.requestStop = true;
	LeaveCriticalSection(&mData.mutex);
	while (completed() == false)
	{
		 Sleep(100);
	}
}

uint64_t WINAPI lcdRender::process(void* param) 
{
	lcdRenderData* data = (lcdRenderData*)param;
	
	char* titleToDisplay = strdup("");
	char* freeMemToDisplay = strdup("");
	char* ipToDisplay = strdup("");
	char* fanCpuToDisplay = strdup("");
	uint8_t lcdEnableType = settingsManager::getLcdEnableType();
	uint8_t backlight = settingsManager::getLcdBacklight();
	uint8_t contrast = settingsManager::getLcdBacklight();

	int counter = 0;

	bool initialized = false;
	bool requestStop = false;
	while (requestStop == false)
	{
		EnterCriticalSection(&data->mutex);
		requestStop = data->requestStop;
		char* currentTitle = context::getCurrentTitle();
		char* tempFreeMem = stringUtility::formatSize(context::getCurrentFreeMem());
		char* currentFreeMem = stringUtility::formatString("Free Mem:%s", tempFreeMem);
		free(tempFreeMem);
		char* tempIp = context::getCurrentIp();
		char* currentIp = stringUtility::formatString("IP:%s", tempIp);
		free(tempIp);
		char* currentFanCpu = stringUtility::formatString("FAN:%i%% CPU:%ic", context::getCurrentFanSpeed(), context::getCurrentCpuTemp());
		LeaveCriticalSection(&data->mutex);

		counter++;
		if (counter == 10)
		{
			counter = 0;
			free(freeMemToDisplay);
			free(ipToDisplay);
			free(fanCpuToDisplay);
			freeMemToDisplay = strdup("");
			ipToDisplay = strdup("");
			fanCpuToDisplay = strdup("");
		}

		uint8_t currentLcdEnableType = settingsManager::getLcdEnableType();
		if (currentLcdEnableType > 0)
		{
			if (currentLcdEnableType != lcdEnableType)
			{
				initialized = false;
			}

			if (initialized == false)
			{
				initialized = true;
				context::getModchip()->lcdInit(backlight, contrast);
				Sleep(500);
				for (int j = 0; j < 4; j++)
				{
					context::getModchip()->lcdSetCursorPosition(3 - j, 0);
					for (int i = 0; i < 20; i++)
					{
						context::getModchip()->lcdPrintMessage(" ");
					}
				}
			}

			if (stringUtility::equals(titleToDisplay, currentTitle, false) == false)
			{
				free(titleToDisplay);
				titleToDisplay = strdup(currentTitle);

				char* titleLine = strlen(titleToDisplay) > 20 ? stringUtility::substr(titleToDisplay, 0, 20) : strdup(titleToDisplay);
				context::getModchip()->lcdSetCursorPosition(0, 0);
				context::getModchip()->lcdPrintMessage(titleLine);

				int lineLen = strlen(titleLine);
				if (lineLen < 20)
				{
					for (int i = 0; i < (20 - lineLen); i++)
					{
						context::getModchip()->lcdPrintMessage(" ");
					}
				}
				free(titleLine);
			}

			if (context::getModchip()->supportsLcdInfo() == false)
			{
				if (stringUtility::equals(freeMemToDisplay, currentFreeMem, false) == false)
				{
					free(freeMemToDisplay);
					freeMemToDisplay = strdup(currentFreeMem);

					char* freeMemLine = strlen(freeMemToDisplay) > 20 ? stringUtility::substr(freeMemToDisplay, 0, 20) : strdup(freeMemToDisplay);
					context::getModchip()->lcdSetCursorPosition(1, 0);
					context::getModchip()->lcdPrintMessage(freeMemLine);

					int lineLen = strlen(freeMemLine);
					if (lineLen < 20)
					{
						for (int i = 0; i < (20 - lineLen); i++)
						{
							context::getModchip()->lcdPrintMessage(" ");
						}
					}
					free(freeMemLine);
				}

				if (stringUtility::equals(ipToDisplay, currentIp, false) == false)
				{
					free(ipToDisplay);
					ipToDisplay = strdup(currentIp);

					char* ipLine = strlen(ipToDisplay) > 20 ? stringUtility::substr(ipToDisplay, 0, 20) : strdup(ipToDisplay);
					context::getModchip()->lcdSetCursorPosition(2, 0);
					context::getModchip()->lcdPrintMessage(ipLine);

					int lineLen = strlen(ipLine);
					if (lineLen < 20)
					{
						for (int i = 0; i < (20 - lineLen); i++)
						{
							context::getModchip()->lcdPrintMessage(" ");
						}
					}
					free(ipLine);
				}

				if (stringUtility::equals(fanCpuToDisplay, currentFanCpu, false) == false)
				{
					free(fanCpuToDisplay);
					fanCpuToDisplay = strdup(currentFanCpu);

					char* fanCpuLine = strlen(fanCpuToDisplay) > 20 ? stringUtility::substr(fanCpuToDisplay, 0, 20) : strdup(fanCpuToDisplay);
					context::getModchip()->lcdSetCursorPosition(3, 0);
					context::getModchip()->lcdPrintMessage(fanCpuLine);

					int lineLen = strlen(fanCpuLine);
					if (lineLen < 20)
					{
						for (int i = 0; i < (20 - lineLen); i++)
						{
							context::getModchip()->lcdPrintMessage(" ");
						}
					}
					free(fanCpuLine);
				}
			}

			if (backlight != settingsManager::getLcdBacklight())
			{
				backlight = settingsManager::getLcdBacklight();
				context::getModchip()->lcdSetBacklight(backlight);
			}

			if (contrast != settingsManager::getLcdContrast())
			{
				contrast = settingsManager::getLcdContrast();
				context::getModchip()->lcdSetContrast(contrast);
			}
		}

		free(currentTitle);
		free(currentFreeMem);
		free(currentIp);
		free(currentFanCpu);
		Sleep(200);
	}

	EnterCriticalSection(&data->mutex);
	LeaveCriticalSection(&data->mutex);
	free(titleToDisplay);
	free(freeMemToDisplay);
	free(ipToDisplay);
	free(fanCpuToDisplay);

	return 0;
}
