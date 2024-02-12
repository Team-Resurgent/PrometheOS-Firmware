#pragma once

#include "scene.h"

#include "..\pointerVector.h"

enum filePickerType
{ 
	filePickerTypeBios = 0,
	filePickerTypeEeprom = 1,
	filePickerTypeUpdate = 2
}; 

class filePickerScene : public scene
{
public:
	filePickerScene(filePickerType filePickerType);
	~filePickerScene();
	void update();
	void render();
	char* getFilePath();
	sceneResult getSceneResult();
private:
	pointerVector* getFileInfoDetails();
private:
	bool mInitialized;
	filePickerType mFilePickerType;
	int mSelectedControl;
	int mScrollPosition;
	pointerVector* mMountedDrives;
	char* mCurrentPath;
	pointerVector* mFileInfoDetails;
	char* mFilePath;
	sceneResult mSceneResult;
};
