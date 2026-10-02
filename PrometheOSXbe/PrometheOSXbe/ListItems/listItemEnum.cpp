#include "listItemEnum.h"

listItemEnum::listItemEnum(int id, const char* name, void* context,
    ListItemEnumGetValueCallback listItemEnumGetValueCallback,
    ListItemEnumSetValueCallback listItemEnumSetValueCallback) : listItem(id, name) {
    mOptions = new pointerVector<char*>(false);
    mContext = context;
    mListItemEnumGetValueCallback = listItemEnumGetValueCallback;
    mListItemEnumSetValueCallback = listItemEnumSetValueCallback;
}

listItemEnum::~listItemEnum() {
    delete mOptions;
}

listItemType listItemEnum::getType() {
    return LISTITEM_ENUM;
}

unsigned int listItemEnum::getCount()
{
    return mOptions->count();
}

unsigned int listItemEnum::getValue()
{
    if (mListItemEnumGetValueCallback == NULL) {
        return 0;
    }
    return mListItemEnumGetValueCallback(this, mContext);
}

void listItemEnum::setValue(unsigned int value)
{
    if (mListItemEnumSetValueCallback == NULL) {
        return;
    }
    mListItemEnumSetValueCallback(this, mContext, value);
}

const char* listItemEnum::getValueString() {
    return mOptions->get(getValue());
}

void listItemEnum::addOption(const char* option) {
    if (mOptions != NULL && option != NULL) {
        mOptions->add(strdup(option));
    }
}
