#include "menuScene.h"
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
#include "..\audioPlayer.h"

menuScene::menuScene(const char* title, const char* subTitle, pointerVector<utils::intContainer*>* sceneItems)
{
	mSelectedControl = 0;
	mTitle = strdup(title);
	mSubTitle = strdup(subTitle);
	mSceneItems = sceneItems;
}

menuScene::~menuScene()
{
	free(mTitle);
	free(mSubTitle);
	delete(mSceneItems);
}

void menuScene::setSubTilte(const char* subTitle)
{
	free(mSubTitle);
	mSubTitle = strdup(subTitle);
}

void menuScene::update()
{
	// Exit Action

	if (inputManager::buttonPressed(ButtonB))
	{
		sceneManager::popScene();
	}

	// Select Actions

	if (inputManager::buttonPressed(ButtonA))
	{
		utils::intContainer* intContainer = mSceneItems->get(mSelectedControl);
		sceneItemEnum sceneItem = (sceneItemEnum)intContainer->value;
		sceneManager::pushScene(sceneItem);
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
			utils::intContainer* intContainer = mSceneItems->get(index);
			sceneItemEnum sceneItem = (sceneItemEnum)intContainer->value;
			char* menuText = NULL;
			if (sceneItem == sceneItemSystemScene)
			{
				menuText = strdup("System");
			}
			else if (sceneItem == sceneItemFlashToolsScene)
			{
				menuText = strdup("Flash Tools");
			}
			else if (sceneItem == sceneItemSettingsScene)
			{
				menuText = strdup("Settings");
			}
			else if (sceneItem == sceneItemGamesScene)
			{
				menuText = strdup("Games");
			}
			else if (sceneItem == sceneItemUtilitiesScene)
			{
				menuText = strdup("Utilities");
			}
			else if (sceneItem == sceneItemEepromToolsScene)
			{
				menuText = strdup("EEPROM Tools");
			}
			else if (sceneItem == sceneItemDateTimeScene)
			{
				bool doRtc = settingsManager::getRtcEnable() == true && xboxConfig::getHasRtcExpansion() == true;
				menuText = strdup(doRtc ? "RTC Exapansion" : "Date Time");
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
			else if (sceneItem == sceneItemSystemInfoSceneStorage)
			{
				menuText = strdup("Storage");
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
			else if (sceneItem == sceneItemSnakeScene)
			{
				menuText = strdup("Snake");
			}
			else if (sceneItem == sceneItemInvadersScene)
			{
				menuText = strdup("Invaders");
			}
			else if (sceneItem == sceneItemGeneralOptionsScene)
			{
				menuText = strdup("General Options");
			}
			else if (sceneItem == sceneItemBootOptionsScene)
			{
				menuText = strdup("Boot Options");
			}
			else if (sceneItem == sceneItemAudioVideoOptionsScene)
			{
				menuText = strdup("Audio/Video Options");
			}
			else if (sceneItem == sceneItemLcdOptionsScene)
			{
				menuText = strdup("LCD Options");
			}
			else if (sceneItem == sceneItemLedOptionsScene)
			{
				menuText = strdup("LED Options");
			}
			else if (sceneItem == sceneItemMiscellaneousOptionsScene)
			{
				menuText = strdup("Miscellaneous Options");
			}
			else if (sceneItem == sceneItemCerbiosIniEditorScene)
			{
				menuText = strdup("Cerbios INI Editor");
			}
			else if (sceneItem == sceneItemFormatDriveOptionsScene)
			{
				menuText = strdup("Format Drive");
			}
			else if (sceneItem == sceneItemFormatDrivePrimaryScene)
			{
				menuText = strdup("Format Drive Primary");
			}
			else if (sceneItem == sceneItemFormatDriveSecondaryScene)
			{
				menuText = strdup("Format Drive Secondary");
			}
			else if (sceneItem == sceneItemFlashUpdateFlowScene)
			{
				menuText = strdup("Update PrometheOS");
			}
			else if (sceneItem == sceneItemFlashBackupScene)
			{
				menuText = strdup("Backup PrometheOS");
			}
			else if (sceneItem == sceneItemFlashUpdateRecoveryFlowScene)
			{
				menuText = strdup("Update Recovery");
			}
			else if (sceneItem == sceneItemFlashBackupRecoveryScene)
			{
				menuText = strdup("Backup Recovery");
			}
			else if (sceneItem == sceneItemPrometheOsThemesScene)
			{
				menuText = strdup("Themes");
			}
			else if (sceneItem == sceneItemAVSettingsScene)
			{
				menuText = strdup("AV Settings");
			}
			else if (sceneItem == sceneItemHdmiSettingsScene)
			{
				menuText = strdup("Cerbios HDMI Settings");
			}
			else if (sceneItem == sceneItemHdmiVideoSettingsScene)
			{
				menuText = strdup("Video Settings");
			}
			else if (sceneItem == sceneItemHdmiAdvancedScene)
			{
				menuText = strdup("Advanced");
			}
			else if (sceneItem == sceneItemHdmiInterpolationScene)
			{
				menuText = strdup("Interpolation");
			}
			else if (sceneItem == sceneItemLauncherFlowScene)
			{
				menuText = strdup("Launch XBE");
			}
			else if (sceneItem == sceneItemDlcSignerScene)
			{
				menuText = strdup("DLC / Update Signer");
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