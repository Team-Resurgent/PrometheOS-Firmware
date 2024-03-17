#pragma once

#include "scene.h"

class backupEepromScene : public scene
{
public:
	backupEepromScene();
	~backupEepromScene();
	void update();
	void render();
private:
	char* mProgress;
	bool mDone;
};
