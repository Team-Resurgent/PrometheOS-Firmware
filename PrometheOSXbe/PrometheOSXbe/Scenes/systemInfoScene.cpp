#include "systemInfoScene.h"
#include "sceneManager.h"
#include "removeScene.h"
#include "flashFlowScene.h"
#include "editFlowScene.h"
#include "audioSettingsScene.h"
#include "videoSettingsScene.h"

#include "..\context.h"
#include "..\drawing.h"
#include "..\component.h"
#include "..\ssfn.h"
#include "..\inputManager.h"
#include "..\settingsManager.h"
#include "..\hdmiDevice.h"
#include "..\xenium.h"
#include "..\stringUtility.h"
#include "..\xboxConfig.h"
#include "..\theme.h"

systemInfoScene::systemInfoScene(systemInfoCategoryEnum systemInfoCategory)
{
	mSelectedControl = 0;
	mInfoItems = new pointerVector(false);
	mSystemInfoCategory = systemInfoCategory;
	
	if (mSystemInfoCategory == systemInfoCategoryConsole)
	{
		char* cpuSpeed = stringUtility::formatString("CPU: %uMHz", xboxConfig::getCPUFreq());
		mInfoItems->add(cpuSpeed);

		char* xboxVersionString = xboxConfig::getXboxVersionString();
		char* xboxVersion = stringUtility::formatString("Xbox Rev: %s", xboxVersionString);
		mInfoItems->add(xboxVersion);
		free(xboxVersionString);

		char* totalMemory = stringUtility::formatString("RAM: %uMB", utils::getTotalPhysicalMemory() >> 20);
		mInfoItems->add(totalMemory);

		char* serialString = xboxConfig::getSerialString();
		char* serial = stringUtility::formatString("Serial: %s", serialString);
		mInfoItems->add(serial);
		free(serialString);

		char* macString = xboxConfig::getMacString();
 		char* mac = stringUtility::formatString("Mac: %s", macString);
		mInfoItems->add(mac);
		free(macString);
	}
	else if (mSystemInfoCategory == systemInfoCategoryAudio)
	{
		char* audioModeString = xboxConfig::getAudioModeString();
		char* audioMode = stringUtility::formatString("Audio Mode: %s", audioModeString);
		mInfoItems->add(audioMode);
		free(audioModeString);

		char* ac3 = stringUtility::formatString("Dolby digital (AC3): %s", xboxConfig::getAudioAC3() ? "Enabled" : "Disabled");
		mInfoItems->add(ac3);

		char* dts = stringUtility::formatString("DTS: %s", xboxConfig::getAudioDTS() ? "Enabled" : "Disabled");
		mInfoItems->add(dts);
	}
	else if (mSystemInfoCategory == systemInfoCategoryVideo)
	{
		char* videoStandardString = xboxConfig::getVideoStandardString();
		char* videoStandard = stringUtility::formatString("Video Standard: %s", videoStandardString);
		mInfoItems->add(videoStandard);
		free(videoStandardString);

		char* gameRegionString = xboxConfig::getGameRegionString();
		char* gameRegion = stringUtility::formatString("Game Region: %s", gameRegionString);
		mInfoItems->add(gameRegion);
		free(gameRegionString);

		char* dvdRegionString = xboxConfig::getDvdRegionString();
		char* dvdRegion = stringUtility::formatString("DVD Region: %s", dvdRegionString);
		mInfoItems->add(dvdRegion);
		free(dvdRegionString);

		char* avPackString = xboxConfig::getAvPackString();
		char* avPack = stringUtility::formatString("AV Pack: %s", avPackString);
		free(avPackString);

		char* encoderString = xboxConfig::getEncoderString();
		char* encoder = stringUtility::formatString("Encoder: %s", encoderString);
		mInfoItems->add(encoder);
		free(encoderString);

		char* hdModString = xboxConfig::getHdModString();
		char* hdMod = stringUtility::formatString("HD Mod: %s", hdModString);
		mInfoItems->add(hdMod);
		free(hdModString);
	}
	else if (mSystemInfoCategory == systemInfoCategoryAbout)
	{
		char *versionSemver = settingsManager::getVersionSting(settingsManager::getVersion());
		char *version = stringUtility::formatString("PrometheOS: V%s", versionSemver);
		mInfoItems->add(version);
		free(versionSemver);

		char *by = strdup("Team Cerbios + Team Resurgent");
		mInfoItems->add(by);

		char *coded = strdup("Coded By: EqUiNoX");
		mInfoItems->add(coded);

		char* skinAutthor = theme::getSkinAuthor();
		char *author = stringUtility::formatString("Skin Author: %s", skinAutthor);
		mInfoItems->add(author);
		free(skinAutthor);

		if (drawing::imageExists("installer-logo") == false)
		{
			utils::dataContainer* installerLogoData = settingsManager::getInstallerLogoData();
			if (installerLogoData->data[0] == 'I' && installerLogoData->data[1] == 'M')
			{
				int width = (uint8_t)installerLogoData->data[2];
				int height = (uint8_t)installerLogoData->data[3];
				drawing::addImage("installer-logo", (uint8_t*)(installerLogoData->data + 4), D3DFMT_A8B8G8R8, width, height);
			}
			delete(installerLogoData);
		}
	}
}

