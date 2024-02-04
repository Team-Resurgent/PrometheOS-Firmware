#pragma once

#include "scene.h"

#include "..\Threads\hddLockUnlock.h"

class hddLockUnlockScene : public scene
{
public:
	hddLockUnlockScene();
	~hddLockUnlockScene();
	void update();
	void render();
private:
	void setProgress(const char* message);
	void processResponse(hddLockUnlock::hddLockUnlockResponse response);
private:
	bool mHddLocked;
	int mStep;
	char* mProgress;
	char* mIdeModel;
	char* mIdeSerial;
	bool mFailed;
	bool mInitialized;
};
