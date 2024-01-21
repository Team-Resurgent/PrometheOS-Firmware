#pragma once

#include "scene.h"

#include "..\pointerVector.h"

class editScene : public scene
{
public:
	editScene();
	~editScene();
	void update();
	void render();
	sceneResult getSceneResult();
	uint8_t getBankId();
private:
	int mSelectedControl;
	pointerVector* mBanks;
	uint8_t mBankId;
	sceneResult mSceneResult;
};