systemInfoScene::~systemInfoScene()
{
	delete(mInfoItems);
}

void systemInfoScene::update()
{
	// Exit Action

	if (inputManager::buttonPressed(ButtonB))
	{
		sceneManager::openScene(sceneItemSystemInfoScene);
	}
	
	// Down Actions

	if (inputManager::buttonPressed(ButtonDpadDown))
	{
		if (mInfoItems != NULL)
		{
			mSelectedControl = mSelectedControl < (int)(mInfoItems->count() - 1) ? mSelectedControl + 1 : 0;
		}
	}

	// Up Actions

	if (inputManager::buttonPressed(ButtonDpadUp))
	{
		if (mInfoItems != NULL)
		{
			mSelectedControl = mSelectedControl > 0 ? mSelectedControl - 1 : (int)(mInfoItems->count() - 1); 
		}
	}
}

void systemInfoScene::render()
{
	component::panel(theme::getPanelFillColor(), theme::getPanelStrokeColor(), 16, 16, 688, 448);

	if (mSystemInfoCategory == systemInfoCategoryConsole)
	{
		drawing::drawBitmapStringAligned(context::getBitmapFontMedium(), "System Info: Console", theme::getHeaderTextColor(), theme::getHeaderAlign(), 40, theme::getHeaderY(), 640);
	}
	else if (mSystemInfoCategory == systemInfoCategoryAudio)
	{
		drawing::drawBitmapStringAligned(context::getBitmapFontMedium(), "System Info: Audio", theme::getHeaderTextColor(), theme::getHeaderAlign(), 40, theme::getHeaderY(), 640);
	}
	else if (mSystemInfoCategory == systemInfoCategoryVideo)
	{
		drawing::drawBitmapStringAligned(context::getBitmapFontMedium(), "System Info: Video", theme::getHeaderTextColor(), theme::getHeaderAlign(), 40, theme::getHeaderY(), 640);
	}
	else if (mSystemInfoCategory == systemInfoCategoryAbout)
	{
		drawing::drawBitmapStringAligned(context::getBitmapFontMedium(), "System Info: About", theme::getHeaderTextColor(), theme::getHeaderAlign(), 40, theme::getHeaderY(), 640);
	}

	uint32_t yPos = 96;

	int32_t maxItems = 7;

	int32_t start = 0;
	if ((int32_t)mInfoItems->count() >= maxItems)
	{
		start = min(max(mSelectedControl - (maxItems / 2), 0), (int32_t)mInfoItems->count() - maxItems);
	}

	int32_t itemCount = min(start + maxItems, (int32_t)mInfoItems->count()) - start; 
	if (itemCount > 0)
	{
		uint32_t yPos = (context::getBufferHeight() - ((itemCount * 40) - 10)) / 2;
		yPos += theme::getCenterOffset();

		if (mSystemInfoCategory == systemInfoCategoryAbout && drawing::imageExists("installer-logo") == true)
		{
			yPos += 32;
			drawing::drawImage("installer-logo", theme::getInstallerTint(), 271, yPos - 64, 178, 46);
		}

		for (int32_t i = 0; i < itemCount; i++)
		{
			uint32_t index = start + i;
			if (index >= mInfoItems->count())
			{
				continue;
			}
			char* infoItem = (char*)mInfoItems->get(index);
			component::textBox(infoItem, mSelectedControl == index, false, horizAlignmentCenter, 193, yPos, 322, 30);
			yPos += 40;
		}
	}
	else
	{
		int yPos = ((context::getBufferHeight() - 44) / 2);
		yPos += theme::getCenterOffset();

		component::textBox("No items", false, false, horizAlignmentCenter, 193, 225, 322, 44);
	}
	drawing::drawBitmapStringAligned(context::getBitmapFontSmall(), "\xC2\xA2 Back", theme::getFooterTextColor(), horizAlignmentRight, 40, theme::getFooterY(), 640);
}
