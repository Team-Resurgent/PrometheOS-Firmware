#include "regionSettingsScene.h"
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

regionSettingsScene::regionSettingsScene()
{
	mSelectedControl = 0;
	if (xboxConfig::getVideoStandardNTSCJ())
	{
		mSelectedControl = 1;
	}
	else if (xboxConfig::getVideoStandardPALI50())
	{
		mSelectedControl = 2;
	}
	else if (xboxConfig::getVideoStandardPALI60())
	{
		mSelectedControl = 3;
	}
}

void regionSettingsScene::update()
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
			xboxConfig::setVideoStandardNTSCM();
		}
		else if (mSelectedControl == 1)
		{
			xboxConfig::setVideoStandardNTSCJ();
		}
		else if (mSelectedControl == 2)
		{
			xboxConfig::setVideoStandardPALI50();
		}
		else if (mSelectedControl == 3)
		{
			xboxConfig::setVideoStandardPALI60();
		}
		else if (mSelectedControl == 4)
		{
			xboxConfig::setGameRegionNorthAmerica();
		}
		else if (mSelectedControl == 5)
		{
			xboxConfig::setGameRegionJapan();
		}
		else if (mSelectedControl == 6)
		{
			xboxConfig::setGameRegionRestOfWorld();
		}
		else if (mSelectedControl == 7)
		{
			xboxConfig::setDvdRegion((xboxConfig::getDvdRegion() + 1) % 7);
		}
	}
	
	// Left Actions

	if (inputManager::buttonPressed(ButtonDpadLeft))
	{
		if (mSelectedControl >= 0 && mSelectedControl <= 3)
		{
			mSelectedControl = mSelectedControl > 0 ? mSelectedControl - 1 : 3; 
		}
		else if (mSelectedControl >= 4 && mSelectedControl <= 6)
		{
			mSelectedControl = mSelectedControl > 4 ? mSelectedControl - 1 : 6; 
		}
	}

	// Right Actions

	if (inputManager::buttonPressed(ButtonDpadRight))
	{
		if (mSelectedControl >= 0 && mSelectedControl <= 3)
		{
			mSelectedControl = mSelectedControl < 3 ? mSelectedControl + 1 : 0;
		}
		else if (mSelectedControl >= 4 && mSelectedControl <= 6)
		{
			mSelectedControl = mSelectedControl < 6 ? mSelectedControl + 1 : 4;
		}
	}

	// Down Actions

	if (inputManager::buttonPressed(ButtonDpadDown))
	{
		if (mSelectedControl >= 0 && mSelectedControl <= 3)
		{
			mSelectedControl = 4;
		}
		else if (mSelectedControl >= 4 && mSelectedControl <= 6)
		{
			mSelectedControl = 7;
		}
		else if (mSelectedControl == 7)
		{
			mSelectedControl = 0;
		}
	}

	// Up Actions

	if (inputManager::buttonPressed(ButtonDpadUp))
	{
		if (mSelectedControl >= 0 && mSelectedControl <= 3)
		{
			mSelectedControl = 7;
		}
		else if (mSelectedControl >= 4 && mSelectedControl <= 6)
		{
			mSelectedControl = 0;
		}
		else if (mSelectedControl == 7)
		{
			mSelectedControl = 4;
		}
	}
}

void regionSettingsScene::render()
{
	component::panel(theme::getPanelFillColor(), theme::getPanelStrokeColor(), 16, 16, 688, 448);
	drawing::drawBitmapStringAligned(context::getBitmapFontMedium(), "Region settings...", theme::getHeaderTextColor(), theme::getHeaderAlign(), 40, theme::getHeaderY(), 640);

	int yPos = (context::getBufferHeight() - (30 + 40 + 30 + 40 + 30 + 30)) / 2;
	yPos += theme::getCenterOffset();

	drawing::drawBitmapString(context::getBitmapFontSmall(), "Video Standard", theme::getTextColor(), 44, yPos);
	yPos += 30;
	component::button(mSelectedControl == 0, xboxConfig::getVideoStandardNTSCM() == true, "NTSC-M", 44 + (159 * 0), yPos, 155, 30);
	component::button(mSelectedControl == 1, xboxConfig::getVideoStandardNTSCJ() == true, "NTSC-J", 44 + (159 * 1), yPos, 155, 30);
	component::button(mSelectedControl == 2, xboxConfig::getVideoStandardPALI50() == true, "PAL-I (50hz)", 44 + (159 * 2), yPos, 155, 30);
	component::button(mSelectedControl == 3, xboxConfig::getVideoStandardPALI60() == true, "PAL-I (60hz)", 44 + (159 * 3), yPos, 155, 30);
	yPos += 40;
	drawing::drawBitmapString(context::getBitmapFontSmall(), "Game Region", theme::getTextColor(), 44, yPos);
	yPos += 30;
	component::button(mSelectedControl == 4, xboxConfig::getGameRegionNorthAmerica() == true, "North America", 44 + (212 * 0), yPos, 208, 30);
	component::button(mSelectedControl == 5, xboxConfig::getGameRegionJapan() == true, "Japan", 44 + (212 * 1), yPos, 208, 30);
	component::button(mSelectedControl == 6, xboxConfig::getGameRegionRestOfWorld() == true, "Rest of World", 44 + (212 * 2), yPos, 208, 30);
	yPos += 40;
	drawing::drawBitmapString(context::getBitmapFontSmall(), "DVD Region", theme::getTextColor(), 44, yPos);
	yPos += 30;
	char* dvdRegionString = xboxConfig::getDvdRegionString();
	component::button(mSelectedControl == 7, false, dvdRegionString, 44, yPos, 632, 30);
	free(dvdRegionString);

	drawing::drawBitmapString(context::getBitmapFontSmall(), "\xC2\xA1 Select", theme::getFooterTextColor(), 40, theme::getFooterY());
	drawing::drawBitmapStringAligned(context::getBitmapFontSmall(), "\xC2\xA2 Back", theme::getFooterTextColor(), horizAlignmentRight, 40, theme::getFooterY(), 640);
}
