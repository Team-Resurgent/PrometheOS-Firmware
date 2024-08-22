#pragma once

#include "scene.h"

#include "..\xboxInternals.h"

class flashUpdateFlowScene : public scene
{
public:
	flashUpdateFlowScene(bool recovery);
	~flashUpdateFlowScene();
	void update();
	void render();
private:
	uint32_t mCurrentSceneId;
	bool mRecovery;
	char* mFilePath;
	static void onFilePickerClosingCallback(sceneResult result, void* context, scene* scene);
	static void onRFlashUpdateClosingCallback(sceneResult result, void* context, scene* scene);
};
