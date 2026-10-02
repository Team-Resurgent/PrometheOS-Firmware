#pragma once

#include "scene.h"
#include "sceneManager.h"

class xecuter4SettingsScene : public scene {
  public:
    xecuter4SettingsScene();
    void update();
    void render();

  private:
    int mSelectedControl;
    uint8_t mBacklight;
    uint8_t mContrast;
};
