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
#include "menuScene.h"
#include "skinSelectionScene.h"
#include "soundPackSelectionScene.h"
#include "boredScene.h"
#include "launchScene.h"
#include "autoBootScene.h"
#include "generalOptionsScene.h"
#include "flashUpdateFlowScene.h"
#include "flashBackupScene.h"

#include "..\lcd.h"
#include "..\settingsManager.h"
#include "..\stringUtility.h"
#include "..\Threads\lcdRender.h"

#include <xtl.h>

namespace 
{
	scene* mScene = NULL;
}

void sceneManager::setScene(scene* scene)
{
	if (mScene != NULL)
	{
		delete mScene;
	}
	mScene = scene;
}

scene* sceneManager::getScene()
{
	return mScene;
}

void sceneManager::openScene(sceneItemEnum sceneItem)
{
	if (sceneItem == sceneItemMainScene)
	{
		setScene(new mainScene());

		char *versionSemver = settingsManager::getVersionSting(settingsManager::getVersion());
		char *version = stringUtility::formatString("PrometheOS: V%s", versionSemver);
		lcdRender::setTitle(version);
		free(version);
		free(versionSemver);
	}
	if (sceneItem == sceneItemSystemSettingsScene)
	{
		pointerVector* sceneItems = new pointerVector(true);
		sceneItems->add(new utils::intContainer(sceneItemSystemInfoScene));
		sceneItems->add(new utils::intContainer(sceneItemVideoSettingsScene));
		sceneItems->add(new utils::intContainer(sceneItemAudioSettingsScene));
		sceneItems->add(new utils::intContainer(sceneItemRegionSettingsScene));
		sceneItems->add(new utils::intContainer(sceneItemNetworkSettingsScene));
		sceneItems->add(new utils::intContainer(sceneItemPrometheOsSettingsScene));
		sceneItems->add(new utils::intContainer(sceneItemUtilitiesScene));
		setScene(new menuScene("Select Settings option...", "", sceneItemMainScene, sceneItems));
		lcdRender::setTitle("System Settings");
	}
	else if (sceneItem == sceneItemUtilitiesScene)
	{
		pointerVector* sceneItems = new pointerVector(true);
		sceneItems->add(new utils::intContainer(sceneItemHddUnlockScene));
		sceneItems->add(new utils::intContainer(sceneItemEepromToolsScene));
		setScene(new menuScene("Select Utility option...", "", sceneItemSystemSettingsScene, sceneItems));
		lcdRender::setTitle("Utilities");
	}
	else if (sceneItem == sceneItemBankManagementScene)
	{
		pointerVector* sceneItems = new pointerVector(true);
		sceneItems->add(new utils::intContainer(sceneItemFlashFlowScene));
		sceneItems->add(new utils::intContainer(sceneItemRemoveScene));
		sceneItems->add(new utils::intContainer(sceneItemEditFlowScene));
		char* subTitle = stringUtility::formatString("Free Slots %i", settingsManager::getFreeSlots());
		setScene(new menuScene("Select Bank option...", subTitle, sceneItemMainScene, sceneItems));
		free(subTitle);
		lcdRender::setTitle("Bank Management");
	}
	else if (sceneItem == sceneItemEepromToolsScene)
	{
		pointerVector* sceneItems = new pointerVector(true);
		sceneItems->add(new utils::intContainer(sceneItemBackupEepromScene));
		sceneItems->add(new utils::intContainer(sceneItemRestoreEepromScene));
		setScene(new menuScene("Select EEPROM option...", "", sceneItemUtilitiesScene, sceneItems));
		lcdRender::setTitle("EEPROM Selection");
	}
	else if (sceneItem == sceneItemFlashFlowScene)
	{
		setScene(new flashFlowScene());
		lcdRender::setTitle("Flash Bank");
	}
	else if (sceneItem == sceneItemRemoveScene)
	{
		setScene(new removeScene());
		lcdRender::setTitle("Remove Bank");
	}
	else if (sceneItem == sceneItemEditFlowScene)
	{
		setScene(new editFlowScene());
		lcdRender::setTitle("Edit");
	}
	else if (sceneItem == sceneItemSystemInfoScene)
	{
		pointerVector* sceneItems = new pointerVector(true);
		sceneItems->add(new utils::intContainer(sceneItemSystemInfoSceneConsole));
		sceneItems->add(new utils::intContainer(sceneItemSystemInfoSceneAudio));
		sceneItems->add(new utils::intContainer(sceneItemSystemInfoSceneVideo));
		sceneItems->add(new utils::intContainer(sceneItemSystemInfoSceneAbout));
		setScene(new menuScene("Select System Info option...", "", sceneItemSystemSettingsScene, sceneItems));
		lcdRender::setTitle("System Info");
	}
	else if (sceneItem == sceneItemSystemInfoSceneConsole)
	{
		setScene(new systemInfoScene(systemInfoCategoryConsole));
		lcdRender::setTitle("Console Info");
	}
	else if (sceneItem == sceneItemSystemInfoSceneAudio)
	{
		setScene(new systemInfoScene(systemInfoCategoryAudio));
		lcdRender::setTitle("Audio Info");
	}
	else if (sceneItem == sceneItemSystemInfoSceneVideo)
	{
		setScene(new systemInfoScene(systemInfoCategoryVideo));
		lcdRender::setTitle("Video Info");
	}
	else if (sceneItem == sceneItemSystemInfoSceneAbout)
	{
		setScene(new systemInfoScene(systemInfoCategoryAbout));
		lcdRender::setTitle("About Info");
	}
	else if (sceneItem == sceneItemVideoSettingsScene)
	{
		setScene(new videoSettingsScene());
		lcdRender::setTitle("Video Settings");
	}
	else if (sceneItem == sceneItemAudioSettingsScene)
	{
		setScene(new audioSettingsScene());
		lcdRender::setTitle("Audio Settings");
	}
	else if (sceneItem == sceneItemRegionSettingsScene)
	{
		setScene(new regionSettingsScene());
		lcdRender::setTitle("Region Settings");
	}
	else if (sceneItem == sceneItemNetworkSettingsScene)
	{
		setScene(new networkSettingsScene());
		lcdRender::setTitle("Network");
	}
	else if (sceneItem == sceneItemHddUnlockScene)
	{
		setScene(new hddLockUnlockScene());
		lcdRender::setTitle("HDD Lock / Unlock");
	}
	else if (sceneItem == sceneItemBackupEepromScene)
	{
		setScene(new backupEepromScene());
		lcdRender::setTitle("Backup EEProm");
	}
	else if (sceneItem == sceneItemRestoreEepromScene)
	{
		setScene(new restoreEepromFlowScene());
		lcdRender::setTitle("Restore EEPROM");
	}
	else if (sceneItem == sceneItemPrometheOsSettingsScene)
	{
		pointerVector* sceneItems = new pointerVector(true);
		sceneItems->add(new utils::intContainer(sceneItemPrometheOsThemesScene));
		sceneItems->add(new utils::intContainer(sceneItemGeneralOptionsScene));
		sceneItems->add(new utils::intContainer(sceneItemFlashUpdateFlowScene));
		sceneItems->add(new utils::intContainer(sceneItemFlashBackupScene));
		sceneItems->add(new utils::intContainer(sceneItemSnakeScene));
		setScene(new menuScene("Select PrometheOS option...", "", sceneItemSystemSettingsScene, sceneItems));
		lcdRender::setTitle("PrometheOS");
	}
	else if (sceneItem == sceneItemSkinSelectionScene)
	{
		setScene(new skinSelectionScene());
		lcdRender::setTitle("Skins");
	}
	else if (sceneItem == sceneItemSoundPackSelectionScene)
	{
		setScene(new soundPackSelectionScene());
		lcdRender::setTitle("Sound Packs");
	}
	else if (sceneItem == sceneItemSnakeScene)
	{
		setScene(new boredScene());
		lcdRender::setTitle("Snake");
	}
	else if (sceneItem == sceneItemLaunchScene)
	{
		setScene(new launchScene());
		lcdRender::setTitle("Launch Bank");
	}
	else if (sceneItem == sceneItemAutoBootScene)
	{
		setScene(new autoBootScene());
		lcdRender::setTitle("Auto Boot");
	}
	else if (sceneItem == sceneItemGeneralOptionsScene)
	{
		setScene(new generalOptionsScene());
		lcdRender::setTitle("Options");
	}
	else if (sceneItem == sceneItemFlashUpdateFlowScene)
	{
		setScene(new flashUpdateFlowScene());
		lcdRender::setTitle("Update PrometheOS");
	}
	else if (sceneItem == sceneItemFlashBackupScene)
	{
		setScene(new flashBackupScene());
		lcdRender::setTitle("Backup PrometheOS");
	}
	else if (sceneItem == sceneItemPrometheOsThemesScene)
	{
		pointerVector* sceneItems = new pointerVector(true);
		sceneItems->add(new utils::intContainer(sceneItemSkinSelectionScene));
		sceneItems->add(new utils::intContainer(sceneItemSoundPackSelectionScene));
		setScene(new menuScene("Select PrometheOS Theme option...", "", sceneItemPrometheOsSettingsScene, sceneItems));
		lcdRender::setTitle("PrometheOS");
	}
}