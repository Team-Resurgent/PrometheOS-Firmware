#pragma once

#include "listItem.h"

class listItemBool;

typedef unsigned char (*ListItemBoolGetValueCallback)(listItemBool* sender, void* context);
typedef void (*ListItemBoolSetValueCallback)(listItemBool* sender, void* context, unsigned char value);

class listItemBool : public listItem {
  public:
    listItemBool(int id, const char* name, void* context,
        ListItemBoolGetValueCallback listItemBoolGetValueCallback,
        ListItemBoolSetValueCallback listItemBoolSetValueCallback);
    ~listItemBool();
    listItemType getType();
    unsigned char getValue();
    const char* getValueString();
    void toggleValue();
  private:
    void* mContext;
    ListItemBoolGetValueCallback mListItemBoolGetValueCallback;
    ListItemBoolSetValueCallback mListItemBoolSetValueCallback;
};
