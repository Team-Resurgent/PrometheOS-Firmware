#include "ledColorFlowScene.h"
#include "ledColorSelectorScene.h"
#include "sceneManager.h"

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

ledColorFlowScene::ledColorFlowScene()
{
	mLedColorSelectorScene = new ledColorSelectorScene();
}

ledColorFlowScene::~ledColorFlowScene()
{
	delete(mLedColorSelectorScene);
}

void ledColorFlowScene::update()
{
	mLedColorSelectorScene->update();
	if (mLedColorSelectorScene->getSceneResult() == sceneResultCancelled)
	{
		delete(mLedColorSelectorScene);
		sceneManager::openScene(sceneItemPrometheOsSettingsScene);
	}
	else if (mLedColorSelectorScene->getSceneResult() == sceneResultDone)
	{
		settingsManager::setLedColor(mLedColorSelectorScene->getLedColor());
		delete(mLedColorSelectorScene);
		sceneManager::openScene(sceneItemPrometheOsSettingsScene);
	}
}

void ledColorFlowScene::render()
{
	mLedColorSelectorScene->render();
}