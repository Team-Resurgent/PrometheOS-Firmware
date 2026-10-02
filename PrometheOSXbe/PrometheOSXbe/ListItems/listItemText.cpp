#include "listItemText.h"
#include "..\stringUtility.h"
#include "..\Scenes\keyboardScene.h"
#include "..\utils.h"

static void onClosingCallback(sceneResult result, void* context, scene* caller) {
    if (result != sceneResultDone) {
        return;
    }
    listItemText* self = (listItemText*)context;
    keyboardScene* kb = (keyboardScene*)caller;
    char* text = kb->getText();
    self->setValue(text);
    free(text);
}

listItemText::listItemText(int id, const char* name, int maxLen, void* context,
    ListItemTextGetValueCallback listItemTextGetValueCallback,
    ListItemTextSetValueCallback listItemTextSetValueCallback)
    : listItem(id, name) {
    mMaxLen = maxLen;
    mContext = context;
    mListItemTextGetValueCallback = listItemTextGetValueCallback;
    mListItemTextSetValueCallback = listItemTextSetValueCallback;
}

listItemType listItemText::getType() {
    return LISTITEM_TEXT;
}

const char* listItemText::getValue() {
    if (mListItemTextGetValueCallback == NULL) {
        return "";
    }
    return mListItemTextGetValueCallback(this, mContext);
}

void listItemText::setValue(const char* value) {
    if (mListItemTextSetValueCallback == NULL) {
        return;
    }
    mListItemTextSetValueCallback(this, mContext, value);
}

void listItemText::showEditor() {
    char* label = stringUtility::formatString("%s:", getName());
    sceneManager::pushScene(new sceneContainer(sceneItemKeyboardScene,
        new keyboardScene(mMaxLen, "Please enter value...", label, getValue()), "Keyboard", this, &onClosingCallback));
    free(label);
}
