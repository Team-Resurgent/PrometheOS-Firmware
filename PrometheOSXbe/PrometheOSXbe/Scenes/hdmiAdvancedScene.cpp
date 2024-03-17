#include "hdmiAdvancedScene.h"
#include "sceneManager.h"

#include "..\theme.h"
#include "..\context.h"
#include "..\drawing.h"
#include "..\component.h"
#include "..\inputManager.h"
#include "..\hdmiSettingsManager.h"
#include "..\hdmiDevice.h"

hdmiAdvancedScene::hdmiAdvancedScene()
{
	mSelectedControl = 0;
}

void hdmiAdvancedScene::update()
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
			settings->colorspace = 0;
			hdmiSettingsManager::saveSettings();
		}
		else if (mSelectedControl == 1)
		{
			settings->colorspace = 1;
			hdmiSettingsManager::saveSettings();
		}
		else if (mSelectedControl == 2)
		{
			settings->interpolation = 0;
			hdmiSettingsManager::saveSettings();
		}
		else if (mSelectedControl == 3)
		{
			settings->interpolation = 1;
			hdmiSettingsManager::saveSettings();
		}
		else if (mSelectedControl == 4)
		{
			settings->autoVideoBlank = settings->autoVideoBlank == 0 ? 1 : 0;
			hdmiSettingsManager::saveSettings();
		}
		else if (mSelectedControl == 5)
		{
			settings->autoRegionSwitch = settings->autoRegionSwitch == 0 ? 1 : 0;
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
		else if (mSelectedControl == 4)
		{
			mSelectedControl = 5;
		}
		else if (mSelectedControl == 5)
		{
			mSelectedControl = 0;
		}
	}

	// Up Actions

	if (inputManager::buttonPressed(ButtonDpadUp))
	{
		if (mSelectedControl >= 0 && mSelectedControl <= 1)
		{
			mSelectedControl = 5;
		}
		else if (mSelectedControl >= 2 && mSelectedControl <= 3)
		{
			mSelectedControl = 0;
		}
		else if (mSelectedControl == 4)
		{
			mSelectedControl = 2;
		}
		else if (mSelectedControl == 5)
		{
			mSelectedControl = 4;
		}
	}
}

void hdmiAdvancedScene::render()
{
	component::panel(theme::getPanelFillColor(), theme::getPanelStrokeColor(), 16, 16, 688, 448);
	drawing::drawBitmapStringAligned(context::getBitmapFontMedium(), "HDMI Advanced settings...", theme::getHeaderTextColor(), theme::getHeaderAlign(), 40, theme::getHeaderY(), 640);
	
	int xPos = 117;
	int yPos = (context::getBufferHeight() - (30 + 40 + 30 + 40 + 30 + 40 + 30)) / 2;
	yPos += theme::getCenterOffset();

	hdmiSettingsState* settings = hdmiSettingsManager::getSettings();

	drawing::drawBitmapString(context::getBitmapFontSmall(), "HDMI Colorspace", theme::getTextColor(), xPos, yPos);
	yPos += 30;
	component::button(mSelectedControl == 0, settings->colorspace == 0, "YCBCR", xPos, yPos, 241, 30);
	component::button(mSelectedControl == 1, settings->colorspace == 1, "RGB", xPos + 245, yPos, 241, 30);
	yPos += 40;
	drawing::drawBitmapString(context::getBitmapFontSmall(), "Upscaling Interpolation", theme::getTextColor(), xPos, yPos);
	yPos += 30;
	component::button(mSelectedControl == 2, settings->interpolation == 0, "Bilinear", xPos, yPos, 241, 30);
	component::button(mSelectedControl == 3, settings->interpolation == 1, "Truncate", xPos + 245, yPos, 241, 30);
	yPos += 40;
	drawing::drawBitmapString(context::getBitmapFontSmall(), "Other", theme::getTextColor(), xPos, yPos);
	yPos += 30;
	component::button(mSelectedControl == 4, false, settings->autoVideoBlank == 1 ? "Auto Video Blanking: Enabled" : "Auto Video Blanking: Disabled", xPos, yPos, 488, 30);
	yPos += 40;
	component::button(mSelectedControl == 5, false, settings->autoRegionSwitch == 1 ? "Auto Region Switching: Enabled" : "Auto Region Switching: Disabled", xPos, yPos, 488, 30);

	drawing::drawBitmapString(context::getBitmapFontSmall(), "\xC2\xA1 Select", theme::getFooterTextColor(), 40, theme::getFooterY());
	drawing::drawBitmapStringAligned(context::getBitmapFontSmall(), "\xC2\xA2 Back", theme::getFooterTextColor(), horizAlignmentRight, 40, theme::getFooterY(), 640);
}
