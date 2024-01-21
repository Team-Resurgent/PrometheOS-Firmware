#pragma once

#include "scene.h"
#include "editScene.h"
#include "keyboardScene.h"
#include "ledColorSelectorScene.h"
#include "flashingScene.h"

#include "..\xboxInternals.h"

class editFlowScene : public scene
{
public:
	editFlowScene();
	~editFlowScene();
	void update();
	void render();
private:
	editScene*	mEditScene;
	keyboardScene* mKeyboardScene;
	ledColorSelectorScene* mLedColorSelectorScene;
	uint32_t mCurrentSceneId;
	uint8_t mBankId;
	char* mBankName;
};
