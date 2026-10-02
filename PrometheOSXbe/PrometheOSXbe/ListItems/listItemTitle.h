#pragma once

#include "listItem.h"

class listItemTitle : public listItem {
  public:
    listItemTitle(int id, const char* name);
    listItemType getType();
};
