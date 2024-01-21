#pragma once

#include "scene.h"
#include "restoreEepromPickerScene.h"
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
	restoreEepromPickerScene*	mRestoreEepromPickerScene;
	restoreEepromScene* mRestoreEepromScene;
	uint32_t mCurrentSceneId;
	char* mFilePath;
};
