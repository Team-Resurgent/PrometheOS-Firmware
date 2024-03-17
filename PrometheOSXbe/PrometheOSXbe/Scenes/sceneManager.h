#pragma once

#include "scene.h"

#include <xtl.h>

enum sceneItemEnum
{ 
	sceneItemMainScene = 0,
	sceneItemSystemSettingsScene = 1,
	sceneItemUtilitiesScene = 2,
	sceneItemBankManagementScene = 3,
	sceneItemEepromToolsScene = 4,
	sceneItemFlashFlowScene = 5,
	sceneItemRemoveScene = 6,
	sceneItemEditFlowScene = 7,
	sceneItemSystemInfoScene = 8,
	sceneItemSystemInfoSceneConsole = 9,
	sceneItemSystemInfoSceneAudio = 10,
	sceneItemSystemInfoSceneVideo = 11,
	sceneItemSystemInfoSceneAbout = 12,
	sceneItemVideoSettingsScene = 13,
	sceneItemAudioSettingsScene = 14,
	sceneItemRegionSettingsScene = 15,
	sceneItemNetworkSettingsScene = 16,
	sceneItemHddUnlockScene = 17,
	sceneItemBackupEepromScene = 18,
	sceneItemRestoreEepromScene = 19,
	sceneItemPrometheOsSettingsScene = 20,
	sceneItemSkinSelectionScene = 21,
	sceneItemSoundPackSelectionScene = 22,
	sceneItemSnakeScene = 23,
	sceneItemLaunchScene = 24,
	sceneItemAutoBootScene = 25,
	sceneItemGeneralOptionsScene = 26,
	sceneItemFlashUpdateFlowScene = 27,
	sceneItemFlashBackupScene = 28,
	sceneItemFlashUpdateRecoveryFlowScene = 29,
	sceneItemFlashBackupRecoveryScene = 30,
	sceneItemPrometheOsThemesScene = 31,
	sceneItemAVSettingsScene = 32,
	sceneItemHdmiSettingsScene = 33,
	sceneItemHdmiVideoSettingsScene = 34,
	sceneItemHdmiAdvancedScene = 35,
	sceneItemHdmiInterpolationScene = 36,
	sceneItemHddPasswordScene = 37
}; 

typedef struct sceneContainer 
{
	sceneItemEnum sceneItem;
	scene* scene;
	char* description;

	sceneContainer(sceneItemEnum sceneItem, scene* scene, const char* description)
	{
		this->sceneItem = sceneItem;
		this->scene = scene; 
		this->description = strdup(description);
	}

	~sceneContainer()
	{
		delete(this->scene);
		free(this->description);
	}

} sceneContainer;

class sceneManager
{
public:
	static scene* getScene();
	static void pushScene(sceneItemEnum sceneItem);
	static void popScene();
private:
	static void addScene(sceneContainer* sceneContainer);
};
