#include "mainScene.h"
#include "sceneManager.h"
#include "menuScene.h"
#include "launchScene.h"

#include "..\context.h"
#include "..\drawing.h"
#include "..\component.h"
#include "..\ssfn.h"
#include "..\inputManager.h"
#include "..\settingsManager.h"
#include "..\hdmiDevice.h"
#include "..\xenium.h"
#include "..\stringUtility.h"
#include "..\socketUtility.h"
#include "..\ftpServer.h"
#include "..\httpServer.h"
#include "..\temperatureManager.h"
#include "..\xboxConfig.h"
#include "..\theme.h"
#include "..\network.h"

mainScene::mainScene()
{
	mSelectedControl = 0;
	mIpAddress = strdup("0.0.0.0");
	mFreeMemory = strdup("");
	mCounter = 0; 
}

mainScene::~mainScene()
{
	free(mIpAddress);
}

void mainScene::update()
{
	// Select Actions

	if (inputManager::buttonPressed(ButtonA))
	{
		if (mSelectedControl == 0) 
		{
			sceneManager::setScene(new launchScene());
			return;
		}
		else if (mSelectedControl == 1) 
		{
			sceneManager::openScene(sceneItemBankManagementScene);
			return;
		}
		else if (mSelectedControl == 2) 
		{
			sceneManager::openScene(sceneItemSystemSettingsScene);
			return;
		}
		else if (mSelectedControl == 3) 
		{
			utils::reboot();
			return;
		}
		else if (mSelectedControl == 4) 
		{
			utils::shutdown();
			return;
		}
	}

	// Down Actions

	if (inputManager::buttonPressed(ButtonDpadDown))
	{
		mSelectedControl = mSelectedControl < 4 ? mSelectedControl + 1 : 0;
	}

	// Up Actions

	if (inputManager::buttonPressed(ButtonDpadUp))
	{
		mSelectedControl = mSelectedControl > 0 ? mSelectedControl - 1 : 4; 
	}

	network::init();

	if (network::isReady() == true)
	{
		XNADDR addr;
		memset(&addr, 0, sizeof(addr));
		DWORD dwState = XNetGetTitleXnAddr(&addr);
		if (dwState != XNET_GET_XNADDR_PENDING)
		{
			char* ipAddress = (XNetGetEthernetLinkStatus() & XNET_ETHERNET_LINK_ACTIVE) ? stringUtility::formatString("%i.%i.%i.%i", 
				addr.ina.S_un.S_un_b.s_b1,
				addr.ina.S_un.S_un_b.s_b2,
				addr.ina.S_un.S_un_b.s_b3,
				addr.ina.S_un.S_un_b.s_b4) : strdup("0.0.0.0");
			if (stringUtility::equals(ipAddress, mIpAddress, false) == false)
			{
				free(mIpAddress);
				mIpAddress = strdup(ipAddress);
				mCounter = 0;
			}
			free(ipAddress);
		}
	}
}

void mainScene::render()
{
	if (mCounter == 0)
	{
		free(mFreeMemory);
		mFreeMemory = stringUtility::formatSize(utils::getFreePhysicalMemory());
		mCpuTempReading = temperatureManager::getCpuTemp();
		mMbTempReading = temperatureManager::getMbTemp();
		mCounter = 60;
	}
	else
	{
		mCounter--;
	}

	drawing::clearBackground();
	component::panel(theme::getPanelFillColor(), theme::getPanelStrokeColor(), 16, 16, 688, 448);
	drawing::drawBitmapStringAligned(context::getBitmapFontLarge(), "\xC2\xA7\xC2\xA8\xC2\xA9\xC2\xAA\xC2\xAB\xC2\xAC\xC2\xAD\xC2\xAB\xC2\xA9\xC2\xAE", theme::getPrometheosColor(), theme::getPrometheosAlign(), 40, theme::getPrometheosY(), 640);

	int32_t yPos = (context::getBufferHeight() - (5 * 40) - 10) / 2;
	yPos += theme::getCenterOffset();

	component::button(mSelectedControl == 0, false, "Launch Bank", 193, yPos, 322, 30);

	yPos += 40;

	component::button(mSelectedControl == 1, false, "Bank Management", 193, yPos, 322, 30);

	yPos += 40;

	component::button(mSelectedControl == 2, false, "System Settings", 193, yPos, 322, 30);

	yPos += 40;

	component::button(mSelectedControl == 3, false, "Reboot", 193, yPos, 322, 30);

	yPos += 40;

	component::button(mSelectedControl == 4, false, "Shutdown", 193, yPos, 322, 30);

	char* ip = stringUtility::formatString("IP %s", mIpAddress);
	drawing::drawBitmapStringAligned(context::getBitmapFontSmall(), ip, theme::getFooterTextColor(), horizAlignmentCenter, 60 + (150 * 0), theme::getFooterY(), 150);
	free(ip);

	char* cpuTemp = stringUtility::formatString("CPU Temp %i\xC2\xB1" "C", mCpuTempReading);
	drawing::drawBitmapStringAligned(context::getBitmapFontSmall(), cpuTemp, theme::getFooterTextColor(), horizAlignmentCenter, 60 + (150 * 1), theme::getFooterY(), 150);
	free(cpuTemp);

	char* mbTemp = stringUtility::formatString("M/B Temp %i\xC2\xB1" "C", mMbTempReading);
	drawing::drawBitmapStringAligned(context::getBitmapFontSmall(), mbTemp, theme::getFooterTextColor(), horizAlignmentCenter, 60 + (150 * 2), theme::getFooterY(), 150);
	free(mbTemp);

	char* memory = stringUtility::formatString("Free Mem %s", mFreeMemory);
	drawing::drawBitmapStringAligned(context::getBitmapFontSmall(), memory, theme::getFooterTextColor(), horizAlignmentCenter, 60 + (150 * 3), theme::getFooterY(), 150);
	free(memory);

	//drawing::drawBitmapStringAligned(context::getBitmapFontSmall(), "Team Cerbios + Team Resurgent", theme::getTextColor(), horizAlignmentCenter, 40, theme::getFooterY(), 640);
}
