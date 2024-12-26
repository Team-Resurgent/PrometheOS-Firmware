#pragma once

#include "scene.h"

#include "..\xboxInternals.h"

class keyboardScene : public scene
{
public:
	keyboardScene(uint32_t maxLen, const char* title, const char* label, const char* text);
	~keyboardScene();
	void update();
	void render();
	char* getText();
	void clearText();
	void calcShortStringRange(const char* value, int& startPos, int &endPos);
private:
	void keyboardButton(bool selected, bool active, char key, int x, int y, int width, int height);
private:
	int mSelectedControl;
	bool mCapitals;
	bool mSymbols;
	uint32_t mMaxLen;
	char* mTitle;
	char* mLabel;
	char* mText;
	int mCounter;
	int mCursorPos;
	int mShortTextStart;
	int mShortTextEnd;
};
