#pragma once

#include "scene.h"
#include "sceneManager.h"

#include "../cerbiosIniHelper.h"
#include "../pointerVector.h"
#include "../ListItems/listItem.h"

class cerbiosIniEditorScene : public scene {
  public:
    cerbiosIniEditorScene(int page);
    ~cerbiosIniEditorScene();
    void refreshPage();
    void update();
    void render();
    cerbiosConfig* getConfig();

  private:
    int mSelectedControl;
    int mPage;
    bool mShowInfo;
    pointerVector<listItem*>* mSceneItems;
};
