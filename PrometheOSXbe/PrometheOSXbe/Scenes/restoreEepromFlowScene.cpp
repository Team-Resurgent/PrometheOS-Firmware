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
#include "..\xenium.h"
#include "..\stringUtility.h"
#include "..\xboxInternals.h"
#include "..\fileSystem.h"

restoreEepromFlowScene::restoreEepromFlowScene()
{
	mRestoreEepromPickerScene = NULL;
	mRestoreEepromScene = NULL;
	mCurrentSceneId = 0;
	mFilePath = NULL;
}

restoreEepromFlowScene::~restoreEepromFlowScene()
{
	delete(mRestoreEepromPickerScene);
	delete(mRestoreEepromScene);
	free(mFilePath);
}

void restoreEepromFlowScene::update()
{
	if (mCurrentSceneId == 0)
	{
		if (mRestoreEepromPickerScene == NULL)
		{
			mRestoreEepromPickerScene = new restoreEepromPickerScene();
		}
		mRestoreEepromPickerScene->update();
		if (mRestoreEepromPickerScene->getSceneResult() == sceneResultCancelled)
		{
			delete(mRestoreEepromPickerScene);
			sceneManager::openScene(sceneItemEepromToolsScene);
			return;
		}
		else if (mRestoreEepromPickerScene->getSceneResult() == sceneResultDone)
		{
			mFilePath = mRestoreEepromPickerScene->getFilePath();
			delete(mRestoreEepromPickerScene);
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
			sceneManager::openScene(sceneItemEepromToolsScene);
			return;
		}
		else if (mRestoreEepromScene->getSceneResult() == sceneResultDone)
		{
			delete(mRestoreEepromScene);
			sceneManager::openScene(sceneItemEepromToolsScene);
			return;
		}
	}
}

void restoreEepromFlowScene::render()
{
	if (mCurrentSceneId == 0)
	{
		if (mRestoreEepromPickerScene != NULL) 
		{
			mRestoreEepromPickerScene->render();
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