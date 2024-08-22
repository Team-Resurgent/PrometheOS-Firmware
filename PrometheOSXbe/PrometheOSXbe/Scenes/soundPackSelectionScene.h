#pragma once

#include "scene.h"
#include "sceneManager.h"

#include "../pointerMap.h"

class soundPackSelectionScene : public scene
{
public:
	soundPackSelectionScene();
	~soundPackSelectionScene();
	void update();
	void render();
private:
	int mSelectedControl;
	pointerVector<char*>* mMenuItems;
};
