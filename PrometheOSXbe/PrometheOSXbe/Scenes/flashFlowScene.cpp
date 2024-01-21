#include "flashFlowScene.h"
#include "keyboardScene.h"
#include "sceneManager.h"
#include "launchScene.h"
#include "removeScene.h"
#include "flashScene.h"
#include "flashingScene.h"
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

flashFlowScene::flashFlowScene()
{
	mFlashScene = NULL;
	mKeyboardScene = NULL;
	mLedColorSelectorScene = NULL;
	mFlashingScene = NULL;
	mCurrentSceneId = 0;
	mFilePath = NULL;
	mBankName = NULL;
	mLedColor = 0;
}

flashFlowScene::~flashFlowScene()
{
	delete(mFlashScene);
	delete(mKeyboardScene);
	delete(mLedColorSelectorScene);
	free(mFilePath);
}

void flashFlowScene::update()
{
	if (mCurrentSceneId == 0)
	{
		if (mFlashScene == NULL)
		{
			mFlashScene = new flashScene();
		}
		mFlashScene->update();
		if (mFlashScene->getSceneResult() == sceneResultCancelled)
		{
			sceneManager::openScene(sceneItemBankManagementScene);
			return;
		}
		else if (mFlashScene->getSceneResult() == sceneResultDone)
		{
			mFilePath = mFlashScene->getFilePath();
			delete(mFlashScene);
			mCurrentSceneId = 1;
		}
		return;
	}
	
	if (mCurrentSceneId == 1)
	{
		if (mKeyboardScene == NULL)
		{
			char* fileName = fileSystem::getFileNameWithoutExtension(mFilePath);
			mKeyboardScene = new keyboardScene(fileName);
			free(fileName);
		}
		mKeyboardScene->update();
		if (mKeyboardScene->getSceneResult() == sceneResultCancelled)
		{
			sceneManager::openScene(sceneItemBankManagementScene);
			return;
		}
		else if (mKeyboardScene->getSceneResult() == sceneResultDone)
		{
			mBankName = mKeyboardScene->getText();
			delete(mKeyboardScene);
			mCurrentSceneId = 2;
		}
		return;
	}

	if (mCurrentSceneId == 2)
	{
		if (mLedColorSelectorScene == NULL)
		{
			mLedColorSelectorScene = new ledColorSelectorScene();
		}
		mLedColorSelectorScene->update();
		if (mLedColorSelectorScene->getSceneResult() == sceneResultCancelled)
		{
			delete(mLedColorSelectorScene);
			sceneManager::openScene(sceneItemBankManagementScene);
			return;
		}
		else if (mLedColorSelectorScene->getSceneResult() == sceneResultDone)
		{
			mLedColor = mLedColorSelectorScene->getLedColor();
			delete(mLedColorSelectorScene);
			mCurrentSceneId = 3;
		}
		return;
	}

	if (mCurrentSceneId == 3)
	{
		if (mFlashingScene == NULL)
		{
			mFlashingScene = new flashingScene(mFilePath, mBankName, mLedColor);
		}
		mFlashingScene->update();
		if (mFlashingScene->getSceneResult() == sceneResultCancelled)
		{
			delete(mFlashingScene);
			sceneManager::openScene(sceneItemBankManagementScene);
			return;
		}
		else if (mFlashingScene->getSceneResult() == sceneResultDone)
		{
			delete(mFlashingScene);
			sceneManager::openScene(sceneItemBankManagementScene);
			return;
		}
	}
}

void flashFlowScene::render()
{
	if (mCurrentSceneId == 0)
	{
		if (mFlashScene != NULL) 
		{
			mFlashScene->render();
		}
		return;
	}

	if (mCurrentSceneId == 1)
	{
		if (mKeyboardScene != NULL) 
		{
			mKeyboardScene->render();
		}
		return;
	}

	if (mCurrentSceneId == 2)
	{
		if (mLedColorSelectorScene != NULL) 
		{
			mLedColorSelectorScene->render();
		}
		return;
	}

	if (mCurrentSceneId == 3)
	{
		if (mFlashingScene != NULL) 
		{
			mFlashingScene->render();
		}
		return;
	}
}