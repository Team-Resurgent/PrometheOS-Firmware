#pragma once

#include "scene.h"

#include "..\xboxInternals.h"

class ledColorSelectorScene : public scene
{
public:
	ledColorSelectorScene();
	void update();
	void render();
	uint8_t getLedColor();
	sceneResult getSceneResult();
private:
	int mSelectedControl;
	uint8_t mCancelled;
	uint8_t mLedColor;
	sceneResult mSceneResult;
};
