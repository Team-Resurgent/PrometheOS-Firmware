#include "flashUpdateFlowScene.h"
#include "keyboardScene.h"
#include "sceneManager.h"
#include "launchScene.h"
#include "removeScene.h"
#include "filePickerScene.h"
#include "flashUpdateScene.h"
#include "audioSettingsScene.h"

#include "..\context.h"
#include "..\drawing.h"
#include "..\component.h"
#include "..\ssfn.h"
#include "..\inputManager.h"
#include "..\settingsManager.h"
#include "..\hdmiDevice.h"
#include "..\xenium.h"
#include "..\stringUtility.h"
#include "..\xboxInternals.h"
#include "..\fileSystem.h"

flashUpdateFlowScene::flashUpdateFlowScene()
{
	mFilePickerScene = NULL;
	mFlashUpdateScene = NULL;
	mCurrentSceneId = 0;
	mFilePath = NULL;
}

flashUpdateFlowScene::~flashUpdateFlowScene()
{
	delete(mFilePickerScene);
	delete(mFlashUpdateScene);
	free(mFilePath);
}

void flashUpdateFlowScene::update()
{
	if (mCurrentSceneId == 0)
	{
		if (mFilePickerScene == NULL)
		{
			mFilePickerScene = new filePickerScene(filePickerTypeUpdate);
		}
		mFilePickerScene->update();
		if (mFilePickerScene->getSceneResult() == sceneResultCancelled)
		{
			sceneManager::openScene(sceneItemPrometheOsSettingsScene);
			return;
		}
		else if (mFilePickerScene->getSceneResult() == sceneResultDone)
		{
			mFilePath = mFilePickerScene->getFilePath();
			delete(mFilePickerScene);
			mCurrentSceneId = 1;
		}
		return;
	}

	if (mCurrentSceneId == 1)
	{
		if (mFlashUpdateScene == NULL)
		{
			mFlashUpdateScene = new flashUpdateScene(mFilePath);
		}
		mFlashUpdateScene->update();
		if (mFlashUpdateScene->getSceneResult() == sceneResultCancelled)
		{
			delete(mFlashUpdateScene);
			sceneManager::openScene(sceneItemPrometheOsSettingsScene);
			return;
		}
		else if (mFlashUpdateScene->getSceneResult() == sceneResultDone)
		{
			delete(mFlashUpdateScene);
			sceneManager::openScene(sceneItemPrometheOsSettingsScene);
			return;
		}
	}
}

void flashUpdateFlowScene::render()
{
	if (mCurrentSceneId == 0)
	{
		if (mFilePickerScene != NULL) 
		{
			mFilePickerScene->render();
		}
		return;
	}

	if (mCurrentSceneId == 1)
	{
		if (mFlashUpdateScene != NULL) 
		{
			mFlashUpdateScene->render();
		}
		return;
	}
}