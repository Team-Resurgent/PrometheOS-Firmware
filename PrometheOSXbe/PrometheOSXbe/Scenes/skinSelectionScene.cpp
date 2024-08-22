#include "skinSelectionScene.h"
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

skinSelectionScene::skinSelectionScene()
{
	mMenuItems = new pointerVector<char*>(false);
	mMenuItems->add(strdup("Random"));
	mMenuItems->add(strdup("Default"));

	char* currentSkinName = settingsManager::getSkinName();
	mSelectedControl = stringUtility::equals(currentSkinName, "*", false) == true ? 0 : 1;

	pointerVector<char*>* skins = theme::getSkins();
	if (skins != NULL)
	{
		for (uint32_t i = 0; i < skins->count(); i++)
		{
			char* skinName = strdup(skins->get(i));
			mMenuItems->add(skinName);
			if (stringUtility::equals(skinName, currentSkinName, true) == true)
			{
				mSelectedControl = i + 2;
			}
		}
	}
	delete(skins);

	free(currentSkinName);
}

skinSelectionScene::~skinSelectionScene()
{
	delete(mMenuItems);
}

void skinSelectionScene::update()
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
			theme::loadSkin("*");
			settingsManager::setSkinName("*");
		}
		else if (mSelectedControl == 1)
		{
			theme::loadSkin("");
			settingsManager::setSkinName("");
		}
		else
		{
			char* skinName = mMenuItems->get(mSelectedControl);
			theme::loadSkin(skinName);
			settingsManager::setSkinName(skinName);
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

void skinSelectionScene::render()
{
	component::panel(theme::getPanelFillColor(), theme::getPanelStrokeColor(), 16, 16, 688, 448);
	drawing::drawBitmapStringAligned(context::getBitmapFontMedium(), "Select Skin option...", theme::getHeaderTextColor(), theme::getHeaderAlign(), 40, theme::getHeaderY(), 640);

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
		char* skinName = mMenuItems->get(index);
		component::button(mSelectedControl == index, false, skinName, 193, yPos, 322, 30);
		yPos += 40;
	}

	drawing::drawBitmapString(context::getBitmapFontSmall(), "\xC2\xA1 Select", theme::getFooterTextColor(), 40, theme::getFooterY());
	drawing::drawBitmapStringAligned(context::getBitmapFontSmall(), "\xC2\xA2 Back", theme::getFooterTextColor(), horizAlignmentRight, 40, theme::getFooterY(), 640);
}