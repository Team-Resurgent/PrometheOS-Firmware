#pragma once

#include "scene.h"
#include "sceneManager.h"

#include "../pointerMap.h"

class formatDriveScene : public scene
{
public:
	formatDriveScene(int32_t harddrive);
	~formatDriveScene();
	void update();
	void render();
private:
	int32_t mHarddrive;
	int32_t mCounter;
	char* mProgress;
	bool mProcessing;
	bool mNeedsReboot;
	char* mIdeModel;
	char* mIdeSerial;
	bool mInitialized;
	bool mAllowFormat;
};
