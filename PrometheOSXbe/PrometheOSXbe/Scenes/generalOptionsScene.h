#pragma once

#include "scene.h"
#include "sceneManager.h"

class generalOptionsScene : public scene
{
public:
	generalOptionsScene();
	void update();
	void render();
private:
	int mSelectedControl;
};
