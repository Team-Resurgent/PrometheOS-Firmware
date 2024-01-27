#pragma once

#include "scene.h"

#include "..\utils.h"
#include "..\flash.h"
#include "..\xboxInternals.h"

class flashingScene : public scene
{
public:
	flashingScene(const char* filePath, const char* bankName, uint8_t ledColor);
	~flashingScene();
	void update();
	void render();
	sceneResult getSceneResult();
private:
	void setProgress(const char* message);
	void processResponse(flash::flashResponse response);
private:
	bool mDone;
	char* mProgress;
	sceneResult mSceneResult;
};
