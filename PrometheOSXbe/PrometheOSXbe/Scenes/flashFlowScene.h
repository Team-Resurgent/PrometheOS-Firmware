#pragma once

#include "scene.h"
#include "flashScene.h"
#include "keyboardScene.h"
#include "ledColorSelectorScene.h"
#include "flashingScene.h"

#include "..\xboxInternals.h"

class flashFlowScene : public scene
{
public:
	flashFlowScene();
	~flashFlowScene();
	void update();
	void render();
private:
	flashScene*	mFlashScene;
	keyboardScene* mKeyboardScene;
	ledColorSelectorScene* mLedColorSelectorScene;
	flashingScene* mFlashingScene;
	uint32_t mCurrentSceneId;
	char* mFilePath;
	char* mBankName;
	uint8_t mLedColor;
};
