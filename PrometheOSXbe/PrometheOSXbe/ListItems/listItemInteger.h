#pragma once

#include "listItem.h"
#include "..\Scenes\sceneManager.h"

class listItemInteger;

typedef unsigned int (*ListItemIntegerGetValueCallback)(listItemInteger* sender, void* context);
typedef void (*ListItemIntegerSetValueCallback)(listItemInteger* sender, void* context, unsigned int value);

class listItemInteger : public listItem {
  public:
    listItemInteger(int id, const char* name, unsigned int maxValue, void* context,
        ListItemIntegerGetValueCallback listItemIntegerGetValueCallback,
        ListItemIntegerSetValueCallback listItemIntegerSetValueCallback,
        bool isDuration = false, unsigned int minValue = 0, const char* zeroLabel = NULL);
    ~listItemInteger();
    listItemType getType();
    unsigned int getValue();
    void setValue(unsigned int value);
    unsigned int getMaxValue();
    char* getDisplayValue();
    void showEditor();

  private:
    unsigned int mMaxValue;
    unsigned int mMinValue;
    bool mIsDuration;
    char* mZeroLabel;
    void* mContext;
    ListItemIntegerGetValueCallback mListItemIntegerGetValueCallback;
    ListItemIntegerSetValueCallback mListItemIntegerSetValueCallback;
};

