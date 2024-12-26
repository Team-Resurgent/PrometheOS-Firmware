#include "scenemanager.h"
#include "scene.h"
#include "flashFlowScene.h"
#include "removeScene.h"
#include "editFlowScene.h"
#include "mainScene.h"
#include "systemInfoScene.h"
#include "videoSettingsScene.h"
#include "videoSettingsScene.h"
#include "audioSettingsScene.h"
#include "regionSettingsScene.h"
#include "networkSettingsScene.h"
#include "hddLockUnlockScene.h"
#include "backupEepromScene.h"
#include "restoreEepromFlowScene.h"
#include "rtcScene.h"
#include "menuScene.h"
#include "skinSelectionScene.h"
#include "soundPackSelectionScene.h"
#include "snakeScene.h"
#include "invadersScene.h"
#include "launchScene.h"
#include "autoBootScene.h"
#include "bootOptionsScene.h"
#include "audioVideoOptionsScene.h"
#include "lcdOptionsScene.h"
#include "ledOptionsScene.h"
#include "miscellaneousOptionsScene.h"
#include "cerbiosIniEditorScene.h"
#include "flashUpdateFlowScene.h"
#include "flashBackupScene.h"
#include "hdmiVideoSettingsScene.h"
#include "hdmiAdvancedScene.h"
#include "hdmiInterpolationScene.h"
#include "hddPasswordScene.h"
#include "formatDriveScene.h"
#include "launcherFlowScene.h"
#include "dlcSignerScene.h"

#include "..\xboxConfig.h"
#include "..\context.h"
#include "..\settingsManager.h"
#include "..\stringUtility.h"
#include "..\hdmiDevice.h"
#include "..\pointerVector.h"
#include "..\Threads\lcdRender.h"

#include <xtl.h>

namespace 
{
	pointerVector<sceneContainer*>* mScenes = new pointerVector<sceneContainer*>(true);
}

scene* sceneManager::getScene()
{
	sceneContainer* result = mScenes->get(mScenes->count() - 1);
	return result->scene;
}

