#include "flashUpdateFlowScene.h"
#include "sceneManager.h"
#include "filePickerScene.h"
#include "flashUpdateScene.h"
#include "audioSettingsScene.h"

#include "..\context.h"
#include "..\drawing.h"
#include "..\component.h"
#include "..\ssfn.h"
#include "..\inputManager.h"
#include "..\hdmiDevice.h"
#include "..\stringUtility.h"
#include "..\xboxInternals.h"
#include "..\fileSystem.h"

flashUpdateFlowScene::flashUpdateFlowScene(bool recovery)
{
	mFilePickerScene = NULL;
	mFlashUpdateScene = NULL;
	mCurrentSceneId = 0;
	mRecovery = recovery;
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
			mFilePickerScene = new filePickerScene(mRecovery ? filePickerTypeUpdateRecovery : filePickerTypeUpdate);
		}
		mFilePickerScene->update();
		if (mFilePickerScene->getSceneResult() == sceneResultCancelled)
		{
			sceneManager::popScene();
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
			mFlashUpdateScene = new flashUpdateScene(mRecovery, mFilePath);
		}
		mFlashUpdateScene->update();
		if (mFlashUpdateScene->getSceneResult() == sceneResultCancelled)
		{
			delete(mFlashUpdateScene);
			sceneManager::popScene();
			return;
		}
		else if (mFlashUpdateScene->getSceneResult() == sceneResultDone)
		{
			delete(mFlashUpdateScene);
			sceneManager::popScene();
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