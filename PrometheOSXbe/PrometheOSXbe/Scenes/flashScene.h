#pragma once

#include "scene.h"

#include "..\pointerVector.h"

class flashScene : public scene
{
public:
	flashScene();
	~flashScene();
	void update();
	void render();
	char* getFilePath();
	sceneResult getSceneResult();
private:
	pointerVector* getFileInfoDetails();
private:
	bool mInitialized;
	int mSelectedControl;
	int mScrollPosition;
	pointerVector* mMountedDrives;
	char* mCurrentPath;
	pointerVector* mFileInfoDetails;
	char* mFilePath;
	sceneResult mSceneResult;
};
