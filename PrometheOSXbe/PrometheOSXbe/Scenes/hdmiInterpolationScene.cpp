#include "hdmiInterpolationScene.h"
#include "hdmiAdvancedScene.h"
#include "sceneManager.h"

#include "..\theme.h"
#include "..\context.h"
#include "..\drawing.h"
#include "..\component.h"
#include "..\inputManager.h"
#include "..\hdmiSettingsManager.h"
#include "..\hdmiDevice.h"

hdmiInterpolationScene::hdmiInterpolationScene()
{
	mSelectedControl = 0;
}

void hdmiInterpolationScene::update()
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
		hdmiSettingsState* settings = hdmiSettingsManager::getSettings();

		if (mSelectedControl == 0)
		{
			settings->interpolationScaleX = 0;
			hdmiSettingsManager::saveSettings();
		}
		else if (mSelectedControl == 1)
		{
			settings->interpolationScaleX = 1;
			hdmiSettingsManager::saveSettings();
		}
		else if (mSelectedControl == 2)
		{
			settings->interpolationScaleY = 0;
			hdmiSettingsManager::saveSettings();
		}
		else if (mSelectedControl == 3)
		{
			settings->interpolationScaleY = 1;
			hdmiSettingsManager::saveSettings();
		}
		else if (mSelectedControl == 4)
		{
			settings->interpolationWeightX = 0;
			hdmiSettingsManager::saveSettings();
		}
		else if (mSelectedControl == 5)
		{
			settings->interpolationWeightX = 1;
			hdmiSettingsManager::saveSettings();
		}
		else if (mSelectedControl == 6)
		{
			settings->interpolationWeightY = 0;
			hdmiSettingsManager::saveSettings();
		}
		else if (mSelectedControl == 7)
		{
			settings->interpolationWeightY = 1;
			hdmiSettingsManager::saveSettings();
		}
	}

	// Left Actions

	if (inputManager::buttonPressed(ButtonDpadLeft))
	{
		if (mSelectedControl >= 0 && mSelectedControl <= 1)
		{
			mSelectedControl = mSelectedControl > 0 ? mSelectedControl - 1 : 1; 
		}
		else if (mSelectedControl >= 2 && mSelectedControl <= 3)
		{
			mSelectedControl = mSelectedControl > 2 ? mSelectedControl - 1 : 3; 
		}
		else if (mSelectedControl >= 4 && mSelectedControl <= 5)
		{
			mSelectedControl = mSelectedControl > 4 ? mSelectedControl - 1 : 5; 
		}
		else if (mSelectedControl >= 6 && mSelectedControl <= 7)
		{
			mSelectedControl = mSelectedControl > 6 ? mSelectedControl - 1 : 7; 
		}
	}

	// Right Actions

	if (inputManager::buttonPressed(ButtonDpadRight))
	{
		if (mSelectedControl >= 0 && mSelectedControl <= 1)
		{
			mSelectedControl = mSelectedControl < 1 ? mSelectedControl + 1 : 0;
		}
		else if (mSelectedControl >= 2 && mSelectedControl <= 3)
		{
			mSelectedControl = mSelectedControl < 3 ? mSelectedControl + 1 : 2;
		}
		else if (mSelectedControl >= 4 && mSelectedControl <= 5)
		{
			mSelectedControl = mSelectedControl < 5 ? mSelectedControl + 1 : 4;
		}
		else if (mSelectedControl >= 6 && mSelectedControl <= 7)
		{
			mSelectedControl = mSelectedControl < 7 ? mSelectedControl + 1 : 6;
		}
	}


	// Down Actions

	if (inputManager::buttonPressed(ButtonDpadDown))
	{
		if (mSelectedControl >= 0 && mSelectedControl <= 1)
		{
			mSelectedControl = 2;
		}
		else if (mSelectedControl >= 2 && mSelectedControl <= 3)
		{
			mSelectedControl = 4;
		}
		else if (mSelectedControl >= 4 && mSelectedControl <= 5)
		{
			mSelectedControl = 6;
		}
		else if (mSelectedControl >= 6 && mSelectedControl <= 7)
		{
			mSelectedControl = 0;
		}
	}

	// Up Actions

	if (inputManager::buttonPressed(ButtonDpadUp))
	{
		if (mSelectedControl >= 0 && mSelectedControl <= 1)
		{
			mSelectedControl = 6;
		}
		else if (mSelectedControl >= 2 && mSelectedControl <= 3)
		{
			mSelectedControl = 0;
		}
		else if (mSelectedControl >= 4 && mSelectedControl <= 5)
		{
			mSelectedControl = 2;
		}
		else if (mSelectedControl >= 6 && mSelectedControl <= 7)
		{
			mSelectedControl = 4;
		}
	}
}

void hdmiInterpolationScene::render()
{
	component::panel(theme::getPanelFillColor(), theme::getPanelStrokeColor(), 16, 16, 688, 448);
	drawing::drawBitmapStringAligned(context::getBitmapFontMedium(), "HDMI Interpolation settings...", theme::getHeaderTextColor(), theme::getHeaderAlign(), 40, theme::getHeaderY(), 640);
	
	int xPos = 117;
	int yPos = (context::getBufferHeight() - (30 + 40 + 40 + 30 + 40 + 30)) / 2;
	yPos += theme::getCenterOffset();

	hdmiSettingsState* settings = hdmiSettingsManager::getSettings();

	drawing::drawBitmapString(context::getBitmapFontSmall(), "Scale", theme::getTextColor(), xPos, yPos);
	yPos += 30;
	component::button(mSelectedControl == 0, settings->interpolationScaleX == 0, "X - Bilinear", xPos, yPos, 241, 30);
	component::button(mSelectedControl == 1, settings->interpolationScaleX == 1, "X - Truncate", xPos + 245, yPos, 241, 30);
	yPos += 40;
	component::button(mSelectedControl == 2, settings->interpolationScaleY == 0, "Y - Bilinear", xPos, yPos, 241, 30);
	component::button(mSelectedControl == 3, settings->interpolationScaleY == 1, "Y - Truncate", xPos + 245, yPos, 241, 30);
	yPos += 40;
	drawing::drawBitmapString(context::getBitmapFontSmall(), "Weight", theme::getTextColor(), xPos, yPos);
	yPos += 30;
	component::button(mSelectedControl == 4, settings->interpolationWeightX == 0, "X - Bilinear", xPos, yPos, 241, 30);
	component::button(mSelectedControl == 5, settings->interpolationWeightX == 1, "X - Truncate", xPos + 245, yPos, 241, 30);
	yPos += 40;
	component::button(mSelectedControl == 6, settings->interpolationWeightY == 0, "Y - Bilinear", xPos, yPos, 241, 30);
	component::button(mSelectedControl == 7, settings->interpolationWeightY == 1, "Y - Truncate", xPos + 245, yPos, 241, 30);

	drawing::drawBitmapString(context::getBitmapFontSmall(), "\xC2\xA1 Select", theme::getFooterTextColor(), 40, theme::getFooterY());
	drawing::drawBitmapStringAligned(context::getBitmapFontSmall(), "\xC2\xA2 Back", theme::getFooterTextColor(), horizAlignmentRight, 40, theme::getFooterY(), 640);
}
