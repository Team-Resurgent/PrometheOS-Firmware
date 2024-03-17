#pragma once

#include "scene.h"
#include "sceneManager.h"

#include "../pointerMap.h"

class menuScene : public scene
{
public:
	menuScene(const char* title, const char* subTitle, pointerVector* sceneItems);
	~menuScene();
	void update();
	void render();
private:
	int mSelectedControl;
	char* mTitle;
	char* mSubTitle;
	pointerVector* mSceneItems;
};
