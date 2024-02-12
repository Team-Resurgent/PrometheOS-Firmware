#include "flashFlowScene.h"
#include "keyboardScene.h"
#include "sceneManager.h"
#include "launchScene.h"
#include "removeScene.h"
#include "filePickerScene.h"
#include "flashBankScene.h"
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
	mFilePickerScene = NULL;
	mKeyboardScene = NULL;
	mLedColorSelectorScene = NULL;
	mFlashBankScene = NULL;
	mCurrentSceneId = 0;
	mFilePath = NULL;
	mBankName = NULL;
	mLedColor = 0;
}

flashFlowScene::~flashFlowScene()
{
	delete(mFilePickerScene);
	delete(mKeyboardScene);
	delete(mLedColorSelectorScene);
	delete(mFlashBankScene);
	free(mFilePath);
}

void flashFlowScene::update()
{
	if (mCurrentSceneId == 0)
	{
		if (mFilePickerScene == NULL)
		{
			mFilePickerScene = new filePickerScene(filePickerTypeBios);
		}
		mFilePickerScene->update();
		if (mFilePickerScene->getSceneResult() == sceneResultCancelled)
		{
			sceneManager::openScene(sceneItemBankManagementScene);
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
		if (mFlashBankScene == NULL)
		{
			mFlashBankScene = new flashBankScene(mFilePath, mBankName, mLedColor);
		}
		mFlashBankScene->update();
		if (mFlashBankScene->getSceneResult() == sceneResultCancelled)
		{
			delete(mFlashBankScene);
			sceneManager::openScene(sceneItemBankManagementScene);
			return;
		}
		else if (mFlashBankScene->getSceneResult() == sceneResultDone)
		{
			delete(mFlashBankScene);
			sceneManager::openScene(sceneItemBankManagementScene);
			return;
		}
	}
}

void flashFlowScene::render()
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
		if (mFlashBankScene != NULL) 
		{
			mFlashBankScene->render();
		}
		return;
	}
}