#include "listItemAction.h"

listItemAction::listItemAction(
    int id, const char* name, void* context, ListItemActionCallback listItemActionCallback)
    : listItem(id, name) {
    mContext = context;
    mListItemActionCallback = listItemActionCallback;
}

listItemAction::~listItemAction() {
}

listItemType listItemAction::getType() {
    return LISTITEM_ACTION;
}

void listItemAction::executeAction() {
    if (mListItemActionCallback == NULL) {
        return;
    }
    mListItemActionCallback(this, mContext);
}
