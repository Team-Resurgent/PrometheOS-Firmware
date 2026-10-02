#include "listItemHex.h"
#include "..\stringUtility.h"
#include "..\cerbiosIniHelper.h"
#include "..\Scenes\hexpadScene.h"
#include "..\utils.h"

static void onClosingCallback(sceneResult result, void* context, scene* caller) {
    if (result != sceneResultDone) {
        return;
    }
    listItemHex* self = (listItemHex*)context;
    hexpadScene* hex = (hexpadScene*)caller;
    char* text = hex->getText();
    if (self->getHexpadType() == hexpadTypeIGR) {
        self->setValue(cerbiosIniHelper::parseIGR(text));
    } else {
        self->setValue(stringUtility::hexToUInt(text));
    }
    free(text);
}

listItemHex::listItemHex(int id, const char* name, hexpadType hexpadType, void* context,
    ListItemHexGetValueCallback listItemHexGetValueCallback,
    ListItemHexSetValueCallback listItemHexSetValueCallback)
    : listItem(id, name) {
    mHexpadType = hexpadType;
    mContext = context;
    mListItemHexGetValueCallback = listItemHexGetValueCallback;
    mListItemHexSetValueCallback = listItemHexSetValueCallback;
}

listItemHex::~listItemHex() {
}

hexpadType listItemHex::getHexpadType() {
    return mHexpadType;
}

listItemType listItemHex::getType() {
    return LISTITEM_HEX;
}

unsigned int listItemHex::getValue() {
    if (mListItemHexGetValueCallback == NULL) {
        return 0;
    }
    return mListItemHexGetValueCallback(this, mContext);
}

void listItemHex::setValue(unsigned int value) {
    if (mListItemHexSetValueCallback == NULL) {
        return;
    }
    mListItemHexSetValueCallback(this, mContext, value);
}

void listItemHex::showEditor() {

    if (mHexpadType == hexpadTypeIGR) {
        char* igrValue = cerbiosIniHelper::shortToIGR((unsigned short)getValue());
        char* label = stringUtility::formatString("%s:", getName());
        sceneManager::pushScene(new sceneContainer(sceneItemHexPadScene,
            new hexpadScene(mHexpadType, "Please enter value...", label, igrValue), "Hex Pad", this, &onClosingCallback));
        free(label);
        free(igrValue);
    } else {
        char* hexValue = stringUtility::formatString("%06X", getValue());
        char* label = stringUtility::formatString("%s:", getName());
        sceneManager::pushScene(new sceneContainer(sceneItemHexPadScene,
            new hexpadScene(mHexpadType, "Please enter value...", label, hexValue), "Hex Pad", this, &onClosingCallback));
        free(label);
        free(hexValue);
    }
}
