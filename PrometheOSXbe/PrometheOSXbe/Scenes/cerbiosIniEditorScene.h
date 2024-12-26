#pragma once

#include "scene.h"
#include "sceneManager.h"

#include "..\cerbiosIniHelper.h"

class cerbiosIniEditorScene : public scene
{
public:
	cerbiosIniEditorScene();
	~cerbiosIniEditorScene();
	void update();
	void render();
	char* shortenString(const char* value);
private:
	int mSelectedControl;
	int mMaxOptionCount;
	bool mNeedsSave;
	bool mHasFilePicker;
	bool mShowingFilePicker;
	bool mShowingInfo;
	cerbiosConfig mConfig;
	char* mShortCdPath1;
	char* mShortCdPath2;
	char* mShortCdPath3;
	char* mShortDashPath1;
	char* mShortDashPath2;
	char* mShortDashPath3;
	char* mShortBootAnimPath;
	char* getOptionInfo(int selectedControl);
	static void onPathClosingCallback(sceneResult result, void* context, scene* scene);
	static void onFrontLedClosingCallback(sceneResult result, void* context, scene* scene);
};
