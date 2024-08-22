#pragma once

#include "scene.h"

#include "..\utils.h"
#include "..\Threads\flashBank.h"
#include "..\xboxInternals.h"

class flashBankScene : public scene
{
public:
	flashBankScene(const char* filePath, const char* bankName, uint8_t ledColor);
	~flashBankScene();
	void update();
	void render();
private:
	void setProgress(const char* message);
	void processResponse(flashBank::flashBankResponse response);
private:
	bool mDone;
	char* mProgress;
};
