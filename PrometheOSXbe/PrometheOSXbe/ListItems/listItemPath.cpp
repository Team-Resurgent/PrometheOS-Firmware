#include "listItemPath.h"
#include "..\stringUtility.h"
#include "..\Scenes\keyboardScene.h"
#include "..\Scenes\filePickerScene.h"
#include "..\utils.h"

static void onKeyboardClosingCallback(sceneResult result, void* context, scene* caller) {
    if (result != sceneResultDone) {
        return;
    }
    listItemPath* self = (listItemPath*)context;
    keyboardScene* kb = (keyboardScene*)caller;
    char* text = kb->getText();
    self->setValue(text);
    free(text);
}

static void onFilePickerClosingCallback(sceneResult result, void* context, scene* caller) {
    if (result != sceneResultDone) {
        return;
    }
    listItemPath* self = (listItemPath*)context;
    filePickerScene* fp = (filePickerScene*)caller;
    char* filePath = fp->getFilePath();
    self->setValue(filePath);
    free(filePath);
}

listItemPath::listItemPath(int id, const char* name, int maxLen, void* context,
    ListItemPathGetValueCallback listItemPathGetValueCallback,
    ListItemPathSetValueCallback listItemPathSetValueCallback)
    : listItem(id, name) {
    mMaxLen = maxLen;
    mContext = context;
    mListItemPathGetValueCallback = listItemPathGetValueCallback;
    mListItemPathSetValueCallback = listItemPathSetValueCallback;
}

listItemPath::~listItemPath() {
}

listItemType listItemPath::getType() {
    return LISTITEM_PATH;
}

const char* listItemPath::getValue() {
    if (mListItemPathGetValueCallback == NULL) {
        return "";
    }
    return mListItemPathGetValueCallback(this, mContext);
}

void listItemPath::setValue(const char* value) {
    if (mListItemPathSetValueCallback == NULL) {
        return;
    }
    mListItemPathSetValueCallback(this, mContext, value);
}

void listItemPath::showEditor() {
    char* label = stringUtility::formatString("%s:", getName());
    sceneManager::pushScene(new sceneContainer(sceneItemKeyboardScene,
        new keyboardScene(mMaxLen, "Please enter value...", label, getValue()), "Keyboard", this, &onKeyboardClosingCallback));
    free(label);
}

void listItemPath::showFilePicker() {
    sceneManager::pushScene(new sceneContainer(sceneItemGenericScene,
        new filePickerScene(filePickerTypeXbe, false, true), "File Picker", this, &onFilePickerClosingCallback));
}
