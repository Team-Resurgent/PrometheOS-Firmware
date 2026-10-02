#pragma once

#include "scene.h"

#include "..\pointerVector.h"
#include "..\settingsManager.h"

class launchScene : public scene {
  public:
    launchScene();
    ~launchScene();
    void update();
    void render();
    void launchSdCardFromPath(const char* sdPath, bool safeMode);

  private:
    void setupBanks();
    void launchSdCardPicker();
    void launchSdCard(bool safeMode);
    static void onSdCardPickerClosingCallback(sceneResult result, void* context, scene* scene);

    int mSelectedControl;
    pointerVector<bankDetails*>* mBanks;
};
