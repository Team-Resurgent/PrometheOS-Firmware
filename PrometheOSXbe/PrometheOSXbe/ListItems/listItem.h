#pragma once

#include <xtl.h>

enum listItemType {
    LISTITEM_TEXT,
    LISTITEM_ENUM,
    LISTITEM_TITLE,
    LISTITEM_ACTION,
    LISTITEM_SCENE,
    LISTITEM_BOOL,
    LISTITEM_HEX,
    LISTITEM_PATH,
    LISTITEM_INTEGER
};

class listItem {
  public:
    listItem(int id, const char* name);
    ~listItem();
    virtual listItemType getType() = 0;
    int getId();
    const char* getName();

  private:
    int mId;
    char* mName;
};
