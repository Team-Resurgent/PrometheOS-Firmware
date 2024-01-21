#pragma once

#include "scene.h"

#include "..\pointerVector.h"

class removeScene : public scene
{
public:
	removeScene();
	~removeScene();
	void update();
	void render();
private:
	int mSelectedControl;
	pointerVector* mBanks;
};
