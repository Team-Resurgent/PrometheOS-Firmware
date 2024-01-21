#pragma once

#include "scene.h"
#include "ledColorSelectorScene.h"

#include "..\xboxInternals.h"

class ledColorFlowScene : public scene
{
public:
	ledColorFlowScene();
	~ledColorFlowScene();
	void update();
	void render();
private:
	ledColorSelectorScene* mLedColorSelectorScene;
};
