#pragma once

#include "scene.h"

class videoSettingsScene : public scene
{
public:
	videoSettingsScene();
	void update();
	void render();
private:
	int mSelectedControl;
};
