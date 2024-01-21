#pragma once

#include "scene.h"
#include "keypadScene.h"

#include "..\xboxInternals.h"
#include "..\settingsManager.h"

class networkSettingsScene : public scene
{
public:
	networkSettingsScene();
	~networkSettingsScene();
	void update();
	void render();
private:
	bool hasNetworkChanges();
private:
	int mSelectedControl;
	keypadScene* mKeypadScene;
	networkModeEnum mNetworkMode;
	uint32_t mAddress;
	uint32_t mSubnet;
	uint32_t mGateway;
	uint32_t mPrimaryDns;
	uint32_t mSecondaryDns;
	bool mRefreshNetwork;
};
