#pragma once

#include "listItem.h"
#include "..\Scenes\sceneManager.h"

class listItemScene;

typedef const char* (*ListItemSceneGetValueCallback)(listItemScene* sender, void* context);
typedef void (*ListItemSceneSetValueCallback)(listItemScene* sender, void* context, const char* value);

class listItemScene : public listItem {
  public:
    listItemScene(int id, const char* name, sceneItemEnum sceneItem, const char* description, void* context,
        ListItemSceneGetValueCallback listItemSceneGetValueCallback,
        ListItemSceneSetValueCallback listItemSceneSetValueCallback,
        SceneOnClosingCallback onSceneClosingCallback);
    ~listItemScene();
    listItemType getType();
    const char* getValue();
    void setValue(const char* value);
    void* getContext();
    void executeScene();

  private:
    sceneItemEnum mSceneItem;
    char* mDescription;
    void* mContext;
    ListItemSceneGetValueCallback mListItemSceneGetValueCallback;
    ListItemSceneSetValueCallback mListItemSceneSetValueCallback;
    SceneOnClosingCallback mOnSceneClosingCallback;
};
