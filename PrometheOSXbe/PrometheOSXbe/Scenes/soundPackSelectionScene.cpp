#include "soundPackSelectionScene.h"
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

soundPackSelectionScene::soundPackSelectionScene()
{
	mMenuItems = new pointerVector<char*>(false);
	mMenuItems->add(strdup("Random"));
	mMenuItems->add(strdup("None"));

	char* currentSoundPackName = settingsManager::getSoundPackName();
	mSelectedControl = stringUtility::equals(currentSoundPackName, "*", false) == true ? 0 : 1;

	pointerVector<char*>* soundPacks = theme::getSoundPacks();
	if (soundPacks != NULL)
	{
		for (uint32_t i = 0; i < soundPacks->count(); i++)
		{
			char* soundPackName = strdup(soundPacks->get(i));
			mMenuItems->add(soundPackName);
			if (stringUtility::equals(soundPackName, currentSoundPackName, true) == true)
			{
				mSelectedControl = i + 2;
			}
		}
	}
	delete(soundPacks);

	free(currentSoundPackName);
}

soundPackSelectionScene::~soundPackSelectionScene()
{
	delete(mMenuItems);
}

void soundPackSelectionScene::update()
{
	// Exit Action

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
			theme::loadSoundPack("*");
			settingsManager::setSoundPackName("*");
		}
		else if (mSelectedControl == 1)
		{
			theme::loadSoundPack("");
			settingsManager::setSoundPackName("");
		}
		else
		{
			char* soundPackName = mMenuItems->get(mSelectedControl);
			theme::loadSoundPack(soundPackName);
			settingsManager::setSoundPackName(soundPackName);
		}

	}
	
	// Down Actions

	if (inputManager::buttonPressed(ButtonDpadDown))
	{
		mSelectedControl = mSelectedControl < (int)(mMenuItems->count() - 1) ? mSelectedControl + 1 : 0;
	}

	// Up Actions

	if (inputManager::buttonPressed(ButtonDpadUp))
	{
		mSelectedControl = mSelectedControl > 0 ? mSelectedControl - 1 : (int)(mMenuItems->count() - 1); 
	}
}

void soundPackSelectionScene::render()
{
	component::panel(theme::getPanelFillColor(), theme::getPanelStrokeColor(), 16, 16, 688, 448);
	drawing::drawBitmapStringAligned(context::getBitmapFontMedium(), "Select Sound Pack option...", theme::getHeaderTextColor(), theme::getHeaderAlign(), 40, theme::getHeaderY(), 640);

	int32_t maxItems = 7;

	int32_t start = 0;
	if ((int32_t)mMenuItems->count() >= maxItems)
	{
		start = min(max(mSelectedControl - (maxItems / 2), 0), (int32_t)mMenuItems->count() - maxItems);
	}

	int32_t itemCount = min(start + maxItems, (int32_t)mMenuItems->count()) - start; 

	uint32_t yPos = (context::getBufferHeight() - ((itemCount * 40) - 10)) / 2;
	yPos += theme::getCenterOffset();

	for (int32_t i = 0; i < itemCount; i++)
	{
		uint32_t index = start + i;
		char* soundPackName = mMenuItems->get(index);
		component::button(mSelectedControl == index, false, soundPackName, 193, yPos, 322, 30);
		yPos += 40;
	}

	drawing::drawBitmapString(context::getBitmapFontSmall(), "\xC2\xA1 Select", theme::getFooterTextColor(), 40, theme::getFooterY());
	drawing::drawBitmapStringAligned(context::getBitmapFontSmall(), "\xC2\xA2 Back", theme::getFooterTextColor(), horizAlignmentRight, 40, theme::getFooterY(), 640);
}