#pragma once

#include "scene.h"

#include <xtl.h>

enum sceneItemEnum
{ 
	sceneItemGenericScene,
	sceneItemMainScene,
	sceneItemSystemScene,
	sceneItemSettingsScene,
	sceneItemUtilitiesScene,
	sceneItemBankManagementScene,
	sceneItemEepromToolsScene,
	sceneItemDateTimeScene,
	sceneItemFlashFlowScene,
	sceneItemRemoveScene,
	sceneItemEditFlowScene,
	sceneItemSystemInfoScene,
	sceneItemSystemInfoSceneConsole,
	sceneItemSystemInfoSceneStorage,
	sceneItemSystemInfoSceneAudio,
	sceneItemSystemInfoSceneVideo,
	sceneItemSystemInfoSceneAbout,
	sceneItemVideoSettingsScene,
	sceneItemAudioSettingsScene,
	sceneItemRegionSettingsScene,
	sceneItemNetworkSettingsScene,
	sceneItemHddUnlockScene,
	sceneItemBackupEepromScene,
	sceneItemRestoreEepromScene,
	sceneItemFlashToolsScene,
	sceneItemPrometheOsSettingsScene,
	sceneItemSkinSelectionScene,
	sceneItemSoundPackSelectionScene,
	sceneItemSnakeScene,
	sceneItemInvadersScene,
	sceneItemLaunchScene,
	sceneItemAutoBootScene,
	sceneItemGeneralOptionsScene,
	sceneItemBootOptionsScene,
	sceneItemAudioVideoOptionsScene,
	sceneItemLcdOptionsScene,
	sceneItemLedOptionsScene,
	sceneItemMiscellaneousOptionsScene,
	sceneItemFlashUpdateFlowScene,
	sceneItemFlashBackupScene,
	sceneItemFlashUpdateRecoveryFlowScene,
	sceneItemFlashBackupRecoveryScene,
	sceneItemPrometheOsThemesScene,
	sceneItemAVSettingsScene,
	sceneItemHdmiSettingsScene,
	sceneItemHdmiVideoSettingsScene,
	sceneItemHdmiAdvancedScene,
	sceneItemHdmiInterpolationScene,
	sceneItemHddPasswordScene,
	sceneItemGamesScene,
	sceneItemCerbiosIniEditorScene,
	sceneItemFormatDriveOptionsScene,
	sceneItemFormatDrivePrimaryScene,
	sceneItemFormatDriveSecondaryScene,
	sceneItemLauncherFlowScene,
	sceneItemDlcSignerScene,
}; 

typedef void (*SceneOnClosingCallback)(sceneResult result, void* context, scene* scene);

typedef struct sceneContainer 
{
	sceneItemEnum sceneItem;
	scene* scene;
	char* description;
	void* context;
	SceneOnClosingCallback onSceneClosingCallback;

	sceneContainer(sceneItemEnum sceneItem, scene* scene, const char* description, void* context = NULL, SceneOnClosingCallback onSceneClosingCallback = NULL)
	{
		this->sceneItem = sceneItem;
		this->scene = scene; 
		this->description = strdup(description);
		this->context = context;
		this->onSceneClosingCallback = onSceneClosingCallback;
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
	static void pushScene(sceneContainer* container);
	static void popScene(sceneResult result = sceneResultNone);
private:
	static void addScene(sceneContainer* sceneContainer);
};
