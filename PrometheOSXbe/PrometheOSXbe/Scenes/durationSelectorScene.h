#pragma once

#include "scene.h"

#include "..\xboxInternals.h"

class durationSelectorScene : public scene
{
public:
	durationSelectorScene(const char* title, const char* label, unsigned int initialSeconds, unsigned int minSeconds, unsigned int maxSeconds, const char* zeroLabel);
	~durationSelectorScene();
	void update();
	void render();
	unsigned int getTotalSeconds();

private:
	unsigned int getRawTotalSeconds();
	unsigned int getFieldValue(int field);
	void setFieldValue(int field, unsigned int value);
	unsigned int getFieldMax(int field);
	const char* getFieldLabel(int field);
	void adjustField(int field, int direction);
	char* buildTotalText();
	static void onKeypadClosingCallback(sceneResult result, void* context, scene* caller);

private:
	int mSelectedControl;
	unsigned int mDays;
	unsigned int mHours;
	unsigned int mMinutes;
	unsigned int mSeconds;
	unsigned int mMinSeconds;
	unsigned int mMaxSeconds;
	unsigned int mMaxDays;
	char* mTitle;
	char* mLabel;
	char* mZeroLabel;
	DWORD mRepeatNextUp;
	DWORD mRepeatNextDown;
};
