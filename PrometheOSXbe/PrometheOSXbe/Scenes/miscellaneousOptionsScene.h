#pragma once

#include "scene.h"
#include "sceneManager.h"

class miscellaneousOptionsScene : public scene
{
public:
	miscellaneousOptionsScene();
	void update();
	void render();
private:
	int mSelectedControl;
};
