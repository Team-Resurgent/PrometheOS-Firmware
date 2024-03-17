#include "launchScene.h"
#include "audioSettingsScene.h"
#include "videoSettingsScene.h"
#include "mainScene.h"
#include "sceneManager.h"

#include "..\context.h"
#include "..\drawing.h"
#include "..\component.h"
#include "..\ssfn.h"
#include "..\inputManager.h"
#include "..\settingsManager.h"
#include "..\hdmiDevice.h"
#include "..\stringUtility.h"
#include "..\alignment.h"
#include "..\theme.h"

launchScene::launchScene()
{
	mSelectedControl = 0;
}

void launchScene::update()
{
	// Exit Action

	if (inputManager::buttonPressed(ButtonB))
	{
		sceneManager::popScene();
		return;
	}

	pointerVector* banks = settingsManager::getBankInfos();

	// Toggle Auto Boot Action

	if (inputManager::buttonPressed(ButtonY))
	{
		for (uint32_t i = 0; i < banks->count(); i++)
		{
			if (mSelectedControl != i)
			{
				continue;
			}
			bankDetails* bank = (bankDetails*)banks->get(i);
			settingsManager::toggleAutoBootBank(bank->id);
		}
	}

	// Select Actions

	if (inputManager::buttonPressed(ButtonA))
	{
		for (uint32_t i = 0; i < banks->count(); i++)
		{
			if (mSelectedControl != i)
			{
				continue;
			}
			bankDetails* bank = (bankDetails*)banks->get(i);
			settingsManager::launchBank(bank->id);
		}
		if (mSelectedControl == banks->count())
		{
			settingsManager::launchTsop();
		}
		else if (mSelectedControl == banks->count() + 1)
		{
			settingsManager::launchRecovery();
		}
	}

	uint32_t bankCount = banks->count() + 1;
	bankCount += context::getModchip()->supportsRecovery() ? 1 : 0;

	// Down Actions

	if (inputManager::buttonPressed(ButtonDpadDown))
	{
		mSelectedControl = mSelectedControl < (int)(bankCount - 1) ? mSelectedControl + 1 : 0;
	}

	// Up Actions

	if (inputManager::buttonPressed(ButtonDpadUp))
	{
		mSelectedControl = mSelectedControl > 0 ? mSelectedControl - 1 : (int)(bankCount - 1); 
	}

	delete(banks);
}

void launchScene::render()
{
	component::panel(theme::getPanelFillColor(), theme::getPanelStrokeColor(), 16, 16, 688, 448);
	drawing::drawBitmapStringAligned(context::getBitmapFontMedium(), "Please select a bank to launch...", theme::getHeaderTextColor(), theme::getHeaderAlign(), 40, theme::getHeaderY(), 640);

	pointerVector* banks = settingsManager::getBankInfos();
	
	uint32_t bankCount = banks->count() + 1;
	bankCount += context::getModchip()->supportsRecovery() ? 1 : 0;

	int32_t yPos = (context::getBufferHeight() - ((bankCount * 40) - 10)) / 2;
	yPos += theme::getCenterOffset();

	for (uint32_t i = 0; i < banks->count(); i++)
	{
		bankDetails* bank = (bankDetails*)banks->get(i);
		char* bankName = stringUtility::formatString(bank->autoBoot ? "* %s *" : "%s", bank->name);
		component::button(mSelectedControl == i, false, bankName, 40, yPos, 640, 30);
		free(bankName);
		yPos += 40;
	}

	component::button(mSelectedControl == banks->count(), false, "TSOP", 40, yPos, 640, 30);

	if (context::getModchip()->supportsRecovery() == true)
	{
		yPos += 40;
		component::button(mSelectedControl == banks->count() + 1, false, "Recovery", 40, yPos, 640, 30);
	}

	delete(banks);

	drawing::drawBitmapString(context::getBitmapFontSmall(), "\xC2\xA1 Select \xC2\xA4 Toggle Auto Boot", theme::getFooterTextColor(), 40, theme::getFooterY());
	drawing::drawBitmapStringAligned(context::getBitmapFontSmall(), "\xC2\xA2 Cancel", theme::getFooterTextColor(), horizAlignmentRight, 40, theme::getFooterY(), 640);
}
