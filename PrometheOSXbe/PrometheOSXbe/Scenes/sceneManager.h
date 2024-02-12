#pragma once

#include "scene.h"

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
	sceneItemPrometheOsThemesScene = 29,
}; 

class sceneManager
{
public:
	static void setScene(scene* scene);
	static scene* getScene();
	static void openScene(sceneItemEnum sceneItem);
};
