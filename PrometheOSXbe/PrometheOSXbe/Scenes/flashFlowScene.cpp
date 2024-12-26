#include "flashFlowScene.h"
#include "keyboardScene.h"
#include "sceneManager.h"
#include "launchScene.h"
#include "removeScene.h"
#include "filePickerScene.h"
#include "ledColorSelectorScene.h"
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

void flashFlowScene::onFilePickerClosingCallback(sceneResult result, void* context, scene* scene)
{
	flashFlowScene* self = (flashFlowScene*)context;
	if (result == sceneResultCancelled)
	{
		self->mCurrentSceneId = 4;
		return;
	}
	filePickerScene* closingScene = (filePickerScene*)scene;
	self->mFilePath = closingScene->getFilePath();
	self->mCurrentSceneId = 1;
}

void flashFlowScene::onKeyboardClosingCallback(sceneResult result, void* context, scene* scene)
{
	flashFlowScene* self = (flashFlowScene*)context;
	if (result == sceneResultCancelled)
	{
		self->mCurrentSceneId = 4;
		return;
	}
	keyboardScene* closingScene = (keyboardScene*)scene;
	self->mBankName = closingScene->getText();
	self->mCurrentSceneId = context::getModchip()->getSupportInfo(true).supportsLed == true ? 2 : 3;
}

void flashFlowScene::onLedColorClosingCallback(sceneResult result, void* context, scene* scene)
{
	flashFlowScene* self = (flashFlowScene*)context;
	if (result == sceneResultCancelled)
	{
		self->mCurrentSceneId = 4;
		return;
	}
	ledColorSelectorScene* closingScene = (ledColorSelectorScene*)scene;
	self->mLedColor = closingScene->getLedColor();
	self->mCurrentSceneId = 3;
}

void flashFlowScene::onFlashBankClosingCallback(sceneResult result, void* context, scene* scene)
{
	flashFlowScene* self = (flashFlowScene*)context;
	self->mCurrentSceneId = 4;
}

flashFlowScene::flashFlowScene()
{
	mCurrentSceneId = 0;
	mFilePath = NULL;
	mBankName = NULL;
	mLedColor = 0;
}

flashFlowScene::~flashFlowScene()
{
	free(mFilePath);
}

void flashFlowScene::update()
{
	if (mCurrentSceneId == 0)
	{
		sceneContainer* container = new sceneContainer(sceneItemGenericScene, new filePickerScene(filePickerTypeBios), "", this, onFilePickerClosingCallback);
		sceneManager::pushScene(container);
		return;
	}
	
	if (mCurrentSceneId == 1)
	{
		char* fileName = fileSystem::getFileNameWithoutExtension(mFilePath);
		sceneContainer* container = new sceneContainer(sceneItemGenericScene, new keyboardScene(63, "Please enter a bank name...", "Bank Name:", fileName), "", this, onKeyboardClosingCallback);
		sceneManager::pushScene(container);
		return;
	}

	if (mCurrentSceneId == 2)
	{
		sceneContainer* container = new sceneContainer(sceneItemGenericScene, new ledColorSelectorScene(), "", this, onLedColorClosingCallback);
		sceneManager::pushScene(container);
		return;
	}

	if (mCurrentSceneId == 3)
	{
		sceneContainer* container = new sceneContainer(sceneItemGenericScene, new flashBankScene(mFilePath, mBankName, mLedColor), "", this, onFlashBankClosingCallback);
		sceneManager::pushScene(container);
	}

	if (mCurrentSceneId == 4)
	{
		sceneManager::popScene();
	}
}

void flashFlowScene::render()
{
}