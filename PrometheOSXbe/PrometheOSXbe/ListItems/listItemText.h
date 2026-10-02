#pragma once

#include "listItem.h"
#include "..\Scenes\sceneManager.h"

class listItemText;

typedef const char* (*ListItemTextGetValueCallback)(listItemText* sender, void* context);
typedef void (*ListItemTextSetValueCallback)(listItemText* sender, void* context, const char* value);

class listItemText : public listItem {
  public:
    listItemText(int id, const char* name, int maxLen, void* context,
        ListItemTextGetValueCallback listItemTextGetValueCallback,
        ListItemTextSetValueCallback listItemTextSetValueCallback);
    ~listItemText();
    listItemType getType();
    const char* getValue();
    void setValue(const char* value);
    void showEditor();

  private:
    int mMaxLen;
    void* mContext;
    ListItemTextGetValueCallback mListItemTextGetValueCallback;
    ListItemTextSetValueCallback mListItemTextSetValueCallback;
};
