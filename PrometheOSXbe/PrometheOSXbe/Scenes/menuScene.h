#pragma once

#include "scene.h"
#include "sceneManager.h"

#include "../pointerMap.h"
#include "../utils.h"

class menuScene : public scene
{
public:
	menuScene(const char* title, const char* subTitle, pointerVector<utils::intContainer*>* sceneItems);
	~menuScene();
	void setSubTilte(const char* subTitle);
	void update();
	void render();
private:
	int mSelectedControl;
	char* mTitle;
	char* mSubTitle;
	pointerVector<utils::intContainer*>* mSceneItems;
};
