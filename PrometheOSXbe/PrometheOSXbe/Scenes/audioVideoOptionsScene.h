#pragma once

#include "scene.h"
#include "sceneManager.h"

class audioVideoOptionsScene : public scene
{
public:
	audioVideoOptionsScene();
	void update();
	void render();
private:
	int mSelectedControl;
};
