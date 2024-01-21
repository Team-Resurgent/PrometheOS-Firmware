#pragma once

#include "scene.h"

class keyboardScene : public scene
{
public:
	keyboardScene(const char* text);
	~keyboardScene();
	void update();
	void render();
	char* getText();
	sceneResult getSceneResult();
private:
	void keyboardButton(bool selected, bool active, char key, int x, int y, int width, int height);
private:
	int mSelectedControl;
	bool mCapitals;
	bool mSymbols;
	char* mText;
	sceneResult mSceneResult;
	int mCounter;
};
