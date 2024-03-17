#pragma once

#include "scene.h"

class hdmiVideoSettingsScene : public scene
{
public:
	hdmiVideoSettingsScene();
	void update();
	void render();
private:
	int mSelectedControl;
};
