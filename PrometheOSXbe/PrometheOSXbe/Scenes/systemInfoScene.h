#pragma once

#include "scene.h"
#include "..\pointerVector.h"

enum systemInfoCategoryEnum
{ 
	systemInfoCategoryConsole = 0,
	systemInfoCategoryStorage = 1,
	systemInfoCategoryAudio = 2,
	systemInfoCategoryVideo = 3,
	systemInfoCategoryAbout = 4
}; 

class systemInfoScene : public scene
{
public:
	systemInfoScene(systemInfoCategoryEnum systemInfoCategory);
	~systemInfoScene();
	void update();
	void render();
private:
	int mSelectedControl;
	systemInfoCategoryEnum mSystemInfoCategory;
	pointerVector<char*>* mInfoItems;
};
