#include "launcherFlowScene.h"
#include "sceneManager.h"
#include "filePickerScene.h"

#include "..\context.h"
#include "..\drawing.h"
#include "..\component.h"
#include "..\ssfn.h"
#include "..\inputManager.h"
#include "..\settingsManager.h"
#include "..\hdmiDevice.h"
#include "..\stringUtility.h"
#include "..\xboxInternals.h"
#include "..\driveManager.h"
#include "..\fileSystem.h"

void launcherFlowScene::onFilePickerClosingCallback(sceneResult result, void* context, scene* scene)
{
	launcherFlowScene* self = (launcherFlowScene*)context;
	if (result == sceneResultCancelled)
	{
		self->mCurrentSceneId = 2;
		return;
	}
	filePickerScene* closingScene = (filePickerScene*)scene;
	self->mFilePath = closingScene->getFilePath();
	self->mCurrentSceneId = 1;
}

launcherFlowScene::launcherFlowScene()
{
	mCurrentSceneId = 0;
	mFilePath = NULL;
}

launcherFlowScene::~launcherFlowScene()
{
	free(mFilePath);
}

void launcherFlowScene::update()
{
	if (mCurrentSceneId == 0)
	{
		sceneContainer* container = new sceneContainer(sceneItemGenericScene, new filePickerScene(filePickerTypeXbe), "", this, onFilePickerClosingCallback);
		sceneManager::pushScene(container);
		return;
	}
	
	if (mCurrentSceneId == 1)
	{
		if (launchXbe(mFilePath) == false)
		{
			mCurrentSceneId = 2;
			return;
		}
	}

	if (mCurrentSceneId == 2)
	{
		sceneManager::popScene();
	}
}

void launcherFlowScene::render()
{
}

bool launcherFlowScene::launchXbe(const char* path)
{
	int result = 0;

	char* mountPointToDelete = strdup("\\??\\D:");
	STRING sMountPointToDelete = {(USHORT)strlen(mountPointToDelete), (USHORT)strlen(mountPointToDelete) + 1, mountPointToDelete};
	result |= IoDeleteSymbolicLink(&sMountPointToDelete);

	char* mountPoint = strdup("\\??\\D:");
	STRING sMountPoint = {(USHORT)strlen(mountPoint), (USHORT)strlen(mountPoint) + 1, mountPoint};
	char* tempPath = fileSystem::getDirectory(path);
	char* systemPath = driveManager::convertMountPointToSystemPath(tempPath);
	STRING sSystemPath = {(USHORT)strlen(systemPath), (USHORT)strlen(systemPath) + 1, systemPath};
	result |= IoCreateSymbolicLink(&sMountPoint, &sSystemPath);

	if (result != 0)
	{
		return false;
	}

	free(mountPointToDelete);
	free(mountPoint);
	free(tempPath);
	free(systemPath);

	char* fileName = fileSystem::getFileName(path);
	char* launchPath = stringUtility::formatString("D:\\%s", fileName);
	free(fileName);

	XLaunchNewImage(launchPath, NULL);
	return false;
}