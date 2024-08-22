#pragma once

#include "scene.h"
#include "sceneManager.h"

class bootOptionsScene : public scene
{
public:
	bootOptionsScene();
	void update();
	void render();
private:
	int mSelectedControl;
};
