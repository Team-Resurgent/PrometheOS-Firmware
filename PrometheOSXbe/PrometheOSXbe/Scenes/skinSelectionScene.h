#pragma once

#include "scene.h"
#include "sceneManager.h"

#include "../pointerMap.h"

class skinSelectionScene : public scene
{
public:
	skinSelectionScene();
	~skinSelectionScene();
	void update();
	void render();
private:
	int mSelectedControl;
	pointerVector<char*>* mMenuItems;
};
