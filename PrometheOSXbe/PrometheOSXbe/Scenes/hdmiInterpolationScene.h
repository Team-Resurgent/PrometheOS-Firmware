#pragma once

#include "scene.h"

class hdmiInterpolationScene : public scene
{
public:
	hdmiInterpolationScene();
	void update();
	void render();
private:
	int mSelectedControl;
};
