#include "restoreEepromFlowScene.h"
#include "sceneManager.h"
#include "restoreEepromScene.h"
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
#include "..\fileSystem.h"

void restoreEepromFlowScene::onFilePickerClosingCallback(sceneResult result, void* context, scene* scene)
{
	restoreEepromFlowScene* self = (restoreEepromFlowScene*)context;
	if (result == sceneResultCancelled)
	{
		self->mCurrentSceneId = 2;
		return;
	}
	filePickerScene* closingScene = (filePickerScene*)scene;
	self->mFilePath = closingScene->getFilePath();
	self->mCurrentSceneId = 1;
}

void restoreEepromFlowScene::onRestoreEepromClosingCallback(sceneResult result, void* context, scene* scene)
{
	restoreEepromFlowScene* self = (restoreEepromFlowScene*)context;
	self->mCurrentSceneId = 2;
}

restoreEepromFlowScene::restoreEepromFlowScene()
{
	mCurrentSceneId = 0;
	mFilePath = NULL;
}

restoreEepromFlowScene::~restoreEepromFlowScene()
{
	free(mFilePath);
}

void restoreEepromFlowScene::update()
{
	if (mCurrentSceneId == 0)
	{
		sceneContainer* container = new sceneContainer(sceneItemGenericScene, new filePickerScene(filePickerTypeEeprom), "", this, onFilePickerClosingCallback);
		sceneManager::pushScene(container);
		return;
	}

	if (mCurrentSceneId == 1)
	{
		sceneContainer* container = new sceneContainer(sceneItemGenericScene, new restoreEepromScene(mFilePath), "", this, onRestoreEepromClosingCallback);
		sceneManager::pushScene(container);
		return;
	}

	if (mCurrentSceneId == 2)
	{
		sceneManager::popScene();
	}
}

void restoreEepromFlowScene::render()
{
}