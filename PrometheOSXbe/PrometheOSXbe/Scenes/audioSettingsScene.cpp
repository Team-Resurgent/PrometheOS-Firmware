#include "audioSettingsScene.h"
#include "sceneManager.h"

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

audioSettingsScene::audioSettingsScene()
{
	mSelectedControl = 0;
	if (xboxConfig::getAudioModeStereo() == true)
	{
		mSelectedControl = 1;
	}
	if (xboxConfig::getAudioModeSurround() == true)
	{
		mSelectedControl = 2;
	}
}

void audioSettingsScene::update()
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
			xboxConfig::setAudioMode(XC_AUDIO_FLAGS_MONO);
		}
		else if (mSelectedControl == 1)
		{
			xboxConfig::setAudioMode(XC_AUDIO_FLAGS_STEREO);
		}
		else if (mSelectedControl == 2)
		{
			xboxConfig::setAudioMode(XC_AUDIO_FLAGS_SURROUND);
		}
		else if (mSelectedControl == 3)
		{
			xboxConfig::setAudioAC3(false);
		}
		else if (mSelectedControl == 4)
		{
			xboxConfig::setAudioAC3(true);
		}
		else if (mSelectedControl == 5)
		{
			xboxConfig::setAudioDTS(false);
		}
		else if (mSelectedControl == 6)
		{
			xboxConfig::setAudioDTS(true);
		}
	}
	
	// Left Actions

	if (inputManager::buttonPressed(ButtonDpadLeft))
	{
		if (mSelectedControl >= 0 && mSelectedControl <= 2)
		{
			mSelectedControl = mSelectedControl > 0 ? mSelectedControl - 1 : 2; 
		}
		else if (mSelectedControl >= 3 && mSelectedControl <= 4)
		{
			mSelectedControl = mSelectedControl > 3 ? mSelectedControl - 1 : 4; 
		}
		else if (mSelectedControl >= 5 && mSelectedControl <= 6)
		{
			mSelectedControl = mSelectedControl > 5 ? mSelectedControl - 1 : 6; 
		}
	}

	// Right Actions

	if (inputManager::buttonPressed(ButtonDpadRight))
	{
		if (mSelectedControl >= 0 && mSelectedControl <= 2)
		{
			mSelectedControl = mSelectedControl < 2 ? mSelectedControl + 1 : 0;
		}
		else if (mSelectedControl >= 3 && mSelectedControl <= 4)
		{
			mSelectedControl = mSelectedControl < 4 ? mSelectedControl + 1 : 3;
		}
		else if (mSelectedControl >= 5 && mSelectedControl <= 6)
		{
			mSelectedControl = mSelectedControl < 6 ? mSelectedControl + 1 : 5;
		}
	}

	// Down Actions

	if (inputManager::buttonPressed(ButtonDpadDown))
	{
		if (mSelectedControl >= 0 && mSelectedControl <= 2)
		{
			mSelectedControl = 3;
		}
		else if (mSelectedControl >= 3 && mSelectedControl <= 4)
		{
			mSelectedControl += 2;
		}
		else if (mSelectedControl >= 5 && mSelectedControl <= 6)
		{
			mSelectedControl = 0;
		}
	}

	// Up Actions

	if (inputManager::buttonPressed(ButtonDpadUp))
	{
		if (mSelectedControl >= 0 && mSelectedControl <= 2)
		{
			mSelectedControl = 5;
		}
		else if (mSelectedControl >= 3 && mSelectedControl <= 4)
		{
			mSelectedControl = 0;
		}
		else if (mSelectedControl >= 5 && mSelectedControl <= 6)
		{
			mSelectedControl -= 2;
		}
	}
}

void audioSettingsScene::render()
{
	component::panel(theme::getPanelFillColor(), theme::getPanelStrokeColor(), 16, 16, 688, 448);
	drawing::drawBitmapStringAligned(context::getBitmapFontMedium(), "Audio settings...", theme::getHeaderTextColor(), theme::getHeaderAlign(), 40, theme::getHeaderY(), 640);

	int xPos = 117;
	int yPos = (context::getBufferHeight() - (30 + 45 + 30 + 45 + 30 + 30)) / 2;
	yPos += theme::getCenterOffset();
	
	drawing::drawBitmapString(context::getBitmapFontSmall(), "Audio Mode", theme::getTextColor(), xPos, yPos);
	yPos += 30;
	component::button(mSelectedControl == 0, xboxConfig::getAudioModeMono(), "Mono", xPos, yPos, 160, 30);
	component::button(mSelectedControl == 1, xboxConfig::getAudioModeStereo(), "Stereo", xPos + 164, yPos, 160, 30);
	component::button(mSelectedControl == 2, xboxConfig::getAudioModeSurround(), "Surround", xPos + (164 * 2), yPos, 160, 30);
	yPos += 45;
	drawing::drawBitmapString(context::getBitmapFontSmall(), "Dolby Digital (AC3)", theme::getTextColor(), xPos, yPos);
	yPos += 30;
	component::button(mSelectedControl == 3, xboxConfig::getAudioAC3() == false, "Disabled", xPos, yPos, 241, 30);
	component::button(mSelectedControl == 4, xboxConfig::getAudioAC3() == true, "Enabled", xPos + 245, yPos, 241, 30);
	yPos += 45;
	drawing::drawBitmapString(context::getBitmapFontSmall(), "DTS", theme::getTextColor(), xPos, yPos);
	yPos += 30;
	component::button(mSelectedControl == 5, xboxConfig::getAudioDTS() == false, "Disabled", xPos, yPos, 241, 30);
	component::button(mSelectedControl == 6, xboxConfig::getAudioDTS() == true, "Enabled", xPos + 245, yPos, 241, 30);

	drawing::drawBitmapString(context::getBitmapFontSmall(), "\xC2\xA1 Select", theme::getFooterTextColor(), 40, theme::getFooterY());
	drawing::drawBitmapStringAligned(context::getBitmapFontSmall(), "\xC2\xA2 Back", theme::getFooterTextColor(), horizAlignmentRight, 40, theme::getFooterY(), 640);
}
