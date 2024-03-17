#pragma once

#include "scene.h"
#include "filePickerScene.h"
#include "keyboardScene.h"
#include "ledColorSelectorScene.h"
#include "flashBankScene.h"

#include "..\xboxInternals.h"

class flashFlowScene : public scene
{
public:
	flashFlowScene();
	~flashFlowScene();
	void update();
	void render();
private:
	filePickerScene* mFilePickerScene;
	keyboardScene* mKeyboardScene;
	ledColorSelectorScene* mLedColorSelectorScene;
	flashBankScene* mFlashBankScene;
	uint32_t mCurrentSceneId;
	char* mFilePath;
	char* mBankName;
	uint8_t mLedColor;
};
