#pragma once

#include "scene.h"

#include "..\pointerVector.h"
#include "..\settingsManager.h"

class removeScene : public scene
{
public:
	removeScene();
	~removeScene();
	void update();
	void render();
private:
	int mSelectedControl;
	pointerVector<bankDetails*>* mBanks;
};
