#include "autoBootScene.h"
#include "sceneManager.h"
#include "mainScene.h"

#include "..\context.h"
#include "..\drawing.h"
#include "..\component.h"
#include "..\ssfn.h"
#include "..\inputManager.h"
#include "..\settingsManager.h"
#include "..\hdmiDevice.h"
#include "..\xenium.h"
#include "..\stringUtility.h"
#include "..\xboxConfig.h"
#include "..\theme.h"
#include "..\driveManager.h"

autoBootScene::autoBootScene()
{
	mCounter = settingsManager::getAutoBootDelay() * 10;
	utils::setLedStates(SMC_LED_STATES_RED_STATE0 | SMC_LED_STATES_RED_STATE1 | SMC_LED_STATES_RED_STATE2 | SMC_LED_STATES_RED_STATE3);
}

void autoBootScene::update()
{
	uint32_t trayState;
	uint32_t trayStateChangeCount;
	HalReadSMCTrayState(&trayState, &trayStateChangeCount);
	bool safeMode = (trayState == SMC_TRAY_STATE_UNLOADING) || (trayState == SMC_TRAY_STATE_OPEN && trayStateChangeCount > 0);
	if (safeMode == true)
	{
		HalWriteSMBusByte(SMC_SLAVE_ADDRESS, 0x0C, 1);
	}

	// Exit Action

	if (inputManager::buttonPressed(ButtonB) || safeMode)
	{
		utils::setLedStates(SMC_LED_STATES_GREEN_STATE0 | SMC_LED_STATES_GREEN_STATE1 | SMC_LED_STATES_GREEN_STATE2 | SMC_LED_STATES_GREEN_STATE3);
		sceneManager::setScene(new mainScene());
	}
}

void autoBootScene::render()
{
	pointerVector* banks = settingsManager::getBankInfos();

	drawing::clearBackground();
	component::panel(theme::getPanelFillColor(), theme::getPanelStrokeColor(), 16, 16, 688, 448);
	drawing::drawBitmapStringAligned(context::getBitmapFontMedium(), "Auto booting...", theme::getTitleTextColor(), theme::getHeaderAlign(), 40, theme::getHeaderY(), 640);

	int yPos = (context::getBufferHeight() - (40 + 44)) / 2;
	yPos += theme::getCenterOffset();

	for (uint32_t i = 0; i < banks->count(); i++)
	{
		bankDetails* bank = (bankDetails*)banks->get(i);
		if (bank->autoBoot == false)
		{
			continue;
		}
		drawing::drawBitmapStringAligned(context::getBitmapFontMedium(), bank->name, theme::getTextColor(), horizAlignmentCenter, 40, yPos, 640);
	}

	yPos += 40;

	char* progress = stringUtility::formatString("Booting in %.1f seconds", mCounter / 10.0f);
	component::textBox(progress, false, false, horizAlignmentCenter, 193, yPos, 322, 44);
	free(progress);

	if (mCounter == 0)
	{
		for (uint32_t i = 0; i < banks->count(); i++)
		{
			bankDetails* bank = (bankDetails*)banks->get(i);
			if (bank->slots > 0 && bank->autoBoot == true)
			{
				utils::setLedStates(SMC_LED_STATES_GREEN_STATE0 | SMC_LED_STATES_GREEN_STATE1 | SMC_LED_STATES_GREEN_STATE2 | SMC_LED_STATES_GREEN_STATE3);
				settingsManager::launchBank(bank->id);
				break;
			}
		}
	}
	else
	{
		Sleep(100);
		mCounter--;
	}

	drawing::drawBitmapStringAligned(context::getBitmapFontSmall(), "\xC2\xA2 Cancel", theme::getFooterTextColor(), horizAlignmentRight, 40, theme::getFooterY(), 640);
	
	delete(banks);
}
