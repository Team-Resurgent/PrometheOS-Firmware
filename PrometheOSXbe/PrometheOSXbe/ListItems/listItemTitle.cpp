#include "listItemTitle.h"

listItemTitle::listItemTitle(int id, const char* name) : listItem(id, name) {
}

listItemType listItemTitle::getType() {
    return LISTITEM_TITLE;
}
