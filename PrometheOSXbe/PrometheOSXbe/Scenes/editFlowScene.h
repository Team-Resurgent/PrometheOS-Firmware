#pragma once

#include "scene.h"
#include "editScene.h"
#include "keyboardScene.h"
#include "ledColorSelectorScene.h"
#include "flashBankScene.h"

#include "..\xboxInternals.h"

class editFlowScene : public scene
{
public:
	editFlowScene();
	void update();
	void render();
private:
	uint32_t mCurrentSceneId;
	uint8_t mBankId;
	char* mBankName;
	static void onEditClosingCallback(sceneResult result, void* context, scene* scene);
	static void onKeyboardClosingCallback(sceneResult result, void* context, scene* scene);
	static void onLedColorClosingCallback(sceneResult result, void* context, scene* scene);
};
