#include "restoreEepromFlowScene.h"
#include "sceneManager.h"
#include "launchScene.h"
#include "removeScene.h"
#include "restoreEepromScene.h"
#include "audioSettingsScene.h"

#include "..\context.h"
#include "..\drawing.h"
#include "..\component.h"
#include "..\ssfn.h"
#include "..\inputManager.h"
#include "..\settingsManager.h"
#include "..\hdmiDevice.h"
#include "..\stringUtility.h"
#include "..\xboxInternals.h"
#include "..\fileSystem.h"

restoreEepromFlowScene::restoreEepromFlowScene()
{
	mFilePickerScene = NULL;
	mRestoreEepromScene = NULL;
	mCurrentSceneId = 0;
	mFilePath = NULL;
}

restoreEepromFlowScene::~restoreEepromFlowScene()
{
	delete(mFilePickerScene);
	delete(mRestoreEepromScene);
	free(mFilePath);
}

void restoreEepromFlowScene::update()
{
	if (mCurrentSceneId == 0)
	{
		if (mFilePickerScene == NULL)
		{
			mFilePickerScene = new filePickerScene(filePickerTypeEeprom);
		}
		mFilePickerScene->update();
		if (mFilePickerScene->getSceneResult() == sceneResultCancelled)
		{
			delete(mFilePickerScene);
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
		if (mRestoreEepromScene == NULL)
		{
			mRestoreEepromScene = new restoreEepromScene(mFilePath);
		}
		mRestoreEepromScene->update();
		if (mRestoreEepromScene->getSceneResult() == sceneResultCancelled)
		{
			delete(mRestoreEepromScene);
			sceneManager::popScene();
			return;
		}
		else if (mRestoreEepromScene->getSceneResult() == sceneResultDone)
		{
			delete(mRestoreEepromScene);
			sceneManager::popScene();
			return;
		}
	}
}

void restoreEepromFlowScene::render()
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
		if (mRestoreEepromScene != NULL) 
		{
			mRestoreEepromScene->render();
		}
		return;
	}
}