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

void flashUpdateFlowScene::onFilePickerClosingCallback(sceneResult result, void* context, scene* scene)
{
	flashUpdateFlowScene* self = (flashUpdateFlowScene*)context;
	if (result == sceneResultCancelled)
	{
		self->mCurrentSceneId = 2;
		return;
	}
	filePickerScene* closingScene = (filePickerScene*)scene;
	self->mFilePath = closingScene->getFilePath();
	self->mCurrentSceneId = 1;
}

void flashUpdateFlowScene::onRFlashUpdateClosingCallback(sceneResult result, void* context, scene* scene)
{
	flashUpdateFlowScene* self = (flashUpdateFlowScene*)context;
	self->mCurrentSceneId = 2;
}

flashUpdateFlowScene::flashUpdateFlowScene(bool recovery)
{
	mCurrentSceneId = 0;
	mRecovery = recovery;
	mFilePath = NULL;
}

flashUpdateFlowScene::~flashUpdateFlowScene()
{
	free(mFilePath);
}

void flashUpdateFlowScene::update()
{
	if (mCurrentSceneId == 0)
	{
		sceneContainer* container = new sceneContainer(sceneItemGenericScene, new filePickerScene(mRecovery ? filePickerTypeUpdateRecovery : filePickerTypeUpdate), "", this, onFilePickerClosingCallback);
		sceneManager::pushScene(container);
		return;
	}

	if (mCurrentSceneId == 1)
	{
		sceneContainer* container = new sceneContainer(sceneItemGenericScene, new flashUpdateScene(mRecovery, mFilePath), "", this, onFilePickerClosingCallback);
		sceneManager::pushScene(container);
		return;
	}

	if (mCurrentSceneId == 2)
	{
		sceneManager::popScene();
	}
}

void flashUpdateFlowScene::render()
{
}