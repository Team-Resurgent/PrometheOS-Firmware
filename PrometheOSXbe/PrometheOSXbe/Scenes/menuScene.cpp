#include "menuScene.h"
#include "sceneManager.h"

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
#include "..\audioPlayer.h"

menuScene::menuScene(const char* title, const char* subTitle, sceneItemEnum backScene, pointerVector* sceneItems)
{
	mSelectedControl = 0;
	mTitle = strdup(title);
	mSubTitle = strdup(subTitle);
	mBackScene = backScene;
	mSceneItems = sceneItems;
}

menuScene::~menuScene()
{
	free(mTitle);
	delete(mSceneItems);
}

void menuScene::update()
{
	// Exit Action

	if (inputManager::buttonPressed(ButtonB))
	{
		sceneManager::openScene(mBackScene);
	}

	// Select Actions

	if (inputManager::buttonPressed(ButtonA))
	{
		utils::intContainer* intContainer = (utils::intContainer*)mSceneItems->get(mSelectedControl);
		sceneItemEnum sceneItem = (sceneItemEnum)intContainer->value;
		sceneManager::openScene(sceneItem);
	}
	
	// Down Actions

	if (inputManager::buttonPressed(ButtonDpadDown))
	{
		mSelectedControl = mSelectedControl < (int)(mSceneItems->count() - 1) ? mSelectedControl + 1 : 0;
	}

	// Up Actions

	if (inputManager::buttonPressed(ButtonDpadUp))
	{
		mSelectedControl = mSelectedControl > 0 ? mSelectedControl - 1 : (int)(mSceneItems->count() - 1); 
	}
}

void menuScene::render()
{
	component::panel(theme::getPanelFillColor(), theme::getPanelStrokeColor(), 16, 16, 688, 448);
	drawing::drawBitmapStringAligned(context::getBitmapFontMedium(), mTitle, theme::getHeaderTextColor(), theme::getHeaderAlign(), 40, theme::getHeaderY(), 640);

	int32_t maxItems = 7;

	int32_t start = 0;
	if ((int32_t)mSceneItems->count() >= maxItems)
	{
		start = min(max(mSelectedControl - (maxItems / 2), 0), (int32_t)mSceneItems->count() - maxItems);
	}

	int32_t itemCount = min(start + maxItems, (int32_t)mSceneItems->count()) - start; 
	if (itemCount > 0)
	{
		uint32_t yPos = (context::getBufferHeight() - ((itemCount * 40) - 10)) / 2;
		yPos += theme::getCenterOffset();

		for (int32_t i = 0; i < itemCount; i++)
		{
			uint32_t index = start + i;
			utils::intContainer* intContainer = (utils::intContainer*)mSceneItems->get(index);
			sceneItemEnum sceneItem = (sceneItemEnum)intContainer->value;
			char* menuText = NULL;
			if (sceneItem == sceneItemSystemSettingsScene)
			{
				menuText = strdup("System Settings");
			}
			else if (sceneItem == sceneItemUtilitiesScene)
			{
				menuText = strdup("Utilities");
			}
			else if (sceneItem == sceneItemEepromToolsScene)
			{
				menuText = strdup("EEPROM Tools");
			}
			else if (sceneItem == sceneItemFlashFlowScene)
			{
				menuText = strdup("Flash Bank");
			}
			else if (sceneItem == sceneItemRemoveScene)
			{
				menuText = strdup("Remove Bank");
			}
			else if (sceneItem == sceneItemEditFlowScene)
			{
				menuText = strdup("Edit Bank");
			}
			else if (sceneItem == sceneItemMainScene)
			{
				menuText = strdup("Main");
			}
			else if (sceneItem == sceneItemSystemInfoScene)
			{
				menuText = strdup("System Info");
			}
			else if (sceneItem == sceneItemSystemInfoSceneConsole)
			{
				menuText = strdup("Console");
			}
			else if (sceneItem == sceneItemSystemInfoSceneAudio)
			{
				menuText = strdup("Audio");
			}
			else if (sceneItem == sceneItemSystemInfoSceneVideo)
			{
				menuText = strdup("Video");
			}
			else if (sceneItem == sceneItemSystemInfoSceneAbout)
			{
				menuText = strdup("About");
			}
			else if (sceneItem == sceneItemVideoSettingsScene)
			{
				menuText = strdup("Video");
			}
			else if (sceneItem == sceneItemAudioSettingsScene)
			{
				menuText = strdup("Audio");
			}
			else if (sceneItem == sceneItemRegionSettingsScene)
			{
				menuText = strdup("Region");
			}
			else if (sceneItem == sceneItemNetworkSettingsScene)
			{
				menuText = strdup("Network");
			}
			else if (sceneItem == sceneItemHddUnlockScene)
			{
				menuText = strdup("HDD Lock / Unlock");
			}
			else if (sceneItem == sceneItemBackupEepromScene)
			{
				menuText = strdup("Backup EEPROM");
			}
			else if (sceneItem == sceneItemRestoreEepromScene)
			{
				menuText = strdup("Restore EEPROM");
			}
			else if (sceneItem == sceneItemPrometheOsSettingsScene)
			{
				menuText = strdup("PrometheOS");
			}
			else if (sceneItem == sceneItemSkinSelectionScene)
			{
				menuText = strdup("Skins");
			}
			else if (sceneItem == sceneItemSoundPackSelectionScene)
			{
				menuText = strdup("Sound Packs");
			}
			else if (sceneItem == sceneItemAutoBootDelayScene)
			{
				menuText = strdup("Auto Boot Delay");
			}
			else if (sceneItem == sceneItemLedColorFlowScene)
			{
				menuText = strdup("Led Color");
			}
			else if (sceneItem == sceneItemBoredScene)
			{
				menuText = strdup("Bored?");
			}
			else
			{
				menuText = strdup("");
			}
			component::button(mSelectedControl == index, false, menuText, 193, yPos, 322, 30);
			free(menuText);
			yPos += 40;
		}
	}
	else
	{
		int yPos = ((context::getBufferHeight() - 44) / 2);
		yPos += theme::getCenterOffset();

		component::textBox("No items", false, false, horizAlignmentCenter, 193, yPos, 322, 44);
	}

	drawing::drawBitmapString(context::getBitmapFontSmall(), "\xC2\xA1 Select", theme::getFooterTextColor(), 40, theme::getFooterY());
	drawing::drawBitmapStringAligned(context::getBitmapFontSmall(), mSubTitle, theme::getFooterTextColor(), horizAlignmentCenter, 40, theme::getFooterY(), 640);
	drawing::drawBitmapStringAligned(context::getBitmapFontSmall(), "\xC2\xA2 Back", theme::getFooterTextColor(), horizAlignmentRight, 40, theme::getFooterY(), 640);
}