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
#include "..\stringUtility.h"
#include "..\xboxConfig.h"
#include "..\theme.h"
#include "..\driveManager.h"
#include "..\Threads\countdown.h"

autoBootScene::autoBootScene()
{
	utils::setLedStates(SMC_LED_STATES_RED_STATE0 | SMC_LED_STATES_RED_STATE1 | SMC_LED_STATES_RED_STATE2 | SMC_LED_STATES_RED_STATE3);
	countdown::startThread(settingsManager::getAutoBootDelay() * 1000);
}

void autoBootScene::update()
{
	uint32_t trayState = 0;
	uint32_t ejectCount = 0x11223344;
	HalReadSMCTrayState(&trayState, &ejectCount);
	bool safeMode = (trayState == SMC_TRAY_STATE_UNLOADING || ejectCount > 0);

	// Exit Action

	if (inputManager::buttonPressed(ButtonA))
	{
		continueBoot();
	}

	if (inputManager::buttonPressed(ButtonB))
	{
		safeMode = true;
	}

	if (safeMode == true)
	{
		countdown::closeThread();
		HalWriteSMBusByte(SMC_SLAVE_ADDRESS, 0x0C, 1);
		
		utils::setLedStates(SMC_LED_STATES_GREEN_STATE0 | SMC_LED_STATES_GREEN_STATE1 | SMC_LED_STATES_GREEN_STATE2 | SMC_LED_STATES_GREEN_STATE3);
		sceneManager::popScene();
	}
}

void autoBootScene::render()
{
	pointerVector<bankDetails*>* banks = settingsManager::getBankInfos();

	component::panel(theme::getPanelFillColor(), theme::getPanelStrokeColor(), 16, 16, 688, 448);
	drawing::drawBitmapStringAligned(context::getBitmapFontMedium(), "Auto booting...", theme::getHeaderTextColor(), theme::getHeaderAlign(), 40, theme::getHeaderY(), 640);

	int yPos = (context::getBufferHeight() - (40 + 44)) / 2;
	yPos += theme::getCenterOffset();

	for (uint32_t i = 0; i < banks->count(); i++)
	{
		bankDetails* bank = banks->get(i);
		if (bank->autoBoot == false)
		{
			continue;
		}
		drawing::drawBitmapStringAligned(context::getBitmapFontMedium(), bank->name, theme::getTextColor(), horizAlignmentCenter, 40, yPos, 640);
	}

	yPos += 40;

	char* progress = stringUtility::formatString("Booting in %.1f seconds", countdown::getTimeRemaining() / 1000.0f);
	component::textBox(progress, false, false, horizAlignmentCenter, 193, yPos, 322, 44);
	free(progress);

	delete(banks);

	if (countdown::getTimeRemaining() == 0)
	{
		continueBoot();
	}

	drawing::drawBitmapString(context::getBitmapFontSmall(), "\xC2\xA1 Continue", theme::getFooterTextColor(), 40, theme::getFooterY());
	drawing::drawBitmapStringAligned(context::getBitmapFontSmall(), "\xC2\xA2 Cancel", theme::getFooterTextColor(), horizAlignmentRight, 40, theme::getFooterY(), 640);
}

void autoBootScene::continueBoot() {
	countdown::closeThread();

	pointerVector<bankDetails*>* banks = settingsManager::getBankInfos();

	for (uint32_t i = 0; i < banks->count(); i++)
	{
		bankDetails* bank = banks->get(i);
		if (bank->slots > 0 && bank->autoBoot == true)
		{
			utils::setLedStates(SMC_LED_STATES_GREEN_STATE0 | SMC_LED_STATES_GREEN_STATE1 | SMC_LED_STATES_GREEN_STATE2 | SMC_LED_STATES_GREEN_STATE3);
			settingsManager::launchBank(bank->id);
			break;
		}
	}

	delete(banks);
}