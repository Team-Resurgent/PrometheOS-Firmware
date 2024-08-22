#include "audioVideoOptionsScene.h"
#include "sceneManager.h"

#include "..\context.h"
#include "..\drawing.h"
#include "..\component.h"
#include "..\ssfn.h"
#include "..\inputManager.h"
#include "..\settingsManager.h"
#include "..\hdmiDevice.h"
#include "..\stringUtility.h"
#include "..\xboxConfig.h"
#include "..\theme.h"
#include "..\fileSystem.h"

audioVideoOptionsScene::audioVideoOptionsScene()
{
	mSelectedControl = 0;
}

void audioVideoOptionsScene::update()
{
	// Exit Action

	if (inputManager::buttonPressed(ButtonB))
	{
		sceneManager::popScene();
		return;
	}

	// Down Actions

	if (inputManager::buttonPressed(ButtonDpadDown))
	{
		mSelectedControl = mSelectedControl < 2? mSelectedControl + 1 : 0;
	}

	// Up Actions

	if (inputManager::buttonPressed(ButtonDpadUp))
	{
		mSelectedControl = mSelectedControl > 0 ? mSelectedControl - 1 : 2; 
	}

	// Left Trigger

	if (inputManager::buttonPressed(ButtonTriggerLeft) || inputManager::buttonPressed(ButtonX))
	{
		if (mSelectedControl == 0)
		{
			uint8_t musicVolume = settingsManager::getMusicVolume();
			musicVolume = (musicVolume >= 5) ? (musicVolume - 5) : 100;
			settingsManager::setMusicVolume(musicVolume);
			return;
		}

		if (mSelectedControl == 1)
		{
			uint8_t soundVolume = settingsManager::getSoundVolume();
			soundVolume = (soundVolume >= 5) ? (soundVolume - 5) : 100;
			settingsManager::setSoundVolume(soundVolume);
			return;
		}

		if (mSelectedControl == 2)
		{
			bool vgaEnable = settingsManager::getVgaEnable();
			settingsManager::setVgaEnable(!vgaEnable);
			return;
		}
	}

	// Right Trigger

	if (inputManager::buttonPressed(ButtonTriggerRight) || inputManager::buttonPressed(ButtonY))
	{
		if (mSelectedControl == 0)
		{
			uint8_t musicVolume = settingsManager::getMusicVolume();
			musicVolume = (musicVolume >= 100) ? 0 : (musicVolume + 5);
			settingsManager::setMusicVolume(musicVolume);
			return;
		}

		if (mSelectedControl == 1)
		{
			uint8_t soundVolume = settingsManager::getSoundVolume();
			soundVolume = (soundVolume >= 100) ? 0 : (soundVolume + 5);
			settingsManager::setSoundVolume(soundVolume);
			return;
		}

		if (mSelectedControl == 2)
		{
			bool vgaEnable = settingsManager::getVgaEnable();
			settingsManager::setVgaEnable(!vgaEnable);
			return;
		}
	}
}

void audioVideoOptionsScene::render()
{
	component::panel(theme::getPanelFillColor(), theme::getPanelStrokeColor(), 16, 16, 688, 448);
	drawing::drawBitmapStringAligned(context::getBitmapFontMedium(), "Audio/Video Options...", theme::getHeaderTextColor(), theme::getHeaderAlign(), 40, theme::getHeaderY(), 640);

	char* musicVolume = stringUtility::formatString("Music Volume: %i%%", settingsManager::getMusicVolume());
	char* soundVolume = stringUtility::formatString("Sound Volume: %i%%", settingsManager::getSoundVolume());
	char* vgaEnable = stringUtility::formatString("VGA Enabled: %s", settingsManager::getVgaEnable() == true ? "Yes" : "No");

	pointerVector<char*>* menuItems = new pointerVector<char*>(false);
	menuItems->add(strdup(musicVolume));
	menuItems->add(strdup(soundVolume));
	menuItems->add(strdup(vgaEnable));

	int32_t maxItems = 7;

	int32_t start = 0;
	if ((int32_t)menuItems->count() >= maxItems)
	{
		start = min(max(mSelectedControl - (maxItems / 2), 0), (int32_t)menuItems->count() - maxItems);
	}

	int32_t itemCount = min(start + maxItems, (int32_t)menuItems->count()) - start; 
	if (itemCount > 0)
	{
		uint32_t yPos = (context::getBufferHeight() - ((itemCount * 40) - 10)) / 2;
		yPos += theme::getCenterOffset();

		for (int32_t i = 0; i < itemCount; i++)
		{
			uint32_t index = start + i;
			char* menuText = menuItems->get(index);
			component::button(mSelectedControl == index, false, menuText, 193, yPos, 322, 30);
			yPos += 40;
		}
	}

	free(vgaEnable);
	free(soundVolume);
	free(musicVolume);

	delete(menuItems);

	drawing::drawBitmapString(context::getBitmapFontSmall(), "\xC2\xA3\xC2\xA4 or \xC2\xB2\xC2\xB3 Change Value", theme::getFooterTextColor(), 40, theme::getFooterY());
	drawing::drawBitmapStringAligned(context::getBitmapFontSmall(), "\xC2\xA2 Back", theme::getFooterTextColor(), horizAlignmentRight, 40, theme::getFooterY(), 640);
}