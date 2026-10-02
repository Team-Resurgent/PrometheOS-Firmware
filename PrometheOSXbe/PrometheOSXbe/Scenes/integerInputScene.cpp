#include "integerInputScene.h"
#include "sceneManager.h"

#include "..\context.h"
#include "..\drawing.h"
#include "..\component.h"
#include "..\ssfn.h"
#include "..\inputManager.h"
#include "..\settingsManager.h"
#include "..\stringUtility.h"
#include "..\xboxInternals.h"
#include "..\theme.h"

integerInputScene::integerInputScene(const char* title, const char* label, unsigned int initialValue, unsigned int maxValue) {
    mSelectedControl = 0;
    mMaxValue = maxValue;
    mTitle = strdup(title);
    mLabel = strdup(label);
    mText = stringUtility::formatString("%u", initialValue);
    mCounter = 0;
    mCursorPos = (int)strlen(mText);
    mShortTextStart = 0;
    mShortTextEnd = 0;
    calcShortStringRange(mText, mShortTextStart, mShortTextEnd);
}

integerInputScene::~integerInputScene() {
    free(mTitle);
    free(mLabel);
    free(mText);
}

void integerInputScene::update() {
    // Cancel Action

    if (inputManager::buttonPressed(JoystickButtonB)) {
        sceneManager::popScene(sceneResultCancelled);
        return;
    }

    // Delete Action

    if (inputManager::buttonPressed(JoystickButtonX)) {
        char* text = deleteChar(mText);
        free(mText);
        mText = text;
        mCursorPos = (int)strlen(mText);
        calcShortStringRange(mText, mShortTextStart, mShortTextEnd);
    }

    // Select Actions

    if (inputManager::buttonPressed(JoystickButtonA)) {
        if ((mSelectedControl >= 0 && mSelectedControl <= 8) || mSelectedControl == 10) {
            uint32_t charIndex = mSelectedControl == 10 ? 9 : mSelectedControl;
            char* text = addChar(mText, charIndex);
            free(mText);
            mText = text;
            mCursorPos = (int)strlen(mText);
            calcShortStringRange(mText, mShortTextStart, mShortTextEnd);
        } else if (mSelectedControl == 9) {
            char* text = deleteChar(mText);
            free(mText);
            mText = text;
            mCursorPos = (int)strlen(mText);
            calcShortStringRange(mText, mShortTextStart, mShortTextEnd);
        } else if (mSelectedControl == 11) {
            sceneManager::popScene(sceneResultDone);
            return;
        }
    }

    // Left Actions

    if (inputManager::buttonPressed(JoystickButtonDpadLeft)) {
        if (mSelectedControl >= 0 && mSelectedControl <= 2) {
            mSelectedControl = mSelectedControl > 0 ? mSelectedControl - 1 : 2;
        } else if (mSelectedControl >= 3 && mSelectedControl <= 5) {
            mSelectedControl = mSelectedControl > 3 ? mSelectedControl - 1 : 5;
        } else if (mSelectedControl >= 6 && mSelectedControl <= 8) {
            mSelectedControl = mSelectedControl > 6 ? mSelectedControl - 1 : 8;
        } else if (mSelectedControl >= 9 && mSelectedControl <= 11) {
            mSelectedControl = mSelectedControl > 9 ? mSelectedControl - 1 : 11;
        }
    }

    // Right Actions

    if (inputManager::buttonPressed(JoystickButtonDpadRight)) {
        if (mSelectedControl >= 0 && mSelectedControl <= 2) {
            mSelectedControl = mSelectedControl < 2 ? mSelectedControl + 1 : 0;
        } else if (mSelectedControl >= 3 && mSelectedControl <= 5) {
            mSelectedControl = mSelectedControl < 5 ? mSelectedControl + 1 : 3;
        } else if (mSelectedControl >= 6 && mSelectedControl <= 8) {
            mSelectedControl = mSelectedControl < 8 ? mSelectedControl + 1 : 6;
        } else if (mSelectedControl >= 9 && mSelectedControl <= 11) {
            mSelectedControl = mSelectedControl < 11 ? mSelectedControl + 1 : 9;
        }
    }

    // Down Actions

    if (inputManager::buttonPressed(JoystickButtonDpadDown)) {
        if (mSelectedControl >= 0 && mSelectedControl <= 11) {
            mSelectedControl = mSelectedControl + 3 > 11 ? (mSelectedControl + 3) - 12 : mSelectedControl + 3;
        }
    }

    // Up Actions

    if (inputManager::buttonPressed(JoystickButtonDpadUp)) {
        if (mSelectedControl >= 0 && mSelectedControl <= 11) {
            mSelectedControl = mSelectedControl - 3 < 0 ? (mSelectedControl - 3) + 12 : mSelectedControl - 3;
        }
    }
}

