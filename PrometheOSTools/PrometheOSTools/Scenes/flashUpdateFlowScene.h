#pragma once

#include "scene.h"
#include "filePickerScene.h"
#include "flashUpdateScene.h"

#include "..\xboxInternals.h"

class flashUpdateFlowScene : public scene
{
public:
	flashUpdateFlowScene(bool recovery);
	~flashUpdateFlowScene();
	void update();
	void render();
private:
	filePickerScene* mFilePickerScene;
	flashUpdateScene* mFlashUpdateScene;
	uint32_t mCurrentSceneId;
	bool mRecovery;
	char* mFilePath;
};
