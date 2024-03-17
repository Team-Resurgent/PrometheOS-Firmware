#include "hdmiVideoSettingsScene.h"
#include "sceneManager.h"

#include "..\theme.h"
#include "..\context.h"
#include "..\drawing.h"
#include "..\component.h"
#include "..\inputManager.h"
#include "..\hdmiSettingsManager.h"
#include "..\hdmiDevice.h"

hdmiVideoSettingsScene::hdmiVideoSettingsScene()
{
	mSelectedControl = 0;
}

void hdmiVideoSettingsScene::update()
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
            settings->outputResolution = 0;
            hdmiSettingsManager::saveSettings();
        }
        else if (mSelectedControl == 1)
        {
            settings->outputResolution = 1;
            hdmiSettingsManager::saveSettings();
        }
        else if (mSelectedControl == 2)
        {
            settings->widescreen = 0;  // Set to Auto
            hdmiSettingsManager::saveSettings();
        }
        else if (mSelectedControl == 3)
        {
            settings->widescreen = 1;  // Set to Force
            hdmiSettingsManager::saveSettings();
        }
        else if (mSelectedControl == 4)
        {
            settings->widescreen = 2;  // Set to Never
            hdmiSettingsManager::saveSettings();
        }
        else if (mSelectedControl == 5)
        {
            settings->prescaleFixDisable = settings->prescaleFixDisable == 0 ? 1 : 0;
            hdmiSettingsManager::saveSettings();
        }
        else if (mSelectedControl == 6)
        {
            settings->disable480p169 = settings->disable480p169 == 0 ? 1 : 0;
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
		else if (mSelectedControl >= 2 && mSelectedControl <= 4)
		{
			mSelectedControl = mSelectedControl > 2 ? mSelectedControl - 1 : 4; 
		}
	}

	// Right Actions

	if (inputManager::buttonPressed(ButtonDpadRight))
	{
		if (mSelectedControl >= 0 && mSelectedControl <= 1)
		{
			mSelectedControl = mSelectedControl < 1 ? mSelectedControl + 1 : 0;
		}
		else if (mSelectedControl >= 2 && mSelectedControl <= 4)
		{
			mSelectedControl = mSelectedControl < 4 ? mSelectedControl + 1 : 2;
		}
	}

	// Down Actions

	if (inputManager::buttonPressed(ButtonDpadDown))
	{
		if (mSelectedControl >= 0 && mSelectedControl <= 1)
		{
			mSelectedControl = 2;
		}
		else if (mSelectedControl >= 2 && mSelectedControl <= 4)
		{
			mSelectedControl = 5;
		}
		else if (mSelectedControl == 5)
		{
			mSelectedControl = 6;
		}
		else if (mSelectedControl == 6)
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
		else if (mSelectedControl >= 2 && mSelectedControl <= 4)
		{
			mSelectedControl = 0;
		}
		else if (mSelectedControl == 5)
		{
			mSelectedControl = 2;
		}
		else if (mSelectedControl == 6)
		{
			mSelectedControl = 5;
		}
	}
}

void hdmiVideoSettingsScene::render()
{
	component::panel(theme::getPanelFillColor(), theme::getPanelStrokeColor(), 16, 16, 688, 448);
	drawing::drawBitmapStringAligned(context::getBitmapFontMedium(), "HDMI Video settings...", theme::getHeaderTextColor(), theme::getHeaderAlign(), 40, theme::getHeaderY(), 640);

	hdmiSettingsState* settings = hdmiSettingsManager::getSettings();

	int xPos = 117;
	int yPos = (context::getBufferHeight() - (30 + 40 + 30 + 40 + 30 + 40 + 30)) / 2;
	yPos += theme::getCenterOffset();

	drawing::drawBitmapString(context::getBitmapFontSmall(), "Target Resolution (Upscale)", theme::getTextColor(), xPos, yPos);
	yPos += 30;
	component::button(mSelectedControl == 0, settings->outputResolution == 0, "480p", xPos, yPos, 241, 30);
	component::button(mSelectedControl == 1, settings->outputResolution == 1, "720p", xPos + 245, yPos, 241, 30);
	yPos += 40;
	drawing::drawBitmapString(context::getBitmapFontSmall(), "4:3 Widescreen", theme::getTextColor(), xPos, yPos);
	yPos += 30;
	component::button(mSelectedControl == 2, settings->widescreen == 0, "Auto", xPos, yPos, 160, 30);
	component::button(mSelectedControl == 3, settings->widescreen == 1, "Force", xPos + 164, yPos, 160, 30);
	component::button(mSelectedControl == 4, settings->widescreen == 2, "Never", xPos + (164 * 2), yPos, 160, 30);
	yPos += 40;
	drawing::drawBitmapString(context::getBitmapFontSmall(), "Other", theme::getTextColor(), xPos, yPos);
	yPos += 30;
	component::button(mSelectedControl == 5, false, settings->prescaleFixDisable == 0 ? "Pre-scale 480p: Enabled" : "Pre-scale 480p: Disabled", xPos, yPos, 488, 30);
	yPos += 40;
	component::button(mSelectedControl == 6, false, settings->disable480p169 == 0 ? "Assume 16:9 480p: Enabled" : "Assume 16:9 480p: Disabled", xPos, yPos, 488, 30);

	drawing::drawBitmapString(context::getBitmapFontSmall(), "\xC2\xA1 Select", theme::getFooterTextColor(), 40, theme::getFooterY());
	drawing::drawBitmapStringAligned(context::getBitmapFontSmall(), "\xC2\xA2 Back", theme::getFooterTextColor(), horizAlignmentRight, 40, theme::getFooterY(), 640);
}
