#pragma once

#include "scene.h"

#include "..\pointerVector.h"
#include "..\settingsManager.h"

class editScene : public scene
{
public:
	editScene();
	~editScene();
	void update();
	void render();
	uint8_t getBankId();
private:
	int mSelectedControl;
	pointerVector<bankDetails*>* mBanks;
	uint8_t mBankId;
};
