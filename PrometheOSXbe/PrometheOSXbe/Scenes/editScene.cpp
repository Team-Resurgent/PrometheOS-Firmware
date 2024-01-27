#include "editScene.h"
#include "sceneManager.h"
#include "audioSettingsScene.h"
#include "videoSettingsScene.h"
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
#include "..\alignment.h"
#include "..\theme.h"

editScene::editScene()
{
	mSelectedControl = 0;
	mBanks = settingsManager::getBankInfos();
	mBankId = 0;
	mSceneResult = sceneResultNone;
}

editScene::~editScene()
{
	delete(mBanks);
}

void editScene::update()
{
	// Exit Action

	if (inputManager::buttonPressed(ButtonB))
	{
		mSceneResult = sceneResultCancelled;
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
			mBankId = (uint8_t)i;
			mSceneResult = sceneResultDone;
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

void editScene::render()
{
	component::panel(theme::getPanelFillColor(), theme::getPanelStrokeColor(), 16, 16, 688, 448);
	drawing::drawBitmapStringAligned(context::getBitmapFontMedium(), "Please select a bank to edit...", theme::getHeaderTextColor(), theme::getHeaderAlign(), 40, theme::getHeaderY(), 640);

	if (mBanks->count() > 0)
	{
		int32_t yPos = (context::getBufferHeight() - ((mBanks->count() * 40) - 10)) / 2;
		yPos += theme::getCenterOffset();

		for (uint32_t i = 0; i < mBanks->count(); i++)
		{
			bankDetails* bank = (bankDetails*)mBanks->get(i);
			component::button(mSelectedControl == i, false, bank->name, 40, yPos, 640, 30);
			yPos += 40;
		}
	}
	else
	{
		int yPos = ((context::getBufferHeight() - 44) / 2);
		yPos += theme::getCenterOffset();

		component::textBox("No items", false, false, horizAlignmentCenter, 40, 225, 640, 44);
	}
	
	drawing::drawBitmapString(context::getBitmapFontSmall(), "\xC2\xA1 Select", theme::getFooterTextColor(), 40, theme::getFooterY());
	drawing::drawBitmapStringAligned(context::getBitmapFontSmall(), "\xC2\xA2 Cancel", theme::getFooterTextColor(), horizAlignmentRight, 40, theme::getFooterY(), 640);
}

sceneResult editScene::getSceneResult()
{
	return mSceneResult;
}

uint8_t editScene::getBankId()
{
	return mBankId;
}