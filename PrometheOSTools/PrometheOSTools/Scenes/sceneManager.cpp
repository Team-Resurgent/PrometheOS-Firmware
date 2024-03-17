#include "scenemanager.h"
#include "scene.h"
#include "mainScene.h"
#include "videoSettingsScene.h"
#include "videoSettingsScene.h"
#include "audioSettingsScene.h"
#include "regionSettingsScene.h"
#include "hddLockUnlockScene.h"
#include "backupEepromScene.h"
#include "restoreEepromFlowScene.h"
#include "menuScene.h"
#include "flashUpdateFlowScene.h"
#include "flashBackupScene.h"
#include "hdmiVideoSettingsScene.h"
#include "hdmiAdvancedScene.h"
#include "hdmiInterpolationScene.h"
#include "hddPasswordScene.h"

#include "..\context.h"
#include "..\stringUtility.h"
#include "..\hdmiDevice.h"
#include "..\pointerVector.h"

#include <xtl.h>

namespace 
{
	pointerVector* mScenes = new pointerVector(true);
}

scene* sceneManager::getScene()
{
	sceneContainer* result = (sceneContainer*)mScenes->get(mScenes->count() - 1);
	return result->scene;
}

	//sceneItemMainScene = 0,
	//sceneItemSystemSettingsScene = 1,
	//


	//sceneItemFlashUpdateFlowScene = 7,
	//sceneItemFlashBackupScene = 8,



void sceneManager::pushScene(sceneItemEnum sceneItem)
{
	if (sceneItem == sceneItemMainScene)
	{
		sceneContainer* container = new sceneContainer(sceneItem, new mainScene());
		addScene(container);
	}
	else if (sceneItem == sceneItemSystemToolsScene)
	{
		pointerVector* sceneItems = new pointerVector(true);
		sceneItems->add(new utils::intContainer(sceneItemEepromToolsScene));
		//sceneItems->add(new utils::intContainer(sceneItemHddUnlockScene));
		sceneItems->add(new utils::intContainer(sceneItemAVSettingsScene));
		sceneItems->add(new utils::intContainer(sceneItemRegionSettingsScene));

		sceneContainer* container = new sceneContainer(sceneItem, new menuScene("Select System Tool option...", "", sceneItems));
		addScene(container);
	}
	else if (sceneItem == sceneItemEepromToolsScene)
	{
		pointerVector* sceneItems = new pointerVector(true);
		sceneItems->add(new utils::intContainer(sceneItemBackupEepromScene));
		sceneItems->add(new utils::intContainer(sceneItemRestoreEepromScene));
		sceneContainer* container = new sceneContainer(sceneItem, new menuScene("Select EEPROM option...", "", sceneItems));
		addScene(container);
	}
	else if (sceneItem == sceneItemVideoSettingsScene)
	{
		sceneContainer* container = new sceneContainer(sceneItem, new videoSettingsScene());
		addScene(container);
	}
	else if (sceneItem == sceneItemAudioSettingsScene)
	{
		sceneContainer* container = new sceneContainer(sceneItem, new audioSettingsScene());
		addScene(container);
	}
	else if (sceneItem == sceneItemRegionSettingsScene)
	{
		sceneContainer* container = new sceneContainer(sceneItem, new regionSettingsScene());
		addScene(container);
	}
	else if (sceneItem == sceneItemHddUnlockScene)
	{
		sceneContainer* container = new sceneContainer(sceneItem, new hddLockUnlockScene());
		addScene(container);
	}
	else if (sceneItem == sceneItemBackupEepromScene)
	{
		sceneContainer* container = new sceneContainer(sceneItem, new backupEepromScene());
		addScene(container);
	}
	else if (sceneItem == sceneItemRestoreEepromScene)
	{
		sceneContainer* container = new sceneContainer(sceneItem, new restoreEepromFlowScene());
		addScene(container);
	}
	else if (sceneItem == sceneItemFlashToolsScene)
	{
		pointerVector* sceneItems = new pointerVector(true);
		sceneItems->add(new utils::intContainer(sceneItemFlashUpdateFlowScene));
		sceneItems->add(new utils::intContainer(sceneItemFlashBackupScene));
		if (context::getModchip()->supportsRecovery() == true)
		{
			sceneItems->add(new utils::intContainer(sceneItemFlashUpdateRecoveryFlowScene));
			sceneItems->add(new utils::intContainer(sceneItemFlashBackupRecoveryScene));
		}
		sceneContainer* container = new sceneContainer(sceneItem, new menuScene("Select Flash Tool option...", "", sceneItems));
		addScene(container);
	}
	else if (sceneItem == sceneItemFlashUpdateFlowScene)
	{
		sceneContainer* container = new sceneContainer(sceneItem, new flashUpdateFlowScene(false));
		addScene(container);
	}
	else if (sceneItem == sceneItemFlashBackupScene)
	{
		sceneContainer* container = new sceneContainer(sceneItem, new flashBackupScene(false));
		addScene(container);
	}
	else if (sceneItem == sceneItemFlashUpdateRecoveryFlowScene)
	{
		sceneContainer* container = new sceneContainer(sceneItem, new flashUpdateFlowScene(true));
		addScene(container);
	}
	else if (sceneItem == sceneItemFlashBackupRecoveryScene)
	{
		sceneContainer* container = new sceneContainer(sceneItem, new flashBackupScene(true));
		addScene(container);
	}
	else if (sceneItem == sceneItemAVSettingsScene)
	{
		pointerVector* sceneItems = new pointerVector(true);
		sceneItems->add(new utils::intContainer(sceneItemVideoSettingsScene));
		sceneItems->add(new utils::intContainer(sceneItemAudioSettingsScene));
		if (hdmiDevice::detectDevice() == true)
		{
			sceneItems->add(new utils::intContainer(sceneItemHdmiSettingsScene));
		}
		sceneContainer* container = new sceneContainer(sceneItem, new menuScene("Select AV Settings option...", "", sceneItems));
		addScene(container);
	}
	else if (sceneItem == sceneItemHdmiSettingsScene)
	{
		pointerVector* sceneItems = new pointerVector(true);
		sceneItems->add(new utils::intContainer(sceneItemHdmiVideoSettingsScene));
		sceneItems->add(new utils::intContainer(sceneItemHdmiAdvancedScene));
		sceneItems->add(new utils::intContainer(sceneItemHdmiInterpolationScene));
		sceneContainer* container = new sceneContainer(sceneItem, new menuScene("Select Cerbios HDMI Settings option...", "", sceneItems));
		addScene(container);
	}
	else if (sceneItem == sceneItemHdmiVideoSettingsScene)
	{
		sceneContainer* container = new sceneContainer(sceneItem, new hdmiVideoSettingsScene());
		addScene(container);
	}
	else if (sceneItem == sceneItemHdmiAdvancedScene)
	{
		sceneContainer* container = new sceneContainer(sceneItem, new hdmiAdvancedScene());
		addScene(container);
	}
	else if (sceneItem == sceneItemHdmiInterpolationScene)
	{
		sceneContainer* container = new sceneContainer(sceneItem, new hdmiInterpolationScene());
		addScene(container);
	}
	else if (sceneItem == sceneItemHddPasswordScene)
	{
		sceneContainer* container = new sceneContainer(sceneItem, new hddPasswordScene());
		addScene(container);
	}
}

void sceneManager::popScene()
{
	mScenes->remove(mScenes->count() - 1);
	sceneContainer* container = (sceneContainer*)mScenes->get(mScenes->count() - 1);
}

// Private

void sceneManager::addScene(sceneContainer* sceneContainer)
{
	mScenes->add(sceneContainer);
}