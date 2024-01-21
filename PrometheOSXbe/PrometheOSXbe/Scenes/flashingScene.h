#pragma once

#include "scene.h"

#include "..\utils.h"
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
	char* mFilePath;
	char* mBankName;
	uint8_t mLedColor;
	uint8_t mStep;
	char* mProgress;
	utils::dataContainer* mBankData;
	uint8_t mBankId;
	sceneResult mSceneResult;
};
