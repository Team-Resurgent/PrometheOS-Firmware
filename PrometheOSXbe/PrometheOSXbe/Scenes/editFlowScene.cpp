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

void editFlowScene::onEditClosingCallback(sceneResult result, void* context, scene* scene)
{
	editFlowScene* self = (editFlowScene*)context;
	if (result == sceneResultCancelled)
	{
		self->mCurrentSceneId = 3;
		return;
	}
	editScene* closingScene = (editScene*)scene;
	self->mBankId = closingScene->getBankId();
	self->mCurrentSceneId = 1;
}

void editFlowScene::onKeyboardClosingCallback(sceneResult result, void* context, scene* scene)
{
	editFlowScene* self = (editFlowScene*)context;
	if (result == sceneResultCancelled)
	{
		self->mCurrentSceneId = 3;
		return;
	}
	keyboardScene* closingScene = (keyboardScene*)scene;
	self->mBankName = closingScene->getText();
	self->mCurrentSceneId = context::getModchip()->getSupportInfo(true).supportsLed == true ? 2 : 3;
}

void editFlowScene::onLedColorClosingCallback(sceneResult result, void* context, scene* scene)
{
	editFlowScene* self = (editFlowScene*)context;
	if (result == sceneResultCancelled)
	{
		self->mCurrentSceneId = 3;
		return;
	}
	ledColorSelectorScene* closingScene = (ledColorSelectorScene*)scene;
	settingsManager::editBank(self->mBankId, self->mBankName, closingScene->getLedColor());
	self->mCurrentSceneId = 3;
}

editFlowScene::editFlowScene()
{
	mCurrentSceneId = 0;
	mBankId = 0;
	mBankName = NULL;
}

void editFlowScene::update()
{
	if (mCurrentSceneId == 0)
	{
		sceneContainer* container = new sceneContainer(sceneItemGenericScene, new editScene(), "", this, onEditClosingCallback);
		sceneManager::pushScene(container);
		return;
	}

	if (mCurrentSceneId == 1)
	{
		bankInfo bank = settingsManager::getBankInfo(mBankId);
		char* bankName = strdup(bank.name);
		sceneContainer* container = new sceneContainer(sceneItemGenericScene, new keyboardScene(63, "Please enter a bank name...", "Bank Name:", bankName), "", this, onKeyboardClosingCallback);
		free(bankName);
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
		sceneManager::popScene();
	}
}

void editFlowScene::render()
{
}