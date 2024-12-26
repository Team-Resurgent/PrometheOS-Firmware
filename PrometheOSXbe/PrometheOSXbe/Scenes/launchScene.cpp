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
#include "..\hdmiDevice.h"
#include "..\stringUtility.h"
#include "..\alignment.h"
#include "..\theme.h"

launchScene::launchScene()
{
	mSelectedControl = 0;
	setupBanks();
}

launchScene::~launchScene()
{
	delete(mBanks);
}

void launchScene::setupBanks() {
	mBanks = settingsManager::getBankInfos();

	bankDetails* tsopBank = new bankDetails();
	tsopBank->id = (uint8_t)0;
	tsopBank->ledColor = 0;
	tsopBank->slots = 0;
	tsopBank->name = strdup("TSOP");
	tsopBank->autoBoot = false;
	mBanks->add(tsopBank);

	if (context::getModchip()->getSupportInfo(true).supportsRecovery == true)
	{
		bankDetails* recoveryBank = new bankDetails();
		recoveryBank->id = (uint8_t)0;
		recoveryBank->ledColor = 0;
		recoveryBank->slots = 0;
		recoveryBank->name = strdup("Recovery");
		recoveryBank->autoBoot = false;
		mBanks->add(recoveryBank);
	}
}

void launchScene::update()
{
	// Exit Action

	bool supportsRecovery = context::getModchip()->getSupportInfo(true).supportsRecovery;
	uint32_t cnt = mBanks->count() - 1;
	if (supportsRecovery) cnt -= 1;

	if (inputManager::buttonPressed(ButtonB))
	{
		sceneManager::popScene();
		return;
	}

	// Toggle Auto Boot Action

	if (inputManager::buttonPressed(ButtonY) && (uint32_t)mSelectedControl < cnt)
	{
		for (uint32_t i = 0; i < cnt; i++)
		{
			bankDetails* bank = mBanks->get(i);

			if (mSelectedControl != i)
			{
				bank->autoBoot = false;
				continue;
			}

			settingsManager::toggleAutoBootBank(bank->id);
			bank->autoBoot = settingsManager::getBankInfo(bank->id).autoBoot == 1;
		}
	}

	// Select Actions

	if (inputManager::buttonPressed(ButtonA))
	{
		for (uint32_t i = 0; i < cnt; i++)
		{
			if (mSelectedControl != i)
			{
				continue;
			}
			bankDetails* bank = mBanks->get(i);
			settingsManager::launchBank(bank->id);
		}
		if (mSelectedControl == cnt)
		{
			settingsManager::launchTsop();
		}
		else if (mSelectedControl == cnt + 1)
		{
			settingsManager::launchRecovery();
		}
	}

	uint32_t bankCount = cnt + 1;
	if(supportsRecovery) bankCount += 1;

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
}

void launchScene::render()
{
	component::panel(theme::getPanelFillColor(), theme::getPanelStrokeColor(), 16, 16, 688, 448);
	drawing::drawBitmapStringAligned(context::getBitmapFontMedium(), "Please select a bank to launch...", theme::getHeaderTextColor(), theme::getHeaderAlign(), 40, theme::getHeaderY(), 640);

	int32_t maxItems = 7;

	int32_t start = 0;
	if ((int32_t)mBanks->count() >= maxItems)
	{
		start = min(max(mSelectedControl - (maxItems / 2), 0), (int32_t)mBanks->count() - maxItems);
	}

	int32_t itemCount = min(start + maxItems, (int32_t)mBanks->count()) - start; 
	if (itemCount > 0)
	{
		uint32_t yPos = (context::getBufferHeight() - ((itemCount * 40) - 10)) / 2;
		yPos += theme::getCenterOffset();

		for (int32_t i = 0; i < itemCount; i++)
		{
			uint32_t index = start + i;
			bankDetails* bank = mBanks->get(index);
			char* bankName = stringUtility::formatString(bank->autoBoot ? "* %s *" : "%s", bank->name);
			component::button(mSelectedControl == index, false, bankName, 40, yPos, 640, 30);
			free(bankName);
			yPos += 40;
		}
	}
	else
	{
		int yPos = ((context::getBufferHeight() - 44) / 2);
		yPos += theme::getCenterOffset();

		component::textBox("No items", false, false, horizAlignmentCenter, 40, yPos, 640, 44);
	}

	drawing::drawBitmapString(context::getBitmapFontSmall(), "\xC2\xA1 Select \xC2\xA4 Toggle Auto Boot", theme::getFooterTextColor(), 40, theme::getFooterY());
	drawing::drawBitmapStringAligned(context::getBitmapFontSmall(), "\xC2\xA2 Cancel", theme::getFooterTextColor(), horizAlignmentRight, 40, theme::getFooterY(), 640);
}
