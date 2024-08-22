#pragma once

#include "scene.h"

#include "..\utils.h"
#include "..\Threads\flashBackup.h"
#include "..\xboxInternals.h"

class flashBackupScene : public scene
{
public:
	flashBackupScene(bool recovery);
	~flashBackupScene();
	void update();
	void render();
private:
	void setProgress(const char* message);
	void processResponse(flashBackup::flashBackupResponse response);
private:
	bool mDone;
	char* mProgress;
};
