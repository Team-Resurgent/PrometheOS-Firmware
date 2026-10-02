#pragma once

#include "listItem.h"
#include "..\Scenes\sceneManager.h"
#include "..\Scenes\hexpadScene.h"

class listItemHex;

typedef unsigned int (*ListItemHexGetValueCallback)(listItemHex* sender, void* context);
typedef void (*ListItemHexSetValueCallback)(listItemHex* sender, void* context, unsigned int value);

class listItemHex : public listItem {
  public:
    listItemHex(int id, const char* name, hexpadType hexpadType, void* context,
        ListItemHexGetValueCallback listItemHexGetValueCallback,
        ListItemHexSetValueCallback listItemHexSetValueCallback);
    ~listItemHex();
    hexpadType getHexpadType();
    listItemType getType();
    unsigned int getValue();
    void setValue(unsigned int value);
    void showEditor();

  private:
    hexpadType mHexpadType;
    void* mContext;
    ListItemHexGetValueCallback mListItemHexGetValueCallback;
    ListItemHexSetValueCallback mListItemHexSetValueCallback;
};
