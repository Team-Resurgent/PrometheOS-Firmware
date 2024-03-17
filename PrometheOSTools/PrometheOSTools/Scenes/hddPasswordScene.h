#pragma once

#include "scene.h"

class hddPasswordScene : public scene
{
public:
	hddPasswordScene();
	void update();
	void render();
private:
	int mSelectedControl;
};
