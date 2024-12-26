#pragma once

#include "scene.h"
#include "sceneManager.h"

#include "..\xboxinternals.h"

class ledOptionsScene : public scene
{
public:
	ledOptionsScene();
	void update();
	void render();
private:
	void setLedColor(uint8_t ledColor);
private:
	int mSelectedControl;
	int mStatusFormat;
	int mStripFormat;
};
