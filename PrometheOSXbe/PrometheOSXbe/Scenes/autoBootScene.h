#pragma once

#include "scene.h"

class autoBootScene : public scene
{
public:
	autoBootScene();
	void update();
	void render();
private:
	int mCounter;
};
