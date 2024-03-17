#include "videosettingsscene.h"
#include "sceneManager.h"
#include "audioSettingsScene.h"

#include "..\context.h"
#include "..\drawing.h"
#include "..\component.h"
#include "..\inputManager.h"
#include "..\ssfn.h"
#include "..\xboxConfig.h"
#include "..\settingsManager.h"
#include "..\hdmiDevice.h"
#include "..\stringUtility.h"
#include "..\theme.h"

videoSettingsScene::videoSettingsScene()
{
	mSelectedControl = 0;
	if (xboxConfig::getVideo720p() == true)
	{
		mSelectedControl = 1;
	}
	if (xboxConfig::getVideo1080i() == true)
	{
		mSelectedControl = 2;
	}
}

void videoSettingsScene::update()
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
		if (mSelectedControl == 0)
		{
			xboxConfig::setVideo480p(!xboxConfig::getVideo480p());
		}
		else if (mSelectedControl == 1)
		{
			xboxConfig::setVideo720p(!xboxConfig::getVideo720p());
		}
		else if (mSelectedControl == 2)
		{
			xboxConfig::setVideo1080i(!xboxConfig::getVideo1080i());
		}
		else if (mSelectedControl == 3)
		{
			xboxConfig::setVideoAspectRatio(0);
		}
		else if (mSelectedControl == 4)
		{
			xboxConfig::setVideoAspectRatio(EEPROM_VIDEO_FLAGS_LETTERBOX);
		}
		else if (mSelectedControl == 5)
		{
			xboxConfig::setVideoAspectRatio(EEPROM_VIDEO_FLAGS_WIDESCREEN);
		}
	}

	// Left Actions

	if (inputManager::buttonPressed(ButtonDpadLeft))
	{
		if (mSelectedControl >= 0 && mSelectedControl <= 2)
		{
			mSelectedControl = mSelectedControl > 0 ? mSelectedControl - 1 : 2; 
		}
		else if (mSelectedControl >= 3 && mSelectedControl <= 5)
		{
			mSelectedControl = mSelectedControl > 3 ? mSelectedControl - 1 : 5; 
		}
	}

	// Right Actions

	if (inputManager::buttonPressed(ButtonDpadRight))
	{
		if (mSelectedControl >= 0 && mSelectedControl <= 2)
		{
			mSelectedControl = mSelectedControl < 2 ? mSelectedControl + 1 : 0;
		}
		else if (mSelectedControl >= 3 && mSelectedControl <= 5)
		{
			mSelectedControl = mSelectedControl < 5 ? mSelectedControl + 1 : 3;
		}
	}

	// Down Actions

	if (inputManager::buttonPressed(ButtonDpadDown))
	{
		if (mSelectedControl >= 0 && mSelectedControl <= 2)
		{
			mSelectedControl = 3;
		}
		else if (mSelectedControl >= 3 && mSelectedControl <= 5)
		{
			mSelectedControl = 0;
		}
	}

	// Up Actions

	if (inputManager::buttonPressed(ButtonDpadUp))
	{
		if (mSelectedControl >= 0 && mSelectedControl <= 2)
		{
			mSelectedControl = 3;
		}
		else if (mSelectedControl >= 3 && mSelectedControl <= 5)
		{
			mSelectedControl = 0;
		}
	}
}

void videoSettingsScene::render()
{
	component::panel(theme::getPanelFillColor(), theme::getPanelStrokeColor(), 16, 16, 688, 448);
	drawing::drawBitmapStringAligned(context::getBitmapFontMedium(), "Video settings...", theme::getHeaderTextColor(), theme::getHeaderAlign(), 40, theme::getHeaderY(), 640);

	int xPos = 117;
	int yPos = (context::getBufferHeight() - (30 + 40 + 30 + 30)) / 2;
	yPos += theme::getCenterOffset();

	drawing::drawBitmapString(context::getBitmapFontSmall(), "Video Modes", theme::getTextColor(), xPos, yPos);
	yPos += 30;
	component::button(mSelectedControl == 0, xboxConfig::getVideo480p() == true, "480p", xPos, yPos, 160, 30);
	component::button(mSelectedControl == 1, xboxConfig::getVideo720p() == true, "720p", xPos + 164, yPos, 160, 30);
	component::button(mSelectedControl == 2, xboxConfig::getVideo1080i() == true, "1080i", xPos + (164 * 2), yPos, 160, 30);
	yPos += 40;
	drawing::drawBitmapString(context::getBitmapFontSmall(), "Aspect Ratio", theme::getTextColor(), xPos, yPos);
	yPos += 30;
	component::button(mSelectedControl == 3, xboxConfig::getVideoAspectRatioNormal() == true, "Normal", xPos, yPos, 160, 30);
	component::button(mSelectedControl == 4, xboxConfig::getVideoAspectRatioLetterbox() == true, "Letterbox", xPos + 164, yPos, 160, 30);
	component::button(mSelectedControl == 5, xboxConfig::getVideoAspectRatioWidescreen() == true, "Widescreen", xPos + (164 * 2), yPos, 160, 30);

	drawing::drawBitmapString(context::getBitmapFontSmall(), "\xC2\xA1 Select", theme::getFooterTextColor(), 40, theme::getFooterY());
	drawing::drawBitmapStringAligned(context::getBitmapFontSmall(), "\xC2\xA2 Back", theme::getFooterTextColor(), horizAlignmentRight, 40, theme::getFooterY(), 640);
}
