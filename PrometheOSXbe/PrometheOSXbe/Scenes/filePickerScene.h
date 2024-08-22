#pragma once

#include "scene.h"

#include "..\pointerVector.h"
#include "..\fileSystem.h"

enum filePickerType
{ 
	filePickerTypeBios = 0,
	filePickerTypeEeprom = 1,
	filePickerTypeUpdate = 2,
	filePickerTypeUpdateRecovery = 3
}; 

class filePickerScene : public scene
{
public:
	filePickerScene(filePickerType filePickerType);
	~filePickerScene();
	void update();
	void render();
	char* getFilePath();
private:
	pointerVector<fileSystem::FileInfoDetail*>* getFileInfoDetails();
private:
	bool mInitialized;
	filePickerType mFilePickerType;
	int mSelectedControl;
	int mScrollPosition;
	pointerVector<char*>* mMountedDrives;
	char* mCurrentPath;
	pointerVector<fileSystem::FileInfoDetail*>* mFileInfoDetails;
	char* mFilePath;
};
