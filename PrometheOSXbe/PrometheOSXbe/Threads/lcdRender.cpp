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
	char* fanToDisplay = strdup("");
	char* cpuToDisplay = strdup("");

	uint8_t lcdMode = settingsManager::getLcdMode(true);
	uint8_t lcdModel = settingsManager::getLcdModel(true);
	uint8_t lcdAddress = settingsManager::getLcdAddress(true);
	uint8_t backlight = settingsManager::getLcdBacklight(true);
	uint8_t contrast = settingsManager::getLcdBacklight(true);

	int counter = 0;

	bool initialized = false;
	bool requestStop = false;
	while (requestStop == false)
	{
		char* currentTitle = context::getCurrentTitle();
		char* currentIp = context::getCurrentIp();
		char* currentFreeMem = stringUtility::formatSize(context::getCurrentFreeMem());
		char* currentFan = stringUtility::formatString("%i%%", context::getCurrentFanSpeed());
		char* currentCpu = stringUtility::formatString("%ic", context::getCurrentCpuTemp());

		//counter++;
		//if (counter == 10)
		//{
		//	counter = 0;
		//	utils::swapString(&titleToDisplay, "");
		//	utils::swapString(&freeMemToDisplay, "");
		//	utils::swapString(&ipToDisplay, "");
		//	utils::swapString(&fanToDisplay, "");
		//	utils::swapString(&cpuToDisplay, "");
		//}

		uint8_t currentlcdMode = settingsManager::getLcdMode(true);
		if (currentlcdMode != lcdMode)
		{
			lcdMode = currentlcdMode;
			initialized = false;
		}

		uint8_t currentLcdModel = settingsManager::getLcdModel(true);
		if (currentLcdModel != lcdModel)
		{
			lcdModel = currentLcdModel;
			initialized = false;
		}

		uint8_t currentLcdAddress = settingsManager::getLcdAddress(true);
		if (currentLcdAddress != lcdAddress)
		{
			lcdAddress = currentLcdAddress;
			initialized = false;
		}

		if (currentlcdMode > 0)
		{
			if (initialized == false)
			{
				initialized = true;
				context::getModchip()->lcdInit();

				utils::swapString(&titleToDisplay, "");
				utils::swapString(&freeMemToDisplay, "");
				utils::swapString(&ipToDisplay, "");
				utils::swapString(&fanToDisplay, "");
				utils::swapString(&cpuToDisplay, "");

				context::getModchip()->lcdSetCursorPosition(0, 0);
				char* line1 = stringUtility::formatLcdString("", 20);
				context::getModchip()->lcdPrintMessage(line1);
				free(line1);

				context::getModchip()->lcdSetCursorPosition(1, 0);
				char* line2 = stringUtility::formatLcdString("IP:", 20);
				context::getModchip()->lcdPrintMessage(line2);
				free(line2);

				if (context::getModchip()->getSupportInfo(true).supportsLcdInfo == false)
				{
					context::getModchip()->lcdSetCursorPosition(2, 0);
					char* line3 = stringUtility::formatLcdString("Free Mem:", 20);
					context::getModchip()->lcdPrintMessage(line3);
					free(line3);
					
					context::getModchip()->lcdSetCursorPosition(3, 0);
					char* line4 = stringUtility::formatLcdString("FAN:     CPU:", 20);
					context::getModchip()->lcdPrintMessage(line4);
					free(line4);
				}
			}

			if (stringUtility::equals(titleToDisplay, currentTitle, false) == false)
			{
				utils::swapString(&titleToDisplay, currentTitle);

				context::getModchip()->lcdSetCursorPosition(0, 0);
				char* title = stringUtility::formatLcdString(titleToDisplay, 20);
				context::getModchip()->lcdPrintMessage(title);
				free(title);
			}

			if (stringUtility::equals(ipToDisplay, currentIp, false) == false)
			{
				utils::swapString(&ipToDisplay, currentIp);

				if (context::getModchip()->getDisplayDriver(true)->isSpi2Par() == true)
				{
					context::getModchip()->lcdSetCursorPosition(1, 0);
					context::getModchip()->lcdPrintMessage("IP:");
				}

				context::getModchip()->lcdSetCursorPosition(1, 3);
				char* ip = stringUtility::formatLcdString(ipToDisplay, 15);
				context::getModchip()->lcdPrintMessage(ip);
				free(ip);
			}

			if (context::getModchip()->getSupportInfo(true).supportsLcdInfo == false)
			{
				if (stringUtility::equals(freeMemToDisplay, currentFreeMem, false) == false)
				{
					utils::swapString(&freeMemToDisplay, currentFreeMem);

					if (context::getModchip()->getDisplayDriver(true)->isSpi2Par() == true)
					{
						context::getModchip()->lcdSetCursorPosition(2, 0);
						context::getModchip()->lcdPrintMessage("Free Mem:");
					}

					context::getModchip()->lcdSetCursorPosition(2, 9);
					char* freeMem = stringUtility::formatLcdString(freeMemToDisplay, 20 - 9);
					context::getModchip()->lcdPrintMessage(freeMem);
					free(freeMem);
				}

				if (stringUtility::equals(fanToDisplay, currentFan, false) == false)
				{
					utils::swapString(&fanToDisplay, currentFan);

					if (context::getModchip()->getDisplayDriver(true)->isSpi2Par() == true)
					{
						context::getModchip()->lcdSetCursorPosition(3, 0);
						context::getModchip()->lcdPrintMessage("FAN:");
					}

					context::getModchip()->lcdSetCursorPosition(3, 4);
					char* fan = stringUtility::formatLcdString(fanToDisplay, 4);
					context::getModchip()->lcdPrintMessage(fan);
					free(fan);
				}

				if (stringUtility::equals(cpuToDisplay, currentCpu, false) == false)
				{
					utils::swapString(&cpuToDisplay, currentCpu);

					if (context::getModchip()->getDisplayDriver(true)->isSpi2Par() == true)
					{
						context::getModchip()->lcdSetCursorPosition(3, 9);
						context::getModchip()->lcdPrintMessage("CPU:");
					}

					context::getModchip()->lcdSetCursorPosition(3, 13);
					char* cpu = stringUtility::formatLcdString(cpuToDisplay, 3);
					context::getModchip()->lcdPrintMessage(cpu);
					free(cpu);
				}
			}

			if (backlight != settingsManager::getLcdBacklight(true))
			{
				backlight = settingsManager::getLcdBacklight(true);
				context::getModchip()->lcdSetBacklight(backlight);
			}

			if (contrast != settingsManager::getLcdContrast(true))
			{
				contrast = settingsManager::getLcdContrast(true);
				context::getModchip()->lcdSetContrast(contrast);
			}
		}

		utils::freeString(&currentTitle);
		utils::freeString(&currentIp);
		utils::freeString(&currentFreeMem);
		utils::freeString(&currentFan);
		utils::freeString(&currentCpu);

		for (int i = 0; i < 10; i++)
		{
			EnterCriticalSection(&data->mutex);
			requestStop = data->requestStop;
			LeaveCriticalSection(&data->mutex);
			if (requestStop == true)
			{
				break;
			}
			Sleep(100);
		}
	}

	utils::freeString(&titleToDisplay);
	utils::freeString(&ipToDisplay);
	utils::freeString(&freeMemToDisplay);
	utils::freeString(&fanToDisplay);
	utils::freeString(&cpuToDisplay);

	return 0;
}
