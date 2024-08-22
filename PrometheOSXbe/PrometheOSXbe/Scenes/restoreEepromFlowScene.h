#pragma once

#include "scene.h"
#include "filePickerScene.h"
#include "restoreEepromScene.h"

#include "..\xboxInternals.h"

class restoreEepromFlowScene : public scene
{
public:
	restoreEepromFlowScene();
	~restoreEepromFlowScene();
	void update();
	void render();
private:
	uint32_t mCurrentSceneId;
	char* mFilePath;
	static void onFilePickerClosingCallback(sceneResult result, void* context, scene* scene);
	static void onRestoreEepromClosingCallback(sceneResult result, void* context, scene* scene);
};
