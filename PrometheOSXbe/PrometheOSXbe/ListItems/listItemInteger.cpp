#include "listItemInteger.h"
#include "..\stringUtility.h"
#include "..\Scenes\integerInputScene.h"
#include "..\Scenes\durationSelectorScene.h"
#include "..\utils.h"

static void onClosingCallback(sceneResult result, void* context, scene* caller) {
    if (result != sceneResultDone) {
        return;
    }
    listItemInteger* self = (listItemInteger*)context;
    integerInputScene* input = (integerInputScene*)caller;
    unsigned int value = input->getValue();
    self->setValue(value);
}

static void onDurationClosingCallback(sceneResult result, void* context, scene* caller) {
    if (result != sceneResultDone) {
        return;
    }
    listItemInteger* self = (listItemInteger*)context;
    durationSelectorScene* input = (durationSelectorScene*)caller;
    self->setValue(input->getTotalSeconds());
}

listItemInteger::listItemInteger(int id, const char* name, unsigned int maxValue, void* context,
    ListItemIntegerGetValueCallback listItemIntegerGetValueCallback,
    ListItemIntegerSetValueCallback listItemIntegerSetValueCallback,
    bool isDuration, unsigned int minValue, const char* zeroLabel)
    : listItem(id, name) {
    mMaxValue = maxValue;
    mMinValue = minValue;
    mIsDuration = isDuration;
    mZeroLabel = zeroLabel != NULL ? strdup(zeroLabel) : NULL;
    mContext = context;
    mListItemIntegerGetValueCallback = listItemIntegerGetValueCallback;
    mListItemIntegerSetValueCallback = listItemIntegerSetValueCallback;
}

listItemInteger::~listItemInteger() {
    free(mZeroLabel);
}

listItemType listItemInteger::getType() {
    return LISTITEM_INTEGER;
}

unsigned int listItemInteger::getValue() {
    if (mListItemIntegerGetValueCallback == NULL) {
        return 0;
    }
    return mListItemIntegerGetValueCallback(this, mContext);
}

void listItemInteger::setValue(unsigned int value) {
    if (mListItemIntegerSetValueCallback == NULL) {
        return;
    }
    mListItemIntegerSetValueCallback(this, mContext, value);
}

unsigned int listItemInteger::getMaxValue() {
    return mMaxValue;
}

char* listItemInteger::getDisplayValue() {
    unsigned int value = getValue();

    if (value == 0 && mZeroLabel != NULL && strlen(mZeroLabel) > 0) {
        return strdup(mZeroLabel);
    }

    if (mIsDuration == false) {
        return stringUtility::formatString("%u", value);
    }

    unsigned int days = value / 86400;
    unsigned int hours = (value % 86400) / 3600;
    unsigned int minutes = (value % 3600) / 60;
    unsigned int seconds = value % 60;

    if (days > 0) {
        return stringUtility::formatString("%ud %uh %um %us", days, hours, minutes, seconds);
    }
    if (hours > 0) {
        return stringUtility::formatString("%uh %um %us", hours, minutes, seconds);
    }
    if (minutes > 0) {
        return stringUtility::formatString("%um %us", minutes, seconds);
    }
    return stringUtility::formatString("%us", seconds);
}

void listItemInteger::showEditor() {
    char* label = stringUtility::formatString("%s", getName());

    if (mIsDuration) {
        sceneManager::pushScene(new sceneContainer(sceneItemGenericScene,
            new durationSelectorScene("Please set duration...", label, getValue(), mMinValue, mMaxValue, mZeroLabel),
            "Duration Selector", this, &onDurationClosingCallback));
    } else {
        sceneManager::pushScene(new sceneContainer(sceneItemGenericScene,
            new integerInputScene("Please enter value...", label, getValue(), mMaxValue), "Integer Input", this, &onClosingCallback));
    }

    free(label);
}

