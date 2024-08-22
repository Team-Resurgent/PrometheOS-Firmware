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
private:
	char* mFilePath;
	char* mProgress;
	bool mDone;
};
