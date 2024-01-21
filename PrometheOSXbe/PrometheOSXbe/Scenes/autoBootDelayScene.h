#pragma once

#include "scene.h"
#include "sceneManager.h"

#include "../pointerMap.h"

class autoBootDelayScene : public scene
{
public:
	autoBootDelayScene();
	~autoBootDelayScene();
	void update();
	void render();
private:
	int mSelectedControl;
	pointerVector* mMenuItems;
};
