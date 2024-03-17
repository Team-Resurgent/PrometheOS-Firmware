#include "editFlowScene.h"
#include "sceneManager.h"
#include "launchScene.h"
#include "removeScene.h"
#include "editScene.h"
#include "flashBankScene.h"
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

editFlowScene::editFlowScene()
{
	mEditScene = NULL;
	mKeyboardScene = NULL;
	mLedColorSelectorScene = NULL;
	mCurrentSceneId = 0;
	mBankId = 0;
	mBankName = NULL;
}

editFlowScene::~editFlowScene()
{
	delete(mEditScene);
	delete(mKeyboardScene);
	delete(mLedColorSelectorScene);
}

void editFlowScene::update()
{
	if (mCurrentSceneId == 0)
	{
		if (mEditScene == NULL)
		{
			mEditScene = new editScene();
		}
		mEditScene->update();
		if (mEditScene->getSceneResult() == sceneResultCancelled)
		{
			sceneManager::popScene();
			return;
		}
		else if (mEditScene->getSceneResult() == sceneResultDone)
		{
			mBankId = mEditScene->getBankId();
			delete(mEditScene);
			mCurrentSceneId = 1;
		}
		return;
	}

	if (mCurrentSceneId == 1)
	{
		if (mKeyboardScene == NULL)
		{
			bankInfo bank = settingsManager::getBankInfo(mBankId);
			char* bankName = strdup(bank.name);
			mKeyboardScene = new keyboardScene(bankName);
			free(bankName);
		}
		mKeyboardScene->update();
		if (mKeyboardScene->getSceneResult() == sceneResultCancelled)
		{
			sceneManager::popScene();
			return;
		}
		else if (mKeyboardScene->getSceneResult() == sceneResultDone)
		{
			mBankName = mKeyboardScene->getText();
			delete(mKeyboardScene);
			if (context::getModchip()->supportsLed() == true)
			{
				mCurrentSceneId = 2;
			}
			else
			{
				settingsManager::editBank(mBankId, mBankName, 0);
				sceneManager::popScene();
				return;
			}
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
			sceneManager::popScene();
			return;
		}
		else if (mLedColorSelectorScene->getSceneResult() == sceneResultDone)
		{
			settingsManager::editBank(mBankId, mBankName, mLedColorSelectorScene->getLedColor());
			delete(mLedColorSelectorScene);
			sceneManager::popScene();
			return;
		}
	}
}

void editFlowScene::render()
{
	if (mCurrentSceneId == 0)
	{
		if (mEditScene != NULL) 
		{
			mEditScene->render();
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
}