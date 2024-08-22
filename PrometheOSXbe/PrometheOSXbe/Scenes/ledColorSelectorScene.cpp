#include "ledColorSelectorScene.h"
#include "sceneManager.h"
#include "launchScene.h"
#include "removeScene.h"
#include "audioSettingsScene.h"
#include "mainScene.h"

#include "..\context.h"
#include "..\drawing.h"
#include "..\component.h"
#include "..\ssfn.h"
#include "..\inputManager.h"
#include "..\settingsManager.h"
#include "..\hdmiDevice.h"
#include "..\stringUtility.h"
#include "..\xboxInternals.h"
#include "..\theme.h"

ledColorSelectorScene::ledColorSelectorScene()
{
	mSelectedControl = 4;
	mLedColor = mSelectedControl;
	context::getModchip()->setLedColor(mSelectedControl);
}

void ledColorSelectorScene::update()
{
	// Cancel Action

	if (inputManager::buttonPressed(ButtonB))
	{
		context::getModchip()->setLedColor(settingsManager::getLedColor());
		sceneManager::popScene(sceneResultCancelled);
		return;
	}

	// Select Actions

	if (inputManager::buttonPressed(ButtonA))
	{
		mLedColor = mSelectedControl;
		sceneManager::popScene(sceneResultDone);
		return;
	}
	
	// Left Actions

	if (inputManager::buttonPressed(ButtonDpadLeft))
	{
		if (mSelectedControl >= 0 && mSelectedControl <= 3)
		{
			mSelectedControl = mSelectedControl > 0 ? mSelectedControl - 1 : 3; 
		}
		else if (mSelectedControl >= 4 && mSelectedControl <= 7)
		{
			mSelectedControl = mSelectedControl > 4 ? mSelectedControl - 1 : 7; 
		}
		context::getModchip()->setLedColor(mSelectedControl);
	}

	// Right Actions

	if (inputManager::buttonPressed(ButtonDpadRight))
	{
		if (mSelectedControl >= 0 && mSelectedControl <= 3)
		{
			mSelectedControl = mSelectedControl < 3 ? mSelectedControl + 1 : 0;
		}
		else if (mSelectedControl >= 4 && mSelectedControl <= 7)
		{
			mSelectedControl = mSelectedControl < 7 ? mSelectedControl + 1 : 4;
		}
		context::getModchip()->setLedColor(mSelectedControl);
	}

	// Down Actions

	if (inputManager::buttonPressed(ButtonDpadDown))
	{
		if (mSelectedControl >= 0 && mSelectedControl <= 3)
		{
			mSelectedControl += 4;
		}
		else if (mSelectedControl >= 4 && mSelectedControl <= 7)
		{
			mSelectedControl -= 4;
		}
		context::getModchip()->setLedColor(mSelectedControl);
	}

	// Up Actions

	if (inputManager::buttonPressed(ButtonDpadUp))
	{
		if (mSelectedControl >= 0 && mSelectedControl <= 3)
		{
			mSelectedControl += 4;
		}
		else if (mSelectedControl >= 4 && mSelectedControl <= 7)
		{
			mSelectedControl -= 4;
		}
		context::getModchip()->setLedColor(mSelectedControl);
	}
}

void ledColorSelectorScene::render()
{
	component::panel(theme::getPanelFillColor(), theme::getPanelStrokeColor(), 16, 16, 688, 448);
	drawing::drawBitmapStringAligned(context::getBitmapFontMedium(), "Please select a LED Color...", theme::getHeaderTextColor(), theme::getHeaderAlign(), 40, theme::getHeaderY(), 640);

	int32_t ledX = 38;
	int32_t ledY = (context::getBufferHeight() - (2 * 48) - 4) / 2;
	ledY += theme::getCenterOffset();

	component::ledButton(mSelectedControl == 0, 0, "Off", ledX + (162 * 0), ledY + (48 * 0), 158, 44);
	component::ledButton(mSelectedControl == 1, 1, "Red", ledX + (162 * 1), ledY + (48 * 0), 158, 44);
	component::ledButton(mSelectedControl == 2, 2, "Green", ledX + (162 * 2), ledY + (48 * 0), 158, 44);
	component::ledButton(mSelectedControl == 3, 3, "Yellow", ledX + (162 * 3), ledY + (48 * 0), 158, 44);

	component::ledButton(mSelectedControl == 4, 4, "Blue", ledX + (162 * 0), ledY + (48 * 1), 158, 44);
	component::ledButton(mSelectedControl == 5, 5, "Purple", ledX + (162 * 1), ledY + (48 * 1), 158, 44);
	component::ledButton(mSelectedControl == 6, 6, "Turquoise", ledX + (162 * 2), ledY + (48 * 1), 158, 44);
	component::ledButton(mSelectedControl == 7, 7, "White", ledX + (162 * 3), ledY + (48 * 1), 158, 44);

	drawing::drawBitmapString(context::getBitmapFontSmall(), "\xC2\xA1 Select", theme::getFooterTextColor(), 40, theme::getFooterY());
	drawing::drawBitmapStringAligned(context::getBitmapFontSmall(), "\xC2\xA2 Cancel", theme::getFooterTextColor(), horizAlignmentRight, 40, theme::getFooterY(), 640);
}

uint8_t ledColorSelectorScene::getLedColor()
{
	return mLedColor;
}