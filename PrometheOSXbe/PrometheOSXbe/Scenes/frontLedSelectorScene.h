#pragma once

#include "scene.h"

#include "..\xboxInternals.h"

class frontLedSelectorScene : public scene
{
public:
	frontLedSelectorScene(const char* ledSequence);
	void update();
	void render();
	char* getLedSequence();
private:
	int mSelectedControl;
	char mLedSequence[4];
};
