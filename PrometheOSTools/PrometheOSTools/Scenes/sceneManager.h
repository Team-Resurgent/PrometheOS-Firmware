#pragma once

#include "scene.h"

#include <xtl.h>

enum sceneItemEnum
{ 
	sceneItemMainScene = 0,
	sceneItemSystemToolsScene = 1,
	sceneItemEepromToolsScene = 2,
	sceneItemBackupEepromScene = 3,
	sceneItemRestoreEepromScene = 4,
	sceneItemHddUnlockScene = 5,
	sceneItemHddPasswordScene = 6,
	sceneItemFlashToolsScene = 7,
	sceneItemFlashUpdateFlowScene = 8,
	sceneItemFlashBackupScene = 9,
	sceneItemFlashUpdateRecoveryFlowScene = 10,
	sceneItemFlashBackupRecoveryScene = 11,
	sceneItemAVSettingsScene = 12,
	sceneItemVideoSettingsScene = 13,
	sceneItemAudioSettingsScene = 14,
	sceneItemRegionSettingsScene = 15,
	sceneItemHdmiSettingsScene = 16,
	sceneItemHdmiVideoSettingsScene = 17,
	sceneItemHdmiAdvancedScene = 18,
	sceneItemHdmiInterpolationScene = 19
}; 

typedef struct sceneContainer 
{
	sceneItemEnum sceneItem;
	scene* scene;

	sceneContainer(sceneItemEnum sceneItem, scene* scene)
	{
		this->sceneItem = sceneItem;
		this->scene = scene; 
	}

	~sceneContainer()
	{
		delete(this->scene);
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
