#pragma once

#include "scene.h"

class regionSettingsScene : public scene
{
public:
	regionSettingsScene();
	void update();
	void render();
private:
	bool mNeedsRefresh;
	bool mInitialized;
	int mSelectedControl;
};
