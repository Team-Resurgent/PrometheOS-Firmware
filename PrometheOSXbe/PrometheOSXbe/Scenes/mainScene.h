#pragma once

#include "scene.h"

#include "..\xboxInternals.h"

class mainScene : public scene {
  public:
    mainScene();
    void update();
    void render();
    uint8_t smartxxGetManId();
    bool needsBypass();
    void refreshBypass();
    bool attemptBypass();

  private:
    int mSelectedControl;
	uint32_t mCpuUsage;
	uint32_t mFanPct;
    int mCounter;
    bool mNeedsBypass;
};
