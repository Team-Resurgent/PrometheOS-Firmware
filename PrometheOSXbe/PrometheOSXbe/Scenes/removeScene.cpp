#include "removeScene.h"
#include "sceneManager.h"
#include "audioSettingsScene.h"
#include "videoSettingsScene.h"

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

removeScene::removeScene()
{
	mSelectedControl = 0;
	mBanks = settingsManager::getBankInfos();
}

removeScene::~removeScene()
{
	delete(mBanks);
}

void removeScene::update()
{
	// Back Action

	if (inputManager::buttonPressed(ButtonB))
	{
		sceneManager::popScene();
		return;
	}

	// Select Actions

	if (inputManager::buttonPressed(ButtonA))
	{
		for (uint32_t i = 0; i < mBanks->count(); i++)
		{
			if (mSelectedControl != i)
			{
				continue;
			}
			bankDetails* bank = mBanks->get(i);
			settingsManager::deleteBank(bank->id);
			mBanks->remove(i);
			mSelectedControl = max(min(mSelectedControl, (int)mBanks->count() - 1), 0);
		}
	}

	// Down Actions

	if (inputManager::buttonPressed(ButtonDpadDown))
	{
		mSelectedControl = mSelectedControl < (int)(mBanks->count() - 1) ? mSelectedControl + 1 : 0;
	}

	// Up Actions

	if (inputManager::buttonPressed(ButtonDpadUp))
	{
		mSelectedControl = mSelectedControl > 0 ? mSelectedControl - 1 : (int)(mBanks->count() - 1); 
	}
}

void removeScene::render()
{
	component::panel(theme::getPanelFillColor(), theme::getPanelStrokeColor(), 16, 16, 688, 448);
	drawing::drawBitmapStringAligned(context::getBitmapFontMedium(), "Please select a bank to remove...", theme::getHeaderTextColor(), theme::getHeaderAlign(), 40, theme::getHeaderY(), 640);

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
			component::button(mSelectedControl == index, false, bank->name, 40, yPos, 640, 30);
			yPos += 40;
		}
	}
	else
	{
		int yPos = ((context::getBufferHeight() - 44) / 2);
		yPos += theme::getCenterOffset();

		component::textBox("No items", false, false, horizAlignmentCenter, 40, yPos, 640, 44);
	}

	drawing::drawBitmapString(context::getBitmapFontSmall(), "\xC2\xA1 Select", theme::getFooterTextColor(), 40, theme::getFooterY());
	drawing::drawBitmapStringAligned(context::getBitmapFontSmall(), "\xC2\xA2 Cancel", theme::getFooterTextColor(), horizAlignmentRight, 40, theme::getFooterY(), 640);
}