void sceneManager::pushScene(sceneItemEnum sceneItem)
{
	if (sceneItem == sceneItemMainScene)
	{
		char *versionSemver = settingsManager::getVersionSting(settingsManager::getVersion());
		char *version = stringUtility::formatString("PrometheOS: V%s", versionSemver);
		sceneContainer* container = new sceneContainer(sceneItem, new mainScene(), version);
		free(version);
		free(versionSemver);
		addScene(container);
	}
	if (sceneItem == sceneItemSystemScene)
	{
		pointerVector<utils::intContainer*>* sceneItems = new pointerVector<utils::intContainer*>(true);
		sceneItems->add(new utils::intContainer(sceneItemSystemInfoScene));
		sceneItems->add(new utils::intContainer(sceneItemSettingsScene));
		sceneItems->add(new utils::intContainer(sceneItemUtilitiesScene));
		sceneItems->add(new utils::intContainer(sceneItemGamesScene));
#ifdef TOOLS
		sceneItems->add(new utils::intContainer(sceneItemLauncherFlowScene));
#endif
		sceneContainer* container = new sceneContainer(sceneItem, new menuScene("Select System option...", "", sceneItems), "System Settings");
		addScene(container);
	}
	else if (sceneItem == sceneItemSettingsScene)
	{
		pointerVector<utils::intContainer*>* sceneItems = new pointerVector<utils::intContainer*>(true);
		sceneItems->add(new utils::intContainer(sceneItemAVSettingsScene));
		sceneItems->add(new utils::intContainer(sceneItemRegionSettingsScene));
		sceneItems->add(new utils::intContainer(sceneItemNetworkSettingsScene));
		sceneItems->add(new utils::intContainer(sceneItemDateTimeScene));
#ifndef TOOLS
		sceneItems->add(new utils::intContainer(sceneItemPrometheOsSettingsScene));
#endif
		sceneContainer* container = new sceneContainer(sceneItem, new menuScene("Select Settings option...", "", sceneItems), "Settings");
		addScene(container);
	}
	else if (sceneItem == sceneItemGamesScene)
	{
		pointerVector<utils::intContainer*>* sceneItems = new pointerVector<utils::intContainer*>(true);
		sceneItems->add(new utils::intContainer(sceneItemSnakeScene));
		sceneItems->add(new utils::intContainer(sceneItemInvadersScene));
		sceneContainer* container = new sceneContainer(sceneItem, new menuScene("Select Game option...", "", sceneItems), "Games");
		addScene(container);
	}
	else if (sceneItem == sceneItemUtilitiesScene)
	{
		pointerVector<utils::intContainer*>* sceneItems = new pointerVector<utils::intContainer*>(true);
#ifndef TOOLS
		sceneItems->add(new utils::intContainer(sceneItemHddUnlockScene));
		sceneItems->add(new utils::intContainer(sceneItemFlashToolsScene));
#endif
		sceneItems->add(new utils::intContainer(sceneItemEepromToolsScene));
		sceneItems->add(new utils::intContainer(sceneItemCerbiosIniEditorScene));
#ifndef TOOLS
		sceneItems->add(new utils::intContainer(sceneItemFormatDriveOptionsScene));
#endif
		sceneItems->add(new utils::intContainer(sceneItemDlcSignerScene));
		sceneContainer* container = new sceneContainer(sceneItem, new menuScene("Select Utility option...", "", sceneItems), "Utilities");
		addScene(container);
	}
	else if (sceneItem == sceneItemFormatDriveOptionsScene)
	{
		pointerVector<utils::intContainer*>* sceneItems = new pointerVector<utils::intContainer*>(true);
		sceneItems->add(new utils::intContainer(sceneItemFormatDrivePrimaryScene));
		if (settingsManager::getDriveSetup() == 3)
		{
			sceneItems->add(new utils::intContainer(sceneItemFormatDriveSecondaryScene));
		}
		sceneContainer* container = new sceneContainer(sceneItem, new menuScene("Select Format Drive option...", "", sceneItems), "Drive Selection");
		addScene(container);
	}
	else if (sceneItem == sceneItemBankManagementScene)
	{
		pointerVector<utils::intContainer*>* sceneItems = new pointerVector<utils::intContainer*>(true);
		sceneItems->add(new utils::intContainer(sceneItemFlashFlowScene));
		sceneItems->add(new utils::intContainer(sceneItemRemoveScene));
		sceneItems->add(new utils::intContainer(sceneItemEditFlowScene));

		char* subTitle = stringUtility::formatString("Free Slots %i", settingsManager::getFreeSlots());
		sceneContainer* container = new sceneContainer(sceneItem, new menuScene("Select Bank option...", subTitle, sceneItems), "Bank Management");
		addScene(container);
		free(subTitle);
	}
	else if (sceneItem == sceneItemEepromToolsScene)
	{
		pointerVector<utils::intContainer*>* sceneItems = new pointerVector<utils::intContainer*>(true);
		sceneItems->add(new utils::intContainer(sceneItemBackupEepromScene));
		sceneItems->add(new utils::intContainer(sceneItemRestoreEepromScene));
		sceneContainer* container = new sceneContainer(sceneItem, new menuScene("Select EEPROM option...", "", sceneItems), "EEPROM Selection");
		addScene(container);
	}
	else if (sceneItem == sceneItemDateTimeScene)
	{
		bool doRtc = settingsManager::getRtcEnable() == true && xboxConfig::getHasRtcExpansion() == true;
		sceneContainer* container = new sceneContainer(sceneItem, new rtcScene(), doRtc ? "RTC Exapansion" : "Date Time");
		addScene(container);
	}
	else if (sceneItem == sceneItemFlashFlowScene)
	{
		sceneContainer* container = new sceneContainer(sceneItem, new flashFlowScene(), "Flash Bank");
		addScene(container);
	}
	else if (sceneItem == sceneItemRemoveScene)
	{
		sceneContainer* container = new sceneContainer(sceneItem, new removeScene(), "Remove Bank");
		addScene(container);
	}
	else if (sceneItem == sceneItemEditFlowScene)
	{
		sceneContainer* container = new sceneContainer(sceneItem, new editFlowScene(), "Edit");
		addScene(container);
	}
	else if (sceneItem == sceneItemSystemInfoScene)
	{
		pointerVector<utils::intContainer*>* sceneItems = new pointerVector<utils::intContainer*>(true);
		sceneItems->add(new utils::intContainer(sceneItemSystemInfoSceneConsole));
		sceneItems->add(new utils::intContainer(sceneItemSystemInfoSceneStorage));
		sceneItems->add(new utils::intContainer(sceneItemSystemInfoSceneAudio));
		sceneItems->add(new utils::intContainer(sceneItemSystemInfoSceneVideo));
		sceneItems->add(new utils::intContainer(sceneItemSystemInfoSceneAbout));
		sceneContainer* container = new sceneContainer(sceneItem, new menuScene("Select System Info option...", "", sceneItems), "System Info");
		addScene(container);
	}
	else if (sceneItem == sceneItemSystemInfoSceneConsole)
	{
		sceneContainer* container = new sceneContainer(sceneItem, new systemInfoScene(systemInfoCategoryConsole), "Console Info");
		addScene(container);
	}
	else if (sceneItem == sceneItemSystemInfoSceneStorage)
	{
		sceneContainer* container = new sceneContainer(sceneItem, new systemInfoScene(systemInfoCategoryStorage), "Storage Info");
		addScene(container);
	}
	else if (sceneItem == sceneItemSystemInfoSceneAudio)
	{
		sceneContainer* container = new sceneContainer(sceneItem, new systemInfoScene(systemInfoCategoryAudio), "Audio Info");
		addScene(container);
	}
	else if (sceneItem == sceneItemSystemInfoSceneVideo)
	{
		sceneContainer* container = new sceneContainer(sceneItem, new systemInfoScene(systemInfoCategoryVideo), "Video Info");
		addScene(container);
	}
	else if (sceneItem == sceneItemSystemInfoSceneAbout)
	{
		sceneContainer* container = new sceneContainer(sceneItem, new systemInfoScene(systemInfoCategoryAbout), "About Info");
		addScene(container);
	}
	else if (sceneItem == sceneItemVideoSettingsScene)
	{
		sceneContainer* container = new sceneContainer(sceneItem, new videoSettingsScene(), "Video Settings");
		addScene(container);
	}
	else if (sceneItem == sceneItemAudioSettingsScene)
	{
		sceneContainer* container = new sceneContainer(sceneItem, new audioSettingsScene(), "Audio Settings");
		addScene(container);
	}
	else if (sceneItem == sceneItemRegionSettingsScene)
	{
		sceneContainer* container = new sceneContainer(sceneItem, new regionSettingsScene(), "Region Settings");
		addScene(container);
	}
	else if (sceneItem == sceneItemNetworkSettingsScene)
	{
		sceneContainer* container = new sceneContainer(sceneItem, new networkSettingsScene(), "Network");
		addScene(container);
	}
	else if (sceneItem == sceneItemHddUnlockScene)
	{
		sceneContainer* container = new sceneContainer(sceneItem, new hddLockUnlockScene(), "HDD Lock / Unlock");
		addScene(container);
	}
	else if (sceneItem == sceneItemBackupEepromScene)
	{
		sceneContainer* container = new sceneContainer(sceneItem, new backupEepromScene(), "Backup EEPROM");
		addScene(container);
	}
	else if (sceneItem == sceneItemRestoreEepromScene)
	{
		sceneContainer* container = new sceneContainer(sceneItem, new restoreEepromFlowScene(), "Restore EEPROM");
		addScene(container);
	}
	else if (sceneItem == sceneItemFlashToolsScene)
	{
		pointerVector<utils::intContainer*>* sceneItems = new pointerVector<utils::intContainer*>(true);
		sceneItems->add(new utils::intContainer(sceneItemFlashUpdateFlowScene));
		sceneItems->add(new utils::intContainer(sceneItemFlashBackupScene));
		if (context::getModchip()->getSupportInfo(true).supportsRecovery == true)
		{
			sceneItems->add(new utils::intContainer(sceneItemFlashUpdateRecoveryFlowScene));
			sceneItems->add(new utils::intContainer(sceneItemFlashBackupRecoveryScene));
		}
		sceneContainer* container = new sceneContainer(sceneItem, new menuScene("Select Flash Tool option...", "", sceneItems), "Flash Tools");
		addScene(container);
	}
	else if (sceneItem == sceneItemPrometheOsSettingsScene)
	{
		pointerVector<utils::intContainer*>* sceneItems = new pointerVector<utils::intContainer*>(true);
		sceneItems->add(new utils::intContainer(sceneItemPrometheOsThemesScene));
		sceneItems->add(new utils::intContainer(sceneItemGeneralOptionsScene));
		sceneContainer* container = new sceneContainer(sceneItem, new menuScene("Select PrometheOS option...", "", sceneItems), "PrometheOS");
		addScene(container);
	}
	else if (sceneItem == sceneItemSkinSelectionScene)
	{
		sceneContainer* container = new sceneContainer(sceneItem, new skinSelectionScene(), "Skins");
		addScene(container);
	}
	else if (sceneItem == sceneItemSoundPackSelectionScene)
	{
		sceneContainer* container = new sceneContainer(sceneItem, new soundPackSelectionScene(), "Sound Packs");
		addScene(container);
	}
	else if (sceneItem == sceneItemSnakeScene)
	{
		sceneContainer* container = new sceneContainer(sceneItem, new snakeScene(), "Snake");
		addScene(container);
	}
	else if (sceneItem == sceneItemInvadersScene)
	{
		sceneContainer* container = new sceneContainer(sceneItem, new invadersScene(), "Invaders");
		addScene(container);
	}
	else if (sceneItem == sceneItemLaunchScene)
	{
		sceneContainer* container = new sceneContainer(sceneItem, new launchScene(), "Launch Bank");
		addScene(container);
	}
	else if (sceneItem == sceneItemAutoBootScene)
	{
		sceneContainer* container = new sceneContainer(sceneItem, new autoBootScene(), "Auto Boot");
		addScene(container);
	}
	else if (sceneItem == sceneItemGeneralOptionsScene)
	{
		pointerVector<utils::intContainer*>* sceneItems = new pointerVector<utils::intContainer*>(true);
		sceneItems->add(new utils::intContainer(sceneItemBootOptionsScene));
		sceneItems->add(new utils::intContainer(sceneItemAudioVideoOptionsScene));
		if (context::getModchip()->getSupportInfo(true).supportsLcd == true)
		{
			sceneItems->add(new utils::intContainer(sceneItemLcdOptionsScene));
		}
		if (context::getModchipType() == modchipTypeModxo)
		{
			sceneItems->add(new utils::intContainer(sceneItemLedOptionsScene));
		}
		sceneItems->add(new utils::intContainer(sceneItemMiscellaneousOptionsScene));
		sceneContainer* container = new sceneContainer(sceneItem, new menuScene("General Options...", "", sceneItems), "General Options");
		addScene(container);
	}
	else if (sceneItem == sceneItemBootOptionsScene)
	{
		sceneContainer* container = new sceneContainer(sceneItem, new bootOptionsScene(), "Boot Options");
		addScene(container);
	}
	else if (sceneItem == sceneItemAudioVideoOptionsScene)
	{
		sceneContainer* container = new sceneContainer(sceneItem, new audioVideoOptionsScene(), "Audio/Video Options");
		addScene(container);
	}
	else if (sceneItem == sceneItemLcdOptionsScene)
	{
		sceneContainer* container = new sceneContainer(sceneItem, new lcdOptionsScene(), "LCD Options");
		addScene(container);
	}
	else if (sceneItem == sceneItemLedOptionsScene)
	{
		sceneContainer* container = new sceneContainer(sceneItem, new ledOptionsScene(), "LED Options");
		addScene(container);
	}
	else if (sceneItem == sceneItemMiscellaneousOptionsScene)
	{
		sceneContainer* container = new sceneContainer(sceneItem, new miscellaneousOptionsScene(), "Miscellaneous Options");
		addScene(container);
	}
	else if (sceneItem == sceneItemCerbiosIniEditorScene)
	{
		sceneContainer* container = new sceneContainer(sceneItem, new cerbiosIniEditorScene(), "Cerbios INI Editor");
		addScene(container);
	}
	else if (sceneItem == sceneItemFormatDrivePrimaryScene)
	{
		sceneContainer* container = new sceneContainer(sceneItem, new formatDriveScene(0), "Format Primarary Drive");
		addScene(container);
	}
	else if (sceneItem == sceneItemFormatDriveSecondaryScene)
	{
		sceneContainer* container = new sceneContainer(sceneItem, new formatDriveScene(1), "Format Secondary Drive");
		addScene(container);
	}
	else if (sceneItem == sceneItemFlashUpdateFlowScene)
	{
		sceneContainer* container = new sceneContainer(sceneItem, new flashUpdateFlowScene(false), "Update PrometheOS");
		addScene(container);
	}
	else if (sceneItem == sceneItemFlashBackupScene)
	{
		sceneContainer* container = new sceneContainer(sceneItem, new flashBackupScene(false), "Backup PrometheOS");
		addScene(container);
	}
	else if (sceneItem == sceneItemFlashUpdateRecoveryFlowScene)
	{
		sceneContainer* container = new sceneContainer(sceneItem, new flashUpdateFlowScene(true), "Update Recovery");
		addScene(container);
	}
	else if (sceneItem == sceneItemFlashBackupRecoveryScene)
	{
		sceneContainer* container = new sceneContainer(sceneItem, new flashBackupScene(true), "Backup Recovery");
		addScene(container);
	}
	else if (sceneItem == sceneItemPrometheOsThemesScene)
	{
		pointerVector<utils::intContainer*>* sceneItems = new pointerVector<utils::intContainer*>(true);
		sceneItems->add(new utils::intContainer(sceneItemSkinSelectionScene));
		sceneItems->add(new utils::intContainer(sceneItemSoundPackSelectionScene));
		sceneContainer* container = new sceneContainer(sceneItem, new menuScene("Select PrometheOS Theme option...", "", sceneItems), "PrometheOS Themes");
		addScene(container);
	}

	else if (sceneItem == sceneItemAVSettingsScene)
	{
		pointerVector<utils::intContainer*>* sceneItems = new pointerVector<utils::intContainer*>(true);
		sceneItems->add(new utils::intContainer(sceneItemVideoSettingsScene));
		sceneItems->add(new utils::intContainer(sceneItemAudioSettingsScene));
		if (hdmiDevice::detectDevice() == true)
		{
			sceneItems->add(new utils::intContainer(sceneItemHdmiSettingsScene));
		}
		sceneContainer* container = new sceneContainer(sceneItem, new menuScene("Select AV Settings option...", "", sceneItems), "AV Settings");
		addScene(container);
	}
	else if (sceneItem == sceneItemHdmiSettingsScene)
	{
		pointerVector<utils::intContainer*>* sceneItems = new pointerVector<utils::intContainer*>(true);
		sceneItems->add(new utils::intContainer(sceneItemHdmiVideoSettingsScene));
		sceneItems->add(new utils::intContainer(sceneItemHdmiAdvancedScene));
		sceneItems->add(new utils::intContainer(sceneItemHdmiInterpolationScene));
		sceneContainer* container = new sceneContainer(sceneItem, new menuScene("Select Cerbios HDMI Settings option...", "", sceneItems), "Cerbios HDMI Settings");
		addScene(container);
	}
	else if (sceneItem == sceneItemHdmiVideoSettingsScene)
	{
		sceneContainer* container = new sceneContainer(sceneItem, new hdmiVideoSettingsScene(), "HDMI Video Settings");
		addScene(container);
	}
	else if (sceneItem == sceneItemHdmiAdvancedScene)
	{
		sceneContainer* container = new sceneContainer(sceneItem, new hdmiAdvancedScene(), "HDMI Advanced");
		addScene(container);
	}
	else if (sceneItem == sceneItemHdmiInterpolationScene)
	{
		sceneContainer* container = new sceneContainer(sceneItem, new hdmiInterpolationScene(), "HDMI Interpolation");
		addScene(container);
	}
	else if (sceneItem == sceneItemHddPasswordScene)
	{
		sceneContainer* container = new sceneContainer(sceneItem, new hddPasswordScene(), "HDD Password VSC");
		addScene(container);
	}
	else if (sceneItem == sceneItemLauncherFlowScene)
	{
		sceneContainer* container = new sceneContainer(sceneItem, new launcherFlowScene(), "Launch XBE");
		addScene(container);
	}
	else if (sceneItem == sceneItemDlcSignerScene)
	{
		sceneContainer* container = new sceneContainer(sceneItem, new dlcSignerScene(), "DLC / Update Signer");
		addScene(container);
	}
}

