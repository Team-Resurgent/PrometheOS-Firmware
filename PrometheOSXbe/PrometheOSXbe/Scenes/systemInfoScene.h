#pragma once

#include "scene.h"
#include "..\pointerVector.h"

enum systemInfoCategoryEnum {
    systemInfoCategoryConsole = 0,
    systemInfoCategoryStorage = 1,
    systemInfoCategoryAudio = 2,
    systemInfoCategoryVideo = 3,
    systemInfoCategoryModchip = 4,
    systemInfoCategoryAbout = 5
};

class systemInfoScene : public scene {
  public:
    systemInfoScene(systemInfoCategoryEnum systemInfoCategory);
    ~systemInfoScene();
    void update();
    void render();

  private:
    struct partitionInfo {
      char label[20];
      uint64_t total;
      uint64_t used;
    };

    int mSelectedControl;
    systemInfoCategoryEnum mSystemInfoCategory;
    pointerVector<char*>* mInfoItems;
    pointerVector<partitionInfo*>* mPartitions;
};
