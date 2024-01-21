#pragma once

#include "scene.h"

class audioSettingsScene : public scene
{
public:
	audioSettingsScene();
	void update();
	void render();
private:
	int mSelectedControl;
};
