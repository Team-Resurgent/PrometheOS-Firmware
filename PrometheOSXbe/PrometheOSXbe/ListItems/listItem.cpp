#include "listItem.h"

listItem::listItem(int id, const char* name) {
    mId = id;
    mName = strdup(name);
}

listItem::~listItem() {
    free(mName);
}

int listItem::getId() {
    return mId;
}

const char* listItem::getName() {
    return mName;
}
