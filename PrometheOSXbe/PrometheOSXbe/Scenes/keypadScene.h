#pragma once

#include "scene.h"

#include "..\xboxInternals.h"

class keypadScene : public scene
{
public:
	keypadScene(const char* title, uint32_t address);
	~keypadScene();
	void update();
	void render();
	uint32_t getValue();
private:
	void keyboardButton(bool selected, bool active, char key, int x, int y, int width, int height);
	char* addChar(char* value, int charIndex);
	char* deleteChar(char* value);
private:
	int mSelectedControl;
	char* mTitle;
	char* mIp1;
	char* mIp2;
	char* mIp3;
	char* mIp4;
	int mPart;
	int mCounter;
};
