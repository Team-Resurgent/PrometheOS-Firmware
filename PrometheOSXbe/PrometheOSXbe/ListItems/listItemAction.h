#pragma once

#include "listItem.h"

class listItemAction;

typedef void (*ListItemActionCallback)(listItemAction* sender, void* context);

class listItemAction : public listItem {
  public:
    listItemAction(
        int id, const char* name, void* context, ListItemActionCallback listItemActionCallback);
    ~listItemAction();
    listItemType getType();
    void executeAction();

  private:
    void* mContext;
    ListItemActionCallback mListItemActionCallback;
};