void sceneManager::pushScene(sceneContainer* container)
{
	addScene(container);
}

void sceneManager::popScene(sceneResult result)
{
	sceneContainer* popContainer = mScenes->get(mScenes->count() - 1);
	if (popContainer->onSceneClosingCallback != NULL)
	{
		popContainer->onSceneClosingCallback(result, popContainer->context, popContainer->scene);
	}

	mScenes->remove(mScenes->count() - 1);

	sceneContainer* container = mScenes->get(mScenes->count() - 1);
	if (container->sceneItem == sceneItemBankManagementScene)
	{
		menuScene* menu = (menuScene*)container->scene;
		char* subTitle = stringUtility::formatString("Free Slots %i", settingsManager::getFreeSlots());
		menu->setSubTilte(subTitle);
		free(subTitle);
	}
	else if (container->sceneItem == sceneItemDateTimeScene)
	{
		bool doRtc = settingsManager::getRtcEnable() == true && xboxConfig::getHasRtcExpansion() == true;
		free(container->description);
		container->description = strdup(strdup(doRtc ? "RTC Exapansion" : "Date Time"));
	}

	context::setCurrentTitle(container->description);
}

// Private

void sceneManager::addScene(sceneContainer* sceneContainer)
{
	mScenes->add(sceneContainer);
	context::setCurrentTitle(sceneContainer->description);
}