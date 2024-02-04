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
	sceneItemAutoBootDelayScene = 23,
	sceneItemLedColorFlowScene = 24,
	sceneItemBoredScene = 25,
	sceneItemLaunchScene = 26,
	sceneItemAutoBootScene = 27,
}; 

class sceneManager
{
public:
	static void setScene(scene* scene);
	static scene* getScene();
	static void openScene(sceneItemEnum sceneItem);
};
