#pragma once

#include "listItem.h"
#include "..\pointerVector.h"

class listItemEnum;

typedef unsigned int (*ListItemEnumGetValueCallback)(listItemEnum* sender, void* context);
typedef void (*ListItemEnumSetValueCallback)(listItemEnum* sender, void* context, unsigned int value);

class listItemEnum : public listItem {
  public:
    listItemEnum(int id, const char* name, void* context,
    ListItemEnumGetValueCallback listItemEnumGetValueCallback,
    ListItemEnumSetValueCallback listItemEnumSetValueCallback);
    ~listItemEnum();
    listItemType getType();
    unsigned int getCount();
    unsigned int getValue();
    void setValue(unsigned int value);
    const char* getValueString();
    void addOption(const char* option);
    pointerVector<char*>* getOptions();

  private:
    pointerVector<char*>* mOptions;
    void* mContext;
    ListItemEnumGetValueCallback mListItemEnumGetValueCallback;
    ListItemEnumSetValueCallback mListItemEnumSetValueCallback;
};
