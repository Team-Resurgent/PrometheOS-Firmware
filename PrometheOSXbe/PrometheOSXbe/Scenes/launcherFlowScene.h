#pragma once

#include "scene.h"
#include "filePickerScene.h"

#include "..\xboxInternals.h"

class launcherFlowScene : public scene
{
public:
	launcherFlowScene();
	~launcherFlowScene();
	void update();
	void render();
private:
	uint32_t mCurrentSceneId;
	char* mFilePath;
	bool launchXbe(const char* path);
	static void onFilePickerClosingCallback(sceneResult result, void* context, scene* scene);
};
