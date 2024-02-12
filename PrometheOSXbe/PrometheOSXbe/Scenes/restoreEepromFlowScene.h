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
	filePickerScene*	mFilePickerScene;
	restoreEepromScene* mRestoreEepromScene;
	uint32_t mCurrentSceneId;
	char* mFilePath;
};
