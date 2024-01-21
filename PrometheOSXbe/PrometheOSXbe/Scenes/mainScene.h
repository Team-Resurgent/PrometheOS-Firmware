#pragma once

#include "scene.h"

#include "..\xboxInternals.h"

class mainScene : public scene
{
public:
	mainScene();
	~mainScene();
	void update();
	void render();
private:
	int mSelectedControl;
	char* mIpAddress;
	int mCounter;
	char* mFreeMemory;
	uint32_t mCpuTempReading;
	uint32_t mMbTempReading;
};