void integerInputScene::render() {
    if (mCounter == 0) {
        mCounter = 60;
    } else {
        mCounter--;
    }

    component::panel(theme::getPanelFillColor(), theme::getPanelStrokeColor(), 16, 16, 688, 448);
    drawing::drawBitmapStringAligned(context::getBitmapFontMedium(), mTitle, theme::getHeaderTextColor(),
        theme::getHeaderAlign(), 40, theme::getHeaderY(), 640);

    char* keys = "1234567890";

    int32_t keyboardX = 264;
    int32_t keyboardY = (context::getBufferHeight() - (((4 * 48) - 4) + 64)) / 2;
    keyboardY += theme::getCenterOffset();

    // Display the input value with cursor
    char* displayText;
    if (mShortTextEnd > mShortTextStart) {
        char* shortText = stringUtility::substr(mText, mShortTextStart, mShortTextEnd - mShortTextStart);
        displayText = mCounter < 30 ? stringUtility::formatString("%s\xC2\xAF", shortText)
                                    : stringUtility::formatString("%s\xC2\xB0", shortText);
        free(shortText);
    } else {
        displayText = mCounter < 30 ? stringUtility::formatString("%s\xC2\xAF", mText)
                                    : stringUtility::formatString("%s\xC2\xB0", mText);
    }
    
    component::textBox(displayText, false, false, horizAlignmentCenter, 225, keyboardY, 280, 44, false, false, false);
    free(displayText);

    // Draw label if provided
    if (mLabel != NULL && strlen(mLabel) > 0) {
        int labelY = keyboardY - 30;
        component::text(mLabel, false, horizAlignmentCenter, 225, labelY, 280, 30);
    }

    keyboardY += 64;

    keyboardButton(mSelectedControl == 0, false, keys[0], keyboardX + (66 * 0), keyboardY + (48 * 0), 62, 44);
    keyboardButton(mSelectedControl == 1, false, keys[1], keyboardX + (66 * 1), keyboardY + (48 * 0), 62, 44);
    keyboardButton(mSelectedControl == 2, false, keys[2], keyboardX + (66 * 2), keyboardY + (48 * 0), 62, 44);

    keyboardButton(mSelectedControl == 3, false, keys[3], keyboardX + (66 * 0), keyboardY + (48 * 1), 62, 44);
    keyboardButton(mSelectedControl == 4, false, keys[4], keyboardX + (66 * 1), keyboardY + (48 * 1), 62, 44);
    keyboardButton(mSelectedControl == 5, false, keys[5], keyboardX + (66 * 2), keyboardY + (48 * 1), 62, 44);

    keyboardButton(mSelectedControl == 6, false, keys[6], keyboardX + (66 * 0), keyboardY + (48 * 2), 62, 44);
    keyboardButton(mSelectedControl == 7, false, keys[7], keyboardX + (66 * 1), keyboardY + (48 * 2), 62, 44);
    keyboardButton(mSelectedControl == 8, false, keys[8], keyboardX + (66 * 2), keyboardY + (48 * 2), 62, 44);

    component::button(mSelectedControl == 9, false, "Del", keyboardX + (66 * 0), keyboardY + (48 * 3), 62, 44);
    keyboardButton(mSelectedControl == 10, false, keys[9], keyboardX + (66 * 1), keyboardY + (48 * 3), 62, 44);
    component::button(mSelectedControl == 11, false, "Done", keyboardX + (66 * 2), keyboardY + (48 * 3), 62, 44);

    drawing::drawBitmapString(context::getBitmapFontSmall(), "\xC2\xA1 Select \xC2\xA3 Delete",
        theme::getFooterTextColor(), 40, theme::getFooterY());
    drawing::drawBitmapStringAligned(context::getBitmapFontSmall(), "\xC2\xA2 Cancel", theme::getFooterTextColor(),
        horizAlignmentRight, 40, theme::getFooterY(), 640);
}

char* integerInputScene::addChar(char* value, int charIndex) {
    const char* keys = "1234567890";
    char* temp = strcmp(value, "0") == 0 ? stringUtility::formatString("%c", keys[charIndex])
                                         : stringUtility::formatString("%s%c", value, keys[charIndex]);

    unsigned __int64 newValue = 0;
    for (int i = 0; temp[i] != 0; i++) {
        newValue = (newValue * 10) + (unsigned __int64)(temp[i] - '0');
        if (newValue > (unsigned __int64)mMaxValue) {
            free(temp);
            return strdup(value);
        }
    }

    char* result = strdup(temp);
    free(temp);
    return result;
}

char* integerInputScene::deleteChar(char* value) {
    return stringUtility::substr(value, 0, max(strlen(value) - 1, 0));
}

void integerInputScene::keyboardButton(bool selected, bool active, char key, int x, int y, int width, int height) {
    char* keyString = stringUtility::formatString("%c", key);
    component::button(selected, active, keyString, x, y, width, height);
    free(keyString);
}

unsigned int integerInputScene::getValue() {
    unsigned int result = 0;
    for (int i = 0; mText[i] != 0; i++) {
        result = (result * 10) + (unsigned int)(mText[i] - '0');
    }
    return result;
}

char* integerInputScene::getText() {
    return strdup(mText);
}

void integerInputScene::clearText() {
    free(mText);
    mText = strdup("");
    mCursorPos = 0;
    calcShortStringRange(mText, mShortTextStart, mShortTextEnd);
}

void integerInputScene::calcShortStringRange(const char* value, int& startPos, int& endPos) {
    int tw, th;
    drawing::measureBitmapString(context::getBitmapFontSmall(), value, &tw, &th);
    int maxW = 280 - 20;
    
    if (tw <= maxW) {
        startPos = 0;
        endPos = (int)strlen(value);
        return;
    }

    startPos = 0;
    endPos = (int)strlen(value);
    
    for (int i = 0; i < (int)strlen(value); i++) {
        char* temp = stringUtility::substr(value, i, (int)strlen(value) - i);
        drawing::measureBitmapString(context::getBitmapFontSmall(), temp, &tw, &th);
        free(temp);
        if (tw < maxW) {
            startPos = i;
            break;
        }
    }
    
    for (int i = startPos; i < (int)strlen(value); i++) {
        char* temp = stringUtility::substr(value, startPos, i - startPos + 1);
        drawing::measureBitmapString(context::getBitmapFontSmall(), temp, &tw, &th);
        free(temp);
        if (tw >= maxW) {
            endPos = i;
            break;
        }
    }
}

