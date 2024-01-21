#pragma once

#include "scene.h"

class hddLockUnlockScene : public scene
{
public:
	hddLockUnlockScene();
	~hddLockUnlockScene();
	void update();
	void render();
private:
	void setProgress(const char* message);
	void getIdeInfo();
	void lockHDD();
	void unlockHDD();
private:
	int mSelectedControl;
	bool mHDDLocked;
	int mStep;
	char* mProgress;
	char* mIdeModel;
	char* mIdeSerial;
};
