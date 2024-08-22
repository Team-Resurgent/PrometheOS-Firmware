#pragma once

#include "scene.h"

#include "..\utils.h"
#include "..\Threads\flashUpdate.h"
#include "..\xboxInternals.h"

class flashUpdateScene : public scene
{
public:
	flashUpdateScene(bool recovery, const char* filePath);
	~flashUpdateScene();
	void update();
	void render();
private:
	void setProgress(const char* message);
	void processResponse(flashUpdate::flashUpdateResponse response);
private:
	bool mStarted;
	bool mDone;
	bool mFailed;
	char* mProgress;
	char* mFilePath;
	bool mRecovery;
	bool mUpdate;
};
