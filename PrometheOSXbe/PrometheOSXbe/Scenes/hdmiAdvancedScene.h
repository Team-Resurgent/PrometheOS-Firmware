#pragma once

#include "scene.h"

class hdmiAdvancedScene : public scene
{
public:
	hdmiAdvancedScene();
	void update();
	void render();
private:
	int mSelectedControl;
};
