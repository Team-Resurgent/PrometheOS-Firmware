#pragma once

#include "scene.h"

#include "..\pointerVector.h"

class launchScene : public scene
{
public:
	launchScene();
	void update();
	void render();
private:
	int mSelectedControl;
};
