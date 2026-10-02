#include "listItemBool.h"

listItemBool::listItemBool(int id, const char* name, void* context,
        ListItemBoolGetValueCallback listItemBoolGetValueCallback,
        ListItemBoolSetValueCallback listItemBoolSetValueCallback) : listItem(id, name) {
    mContext = context;
    mListItemBoolGetValueCallback = listItemBoolGetValueCallback;
    mListItemBoolSetValueCallback = listItemBoolSetValueCallback;
}

listItemBool::~listItemBool() {
}

listItemType listItemBool::getType() {
    return LISTITEM_BOOL;
}

unsigned char listItemBool::getValue() {
    if (mListItemBoolGetValueCallback == NULL) {
        return 0;
    }
    return mListItemBoolGetValueCallback(this, mContext);
}

const char* listItemBool::getValueString() {
    return getValue() == 0 ? "False" : "True";
}

void listItemBool::toggleValue() {
    if (mListItemBoolSetValueCallback == NULL) {
        return;
    }
    mListItemBoolSetValueCallback(this, mContext, getValue() == 0 ? 1 : 0);
}
