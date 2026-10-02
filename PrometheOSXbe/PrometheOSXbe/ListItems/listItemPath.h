#pragma once

#include "listItem.h"
#include "..\Scenes\sceneManager.h"

class listItemPath;

typedef const char* (*ListItemPathGetValueCallback)(listItemPath* sender, void* context);
typedef void (*ListItemPathSetValueCallback)(listItemPath* sender, void* context, const char* value);

class listItemPath : public listItem {
  public:
    listItemPath(int id, const char* name, int maxLen, void* context,
        ListItemPathGetValueCallback listItemPathGetValueCallback,
        ListItemPathSetValueCallback listItemPathSetValueCallback);
    ~listItemPath();
    listItemType getType();
    const char* getValue();
    void setValue(const char* value);
    void showEditor();
    void showFilePicker();

  private:
    int mMaxLen;
    void* mContext;
    ListItemPathGetValueCallback mListItemPathGetValueCallback;
    ListItemPathSetValueCallback mListItemPathSetValueCallback;
};
