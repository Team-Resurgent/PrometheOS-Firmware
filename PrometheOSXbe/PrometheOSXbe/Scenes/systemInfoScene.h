#pragma once

#include "scene.h"
#include "..\pointerVector.h"

enum systemInfoCategoryEnum
{ 
	systemInfoCategoryConsole = 0,
	systemInfoCategoryAudio = 1,
	systemInfoCategoryVideo = 2,
	systemInfoCategoryAbout = 3
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
	pointerVector* mInfoItems;
};
