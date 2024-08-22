#pragma once

#include "scene.h"

#include "..\pointerVector.h"
#include "..\settingsManager.h"

class launchScene : public scene
{
public:
	launchScene();
	~launchScene();
	void update();
	void render();
private:
	void setupBanks();

	int mSelectedControl;
	pointerVector<bankDetails*>* mBanks;
};
