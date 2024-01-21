#pragma once

#include "scene.h"

#include "..\pointerVector.h"

class restoreEepromScene : public scene
{
public:
	restoreEepromScene(const char* filePath);
	~restoreEepromScene();
	void update();
	void render();
	sceneResult getSceneResult();
private:
	char* mFilePath;
	char* mProgress;
	sceneResult mSceneResult;
	bool mDone;
};
