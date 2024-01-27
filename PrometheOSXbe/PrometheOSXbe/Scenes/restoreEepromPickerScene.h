#pragma once

#include "scene.h"

#include "..\pointerVector.h"

class restoreEepromPickerScene : public scene
{
public:
	restoreEepromPickerScene();
	~restoreEepromPickerScene();
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
	char* mCurrentPath;
	pointerVector* mFileInfoDetails;
	char* mFilePath;
	sceneResult mSceneResult;
};
