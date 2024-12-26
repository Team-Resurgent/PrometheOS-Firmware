#include "frontLedSelectorScene.h"
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

frontLedSelectorScene::frontLedSelectorScene(const char* ledSequence)
{
	mSelectedControl = 0;
	mLedSequence[0] = ledSequence[0];
	mLedSequence[1] = ledSequence[1];
	mLedSequence[2] = ledSequence[2];
	mLedSequence[3] = ledSequence[3];
}

void frontLedSelectorScene::update()
{
	// Cancel Action

	if (inputManager::buttonPressed(ButtonB))
	{
		sceneManager::popScene(sceneResultDone);
		return;
	}

	// Select Actions

	if (inputManager::buttonPressed(ButtonA))
	{
		const char colors[5] = { "GRAO" };
		if (mSelectedControl >= 0 && mSelectedControl <= 3)
		{
			mLedSequence[0] = colors[mSelectedControl];
		}
		else if (mSelectedControl >= 4 && mSelectedControl <= 7)
		{
			mLedSequence[1] = colors[mSelectedControl - 4];
		}
		else if (mSelectedControl >= 8 && mSelectedControl <= 11)
		{
			mLedSequence[2] = colors[mSelectedControl - 8];
		}
		else if (mSelectedControl >= 12 && mSelectedControl <= 15)
		{
			mLedSequence[3] = colors[mSelectedControl - 12];
		}
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
		else if (mSelectedControl >= 8 && mSelectedControl <= 11)
		{
			mSelectedControl = mSelectedControl > 8 ? mSelectedControl - 1 : 11; 
		}
		else if (mSelectedControl >= 12 && mSelectedControl <= 15)
		{
			mSelectedControl = mSelectedControl > 12 ? mSelectedControl - 1 : 15; 
		}
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
		else if (mSelectedControl >= 8 && mSelectedControl <= 11)
		{
			mSelectedControl = mSelectedControl < 11 ? mSelectedControl + 1 : 8;
		}
		else if (mSelectedControl >= 12 && mSelectedControl <= 15)
		{
			mSelectedControl = mSelectedControl < 15 ? mSelectedControl + 1 : 12;
		}
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
			mSelectedControl += 4;
		}
		else if (mSelectedControl >= 8 && mSelectedControl <= 11)
		{
			mSelectedControl += 4;
		}
		else if (mSelectedControl >= 12 && mSelectedControl <= 15)
		{
			mSelectedControl -= 12;
		}
	}

	// Up Actions

	if (inputManager::buttonPressed(ButtonDpadUp))
	{
		if (mSelectedControl >= 0 && mSelectedControl <= 3)
		{
			mSelectedControl += 12;
		}
		else if (mSelectedControl >= 4 && mSelectedControl <= 7)
		{
			mSelectedControl -= 4;
		}
		else if (mSelectedControl >= 8 && mSelectedControl <= 11)
		{
			mSelectedControl -= 4;
		}
			else if (mSelectedControl >= 12 && mSelectedControl <= 15)
		{
			mSelectedControl -= 4;
		}
	}
}

void frontLedSelectorScene::render()
{
	component::panel(theme::getPanelFillColor(), theme::getPanelStrokeColor(), 16, 16, 688, 448);
	drawing::drawBitmapStringAligned(context::getBitmapFontMedium(), "Please select a Front LED Colors...", theme::getHeaderTextColor(), theme::getHeaderAlign(), 40, theme::getHeaderY(), 640);

	int32_t ledX = 112;
	int32_t ledY = (context::getBufferHeight() - (4 * 48) - 4) / 2;
	ledY += theme::getCenterOffset();

	component::text("State 1:", false, horizAlignmentLeft, ledX + (100 * 0), ledY + (48 * 0), 96, 44);
	component::button(mSelectedControl == 0, mLedSequence[0] == 'G', "Green", ledX + (100 * 1), ledY + (48 * 0), 96, 44);
	component::button(mSelectedControl == 1, mLedSequence[0] == 'R', "Red", ledX + (100 * 2), ledY + (48 * 0), 96, 44);
	component::button(mSelectedControl == 2, mLedSequence[0] == 'A', "Amber", ledX + (100 * 3), ledY + (48 * 0), 96, 44);
	component::button(mSelectedControl == 3, mLedSequence[0] == 'O', "Off", ledX + (100 * 4), ledY + (48 * 0), 96, 44);

	component::text("State 2:", false, horizAlignmentLeft, ledX + (100 * 0), ledY + (48 * 1), 96, 44);
	component::button(mSelectedControl == 4, mLedSequence[1] == 'G', "Green", ledX + (100 * 1), ledY + (48 * 1), 96, 44);
	component::button(mSelectedControl == 5, mLedSequence[1] == 'R', "Red", ledX + (100 * 2), ledY + (48 * 1), 96, 44);
	component::button(mSelectedControl == 6, mLedSequence[1] == 'A', "Amber", ledX + (100 * 3), ledY + (48 * 1), 96, 44);
	component::button(mSelectedControl == 7, mLedSequence[1] == 'O', "Off", ledX + (100 * 4), ledY + (48 * 1), 96, 44);

	component::text("State 3:", false, horizAlignmentLeft, ledX + (100 * 0), ledY + (48 * 2), 96, 44);
	component::button(mSelectedControl == 8, mLedSequence[2] == 'G', "Green", ledX + (100 * 1), ledY + (48 * 2), 96, 44);
	component::button(mSelectedControl == 9, mLedSequence[2] == 'R', "Red", ledX + (100 * 2), ledY + (48 * 2), 96, 44);
	component::button(mSelectedControl == 10, mLedSequence[2] == 'A', "Amber", ledX + (100 * 3), ledY + (48 * 2), 96, 44);
	component::button(mSelectedControl == 11, mLedSequence[2] == 'O', "Off", ledX + (100 * 4), ledY + (48 * 2), 96, 44);

	component::text("State 4:", false, horizAlignmentLeft, ledX + (100 * 0), ledY + (48 * 3), 96, 44);
	component::button(mSelectedControl == 12, mLedSequence[3] == 'G', "Green", ledX + (100 * 1), ledY + (48 * 3), 96, 44);
	component::button(mSelectedControl == 13, mLedSequence[3] == 'R', "Red", ledX + (100 * 2), ledY + (48 * 3), 96, 44);
	component::button(mSelectedControl == 14, mLedSequence[3] == 'A', "Amber", ledX + (100 * 3), ledY + (48 * 3), 96, 44);
	component::button(mSelectedControl == 15, mLedSequence[3] == 'O', "Off", ledX + (100 * 4), ledY + (48 * 3), 96, 44);

	drawing::drawBitmapString(context::getBitmapFontSmall(), "\xC2\xA1 Select", theme::getFooterTextColor(), 40, theme::getFooterY());
	drawing::drawBitmapStringAligned(context::getBitmapFontSmall(), "\xC2\xA2 Back", theme::getFooterTextColor(), horizAlignmentRight, 40, theme::getFooterY(), 640);
}

char* frontLedSelectorScene::getLedSequence()
{
	return stringUtility::formatString("%c%c%c%c", mLedSequence[0], mLedSequence[1], mLedSequence[2], mLedSequence[3]);
}