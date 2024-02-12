#include "lcdRender.h"

#include "..\xboxInternals.h"
#include "..\stringUtility.h"
#include "..\fileSystem.h"
#include "..\utils.h"
#include "..\settingsManager.h"
#include "..\lcd.h"

namespace
{
	HANDLE mThread;
	lcdRender::lcdRenderData mData;
}

bool lcdRender::startThread()
{
	memset(&mData, 0, sizeof(lcdRenderData));
	mData.title = strdup("");
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

void lcdRender::setTitle(const char* title)
{
	EnterCriticalSection(&mData.mutex);
	free(mData.title);
	mData.title = strdup(title);
	LeaveCriticalSection(&mData.mutex);
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
	bool enabled = settingsManager::getLcdEnabled();
	uint8_t backlight = settingsManager::getLcdBacklight();
	uint8_t contrast = settingsManager::getLcdBacklight();

	bool initialized = false;
	bool requestStop = false;
	while (requestStop == false)
	{
		EnterCriticalSection(&data->mutex);
		requestStop = data->requestStop;
		char* title = strdup(data->title);
		LeaveCriticalSection(&data->mutex);

		if (settingsManager::getLcdEnabled())
		{
			if (initialized == false)
			{
				initialized = true;
				lcd::init(backlight, contrast);
				Sleep(500);
				for (int j = 0; j < 4; j++)
				{
					lcd::setCursorPosition(0, 3 - j);
					for (int i = 0; i < 20; i++)
					{
						lcd::printMessage(" ");
					}
				}
			}

			if (stringUtility::equals(titleToDisplay, title, false) == false)
			{
				free(titleToDisplay);
				titleToDisplay = strdup(title);

				char* titleLine = strlen(titleToDisplay) > 20 ? stringUtility::substr(titleToDisplay, 0, 20) : strdup(titleToDisplay);
				lcd::setCursorPosition(0, 0);
				lcd::printMessage(titleLine);

				int lineLen = strlen(titleLine);
				if (lineLen < 20)
				{
					for (int i = 0; i < (20 - lineLen); i++)
					{
						lcd::printMessage(" ");
					}
				}
				free(titleLine);
			}

			if (backlight != settingsManager::getLcdBacklight())
			{
				backlight = settingsManager::getLcdBacklight();
				lcd::setBacklight(backlight);
			}

			if (contrast != settingsManager::getLcdContrast())
			{
				contrast = settingsManager::getLcdContrast();
				lcd::setContrast(contrast);
			}
		}

		free(title);
		Sleep(200);
	}

	EnterCriticalSection(&data->mutex);
	free(data->title);
	LeaveCriticalSection(&data->mutex);
	free(titleToDisplay);

	lcd::reboot();

	return 0;
}
