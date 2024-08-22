#pragma once

#include "scene.h"
#include "sceneManager.h"

class lcdOptionsScene : public scene
{
public:
	lcdOptionsScene();
	void update();
	void render();
private:
	int mSelectedControl;
};
