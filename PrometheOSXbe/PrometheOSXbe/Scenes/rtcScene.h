#pragma once

#include "scene.h"

#include "..\rtcManager.h"

class rtcScene : public scene
{
public:
	rtcScene();
	void update();
	void render();
private:
	int mSelectedControl;
	bool mHasRtcExpansion;
	bool mShouldApply;
	rtcDateTime mDateTime;
};
