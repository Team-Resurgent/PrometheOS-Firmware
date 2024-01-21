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
#include "autoBootDelayScene.h"
#include "ledColorFlowScene.h"
#include "boredScene.h"

#include "..\settingsManager.h"
#include "..\stringUtility.h"

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
	}
	else if (sceneItem == sceneItemUtilitiesScene)
	{
		pointerVector* sceneItems = new pointerVector(true);
		sceneItems->add(new utils::intContainer(sceneItemHddUnlockScene));
		sceneItems->add(new utils::intContainer(sceneItemEepromToolsScene));
		setScene(new menuScene("Select Utility option...", "", sceneItemSystemSettingsScene, sceneItems));
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
	}
	else if (sceneItem == sceneItemEepromToolsScene)
	{
		pointerVector* sceneItems = new pointerVector(true);
		sceneItems->add(new utils::intContainer(sceneItemBackupEepromScene));
		sceneItems->add(new utils::intContainer(sceneItemRestoreEepromScene));
		setScene(new menuScene("Select EEPROM option...", "", sceneItemUtilitiesScene, sceneItems));
	}
	else if (sceneItem == sceneItemFlashFlowScene)
	{
		setScene(new flashFlowScene());
	}
	else if (sceneItem == sceneItemRemoveScene)
	{
		setScene(new removeScene());
	}
	else if (sceneItem == sceneItemEditFlowScene)
	{
		setScene(new editFlowScene());
	}
	else if (sceneItem == sceneItemMainScene)
	{
		setScene(new mainScene());
	}
	else if (sceneItem == sceneItemSystemInfoScene)
	{
		pointerVector* sceneItems = new pointerVector(true);
		sceneItems->add(new utils::intContainer(sceneItemSystemInfoSceneConsole));
		sceneItems->add(new utils::intContainer(sceneItemSystemInfoSceneAudio));
		sceneItems->add(new utils::intContainer(sceneItemSystemInfoSceneVideo));
		sceneItems->add(new utils::intContainer(sceneItemSystemInfoSceneAbout));
		setScene(new menuScene("Select System Info option...", "", sceneItemSystemSettingsScene, sceneItems));
	}
	else if (sceneItem == sceneItemSystemInfoSceneConsole)
	{
		setScene(new systemInfoScene(systemInfoCategoryConsole));
	}
	else if (sceneItem == sceneItemSystemInfoSceneAudio)
	{
		setScene(new systemInfoScene(systemInfoCategoryAudio));
	}
	else if (sceneItem == sceneItemSystemInfoSceneVideo)
	{
		setScene(new systemInfoScene(systemInfoCategoryVideo));
	}
	else if (sceneItem == sceneItemSystemInfoSceneAbout)
	{
		setScene(new systemInfoScene(systemInfoCategoryAbout));
	}
	else if (sceneItem == sceneItemVideoSettingsScene)
	{
		setScene(new videoSettingsScene());
	}
	else if (sceneItem == sceneItemAudioSettingsScene)
	{
		setScene(new audioSettingsScene());
	}
	else if (sceneItem == sceneItemRegionSettingsScene)
	{
		setScene(new regionSettingsScene());
	}
	else if (sceneItem == sceneItemNetworkSettingsScene)
	{
		setScene(new networkSettingsScene());
	}
	else if (sceneItem == sceneItemHddUnlockScene)
	{
		setScene(new hddLockUnlockScene());
	}
	else if (sceneItem == sceneItemBackupEepromScene)
	{
		setScene(new backupEepromScene());
	}
	else if (sceneItem == sceneItemRestoreEepromScene)
	{
		setScene(new restoreEepromFlowScene());
	}
	else if (sceneItem == sceneItemPrometheOsSettingsScene)
	{
		pointerVector* sceneItems = new pointerVector(true);
		sceneItems->add(new utils::intContainer(sceneItemSkinSelectionScene));
		sceneItems->add(new utils::intContainer(sceneItemAutoBootDelayScene));
		sceneItems->add(new utils::intContainer(sceneItemLedColorFlowScene));
		sceneItems->add(new utils::intContainer(sceneItemBoredScene));
		setScene(new menuScene("Select PrometheOS option...", "", sceneItemSystemSettingsScene, sceneItems));
	}
	else if (sceneItem == sceneItemSkinSelectionScene)
	{
		setScene(new skinSelectionScene());
	}
	else if (sceneItem == sceneItemAutoBootDelayScene)
	{
		setScene(new autoBootDelayScene());
	}
	else if (sceneItem == sceneItemLedColorFlowScene)
	{
		setScene(new ledColorFlowScene());
	}
	else if (sceneItem == sceneItemBoredScene)
	{
		setScene(new boredScene());
	}
}