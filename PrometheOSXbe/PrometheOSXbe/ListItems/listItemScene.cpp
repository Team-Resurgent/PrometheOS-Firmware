#include "listItemScene.h"
#include "..\Scenes\frontLedSelectorScene.h"

listItemScene::listItemScene(int id, const char* name, sceneItemEnum sceneItem, const char* description, void* context,
    ListItemSceneGetValueCallback listItemSceneGetValueCallback,
    ListItemSceneSetValueCallback listItemSceneSetValueCallback,
    SceneOnClosingCallback onSceneClosingCallback)
    : listItem(id, name) {
    mSceneItem = sceneItem;
    mDescription = strdup(description);
    mContext = context;
    mListItemSceneGetValueCallback = listItemSceneGetValueCallback;
    mListItemSceneSetValueCallback = listItemSceneSetValueCallback;
    mOnSceneClosingCallback = onSceneClosingCallback;
}

listItemScene::~listItemScene() {
    free(mDescription);
}

listItemType listItemScene::getType() {
    return LISTITEM_SCENE;
}

const char* listItemScene::getValue() {
    if (mListItemSceneGetValueCallback == NULL) {
        return "";
    }
    return mListItemSceneGetValueCallback(this, mContext);
}

void listItemScene::setValue(const char* value) {
    if (mListItemSceneSetValueCallback == NULL) {
        return;
    }
    mListItemSceneSetValueCallback(this, mContext, value);
}

void* listItemScene::getContext() {
    return mContext;
}

void listItemScene::executeScene() {
    scene* sceneInstance = NULL;
    if (mSceneItem == sceneItemFrontLedSelectorScene) {
        sceneInstance = new frontLedSelectorScene(getValue());
    }
    if (sceneInstance == NULL) {
        return;
    }
    sceneContainer* container = new sceneContainer(mSceneItem, sceneInstance, mDescription, this, mOnSceneClosingCallback);
    if (container == NULL) {
        delete (sceneInstance);
        return;
    }
    sceneManager::pushScene(container);
}
