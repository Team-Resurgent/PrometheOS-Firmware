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
	uint32_t mCurrentSceneId;
	char* mFilePath;
	char* mBankName;
	uint8_t mLedColor;
	static void onFilePickerClosingCallback(sceneResult result, void* context, scene* scene);
	static void onKeyboardClosingCallback(sceneResult result, void* context, scene* scene);
	static void onLedColorClosingCallback(sceneResult result, void* context, scene* scene);
	static void onFlashBankClosingCallback(sceneResult result, void* context, scene* scene);
};
