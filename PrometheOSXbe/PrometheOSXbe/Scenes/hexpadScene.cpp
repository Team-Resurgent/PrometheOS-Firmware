#include "hexpadScene.h"
#include "sceneManager.h"
#include "mainScene.h"

#include "..\context.h"
#include "..\drawing.h"
#include "..\component.h"
#include "..\ssfn.h"
#include "..\inputManager.h"
#include "..\stringUtility.h"
#include "..\xboxInternals.h"
#include "..\theme.h"

hexpadScene::hexpadScene(hexpadType hexpadType, const char* title, const char* label, const char* text) {
    mSelectedControl = 17;
    mHexpadType = hexpadType;
    mMaxLen = hexpadType == hexpadTypeIGR ? 4 : 6;
    mTitle = strdup(title);
    mLabel = strdup(label);
    mText = strdup(text);
    mCounter = 0;
    mCursorPos = strlen(mText);
    calcShortStringRange(mText, mShortTextStart, mShortTextEnd);
}

hexpadScene::~hexpadScene() {
    free(mTitle);
    free(mLabel);
    free(mText);
}

void hexpadScene::update() {
    // Cancel Action

    if (inputManager::buttonPressed(JoystickButtonB)) {
        sceneManager::popScene(sceneResultCancelled);
        return;
    }

    if (inputManager::buttonPressed(JoystickButtonY)) {
        clearText();
        return;
    }

    // Delete Action

    if (inputManager::buttonPressed(JoystickButtonX)) {
        if (mCursorPos > 0) {
            char* newText = stringUtility::removeAtIndex(mText, mCursorPos);
            free(mText);
            mText = newText;
            mCursorPos--;
            calcShortStringRange(mText, mShortTextStart, mShortTextEnd);
        }
    }

    // Select Actions

    if (inputManager::buttonPressed(JoystickButtonA)) {
        char* keys = "0123456789ABCDEF";

        if (mSelectedControl >= 0 && mSelectedControl <= 14) {
            int maxLen = mHexpadType == hexpadTypeIGR ? 4 : mMaxLen;
            if ((int)strlen(mText) < maxLen) {
                char* newText = stringUtility::insertAtIndex(mText, keys[mSelectedControl], mCursorPos);
                free(mText);
                mText = newText;
                mCursorPos++;
                calcShortStringRange(mText, mShortTextStart, mShortTextEnd);
            }
        } else if (mSelectedControl == 15) {
            if (mCursorPos > 0) {
                char* newText = stringUtility::removeAtIndex(mText, mCursorPos);
                free(mText);
                mText = newText;
                mCursorPos--;
                calcShortStringRange(mText, mShortTextStart, mShortTextEnd);
            }
        } else if (mSelectedControl == 16) {
            if ((int)strlen(mText) < mMaxLen) {
                char* newText = stringUtility::insertAtIndex(mText, keys[mSelectedControl - 1], mCursorPos);
                free(mText);
                mText = newText;
                mCursorPos++;
                calcShortStringRange(mText, mShortTextStart, mShortTextEnd);
            }
        } else if (mSelectedControl == 17) {
            sceneManager::popScene(sceneResultDone);
            return;
        }
    }

    // Left Actions

    if (inputManager::buttonPressed(JoystickButtonDpadLeft)) {
        if (mSelectedControl >= 0 && mSelectedControl <= 4) {
            mSelectedControl = mSelectedControl > 0 ? mSelectedControl - 1 : 4;
        } else if (mSelectedControl >= 5 && mSelectedControl <= 9) {
            mSelectedControl = mSelectedControl > 5 ? mSelectedControl - 1 : 9;
        } else if (mSelectedControl >= 10 && mSelectedControl <= 14) {
            mSelectedControl = mSelectedControl > 10 ? mSelectedControl - 1 : 14;
        } else if (mSelectedControl >= 15 && mSelectedControl <= 17) {
            mSelectedControl = mSelectedControl > 15 ? mSelectedControl - 1 : 17;
        }
    }

    // Right Actions

    if (inputManager::buttonPressed(JoystickButtonDpadRight)) {
        if (mSelectedControl >= 0 && mSelectedControl <= 4) {
            mSelectedControl = mSelectedControl < 4 ? mSelectedControl + 1 : 0;
        } else if (mSelectedControl >= 5 && mSelectedControl <= 9) {
            mSelectedControl = mSelectedControl < 9 ? mSelectedControl + 1 : 5;
        } else if (mSelectedControl >= 10 && mSelectedControl <= 14) {
            mSelectedControl = mSelectedControl < 14 ? mSelectedControl + 1 : 10;
        } else if (mSelectedControl >= 15 && mSelectedControl <= 17) {
            mSelectedControl = mSelectedControl < 17 ? mSelectedControl + 1 : 15;
        }
    }

    // Down Actions

    if (inputManager::buttonPressed(JoystickButtonDpadDown)) {
        if (mSelectedControl >= 0 && mSelectedControl <= 9) {
            mSelectedControl = mSelectedControl + 5;
        } else if (mSelectedControl >= 10 && mSelectedControl <= 11) {
            mSelectedControl = 15;
        } else if (mSelectedControl == 12) {
            mSelectedControl = 16;
        } else if (mSelectedControl >= 13 && mSelectedControl <= 14) {
            mSelectedControl = 17;
        } else if (mSelectedControl == 15) {
            mSelectedControl = 0;
        } else if (mSelectedControl == 16) {
            mSelectedControl = 2;
        } else if (mSelectedControl == 17) {
            mSelectedControl = 4;
        }
    }

    // Up Actions

    if (inputManager::buttonPressed(JoystickButtonDpadUp)) {
        if (mSelectedControl >= 0 && mSelectedControl <= 1) {
            mSelectedControl = 15;
        } else if (mSelectedControl == 2) {
            mSelectedControl = 16;
        } else if (mSelectedControl >= 3 && mSelectedControl <= 4) {
            mSelectedControl = 17;
        } else if (mSelectedControl >= 5 && mSelectedControl <= 14) {
            mSelectedControl = mSelectedControl - 5;
        } else if (mSelectedControl == 15) {
            mSelectedControl = 10;
        } else if (mSelectedControl == 16) {
            mSelectedControl = 12;
        } else if (mSelectedControl == 17) {
            mSelectedControl = 14;
        }
    }

    // Left Trigger

    if (inputManager::buttonPressed(JoystickButtonLeftTrigger)) {
        mCursorPos = mCursorPos > 0 ? mCursorPos - 1 : 0;
        calcShortStringRange(mText, mShortTextStart, mShortTextEnd);
    }

    // Right Trigger

    if (inputManager::buttonPressed(JoystickButtonRightTrigger)) {
        mCursorPos = mCursorPos < (int)strlen(mText) ? mCursorPos + 1 : strlen(mText);
        calcShortStringRange(mText, mShortTextStart, mShortTextEnd);
    }
}

void hexpadScene::render() {
    if (mCounter == 0) {
        mCounter = 60;
    } else {
        mCounter--;
    }

    component::panel(theme::getPanelFillColor(), theme::getPanelStrokeColor(), 16, 16, 688, 448);
    drawing::drawBitmapStringAligned(context::getBitmapFontMedium(), mTitle, theme::getHeaderTextColor(),
        theme::getHeaderAlign(), 40, theme::getHeaderY(), 640);

    char* keys = "0123456789ABCDEF";

    int32_t keyboardX = 227;
    int32_t keyboardY = (context::getBufferHeight() - (((4 * 48) - 4) + 30 + 64)) / 2;
    keyboardY += theme::getCenterOffset();

    if (mHexpadType == hexpadTypeIGR) {
        const char* legendLines[] = {"A=0, B=1, X=2, Y=3, BLACK=4, WHITE=5, L_TRIG=6, R_TRIG=7",
            "UP=8, DOWN=9, LEFT=A, RIGHT=B, START=C, BACK=D, L_THUMB=E, R_THUMB=F"};

        int legendY = theme::getHeaderY() + 40;
        for (int i = 0; i < 2; i++) {
            drawing::drawBitmapString(
                context::getBitmapFontExtraSmall(), legendLines[i], theme::getTextColor(), 40, legendY + (i * 18));
        }
    } else {
        drawing::drawBitmapString(context::getBitmapFontSmall(), mLabel, theme::getTextColor(), 40, keyboardY);
    }

    keyboardY += 30;

    int t = strlen(mText);
    int cursorEnd1 = mCursorPos - mShortTextStart;
    int cursorEnd2 = max(mShortTextEnd - mCursorPos, 0);

    char* leftString = stringUtility::substr(mText, mShortTextStart, mCursorPos - mShortTextStart);
    int q = strlen(leftString);
    char* rightString = stringUtility::substr(mText, mCursorPos, max(mShortTextEnd - mCursorPos, 0));
    int v = strlen(rightString);
    char* shortText = mCounter < 30 ? stringUtility::formatString("%s\xC2\xAF%s", leftString, rightString)
                                    : stringUtility::formatString("%s\xC2\xB0%s", leftString, rightString);
    
    // Color preview box for color mode
    int textBoxWidth = 640;
    int colorBoxSize = 44;
    int colorBoxSpacing = 8;
    
    if (mHexpadType == hexpadTypeColor) {
        // Adjust text box width to make room for color preview
        textBoxWidth = 640 - colorBoxSize - colorBoxSpacing;
        
        // Parse hex color value and draw preview
        uint32_t colorValue = 0;
        if (strlen(mText) > 0) {
            colorValue = stringUtility::hexToUInt(mText);
        }
        
        // Ensure color has alpha channel
        uint32_t colorWithAlpha = colorValue | 0xff000000;
        
        // Draw color preview box (right side, aligned with text box)
        int colorBoxX = 40 + textBoxWidth + colorBoxSpacing;
        int colorBoxY = keyboardY;
        component::panel(colorWithAlpha, 0xff000000, colorBoxX, colorBoxY, colorBoxSize, colorBoxSize);
    }
    
    component::textBox(shortText, false, false, horizAlignmentLeft, 40, keyboardY, textBoxWidth, 44, false, false, false);
    free(shortText);
    free(leftString);
    free(rightString);

    keyboardY += 64;

    keyboardButton(mSelectedControl == 0, false, keys[0], keyboardX + (54 * 0), keyboardY + (48 * 0), 50, 44);
    keyboardButton(mSelectedControl == 1, false, keys[1], keyboardX + (54 * 1), keyboardY + (48 * 0), 50, 44);
    keyboardButton(mSelectedControl == 2, false, keys[2], keyboardX + (54 * 2), keyboardY + (48 * 0), 50, 44);
    keyboardButton(mSelectedControl == 3, false, keys[3], keyboardX + (54 * 3), keyboardY + (48 * 0), 50, 44);
    keyboardButton(mSelectedControl == 4, false, keys[4], keyboardX + (54 * 4), keyboardY + (48 * 0), 50, 44);

    keyboardButton(mSelectedControl == 5, false, keys[5], keyboardX + (54 * 0), keyboardY + (48 * 1), 50, 44);
    keyboardButton(mSelectedControl == 6, false, keys[6], keyboardX + (54 * 1), keyboardY + (48 * 1), 50, 44);
    keyboardButton(mSelectedControl == 7, false, keys[7], keyboardX + (54 * 2), keyboardY + (48 * 1), 50, 44);
    keyboardButton(mSelectedControl == 8, false, keys[8], keyboardX + (54 * 3), keyboardY + (48 * 1), 50, 44);
    keyboardButton(mSelectedControl == 9, false, keys[9], keyboardX + (54 * 4), keyboardY + (48 * 1), 50, 44);

    keyboardButton(mSelectedControl == 10, false, keys[10], keyboardX + (54 * 0), keyboardY + (48 * 2), 50, 44);
    keyboardButton(mSelectedControl == 11, false, keys[11], keyboardX + (54 * 1), keyboardY + (48 * 2), 50, 44);
    keyboardButton(mSelectedControl == 12, false, keys[12], keyboardX + (54 * 2), keyboardY + (48 * 2), 50, 44);
    keyboardButton(mSelectedControl == 13, false, keys[13], keyboardX + (54 * 3), keyboardY + (48 * 2), 50, 44);
    keyboardButton(mSelectedControl == 14, false, keys[14], keyboardX + (54 * 4), keyboardY + (48 * 2), 50, 44);

    component::button(mSelectedControl == 15, false, "Delete", keyboardX, keyboardY + (48 * 3), 104, 44);
    keyboardButton(mSelectedControl == 16, false, keys[15], keyboardX + (54 * 2), keyboardY + (48 * 3), 50, 44);
    component::button(mSelectedControl == 17, false, "Done", keyboardX + (54 * 3), keyboardY + (48 * 3), 104, 44);

    drawing::drawBitmapString(context::getBitmapFontSmall(), "\xC2\xA1 Select, \xC2\xA3 Delete, \xC2\xA4 Clear",
        theme::getFooterTextColor(), 40, theme::getFooterY());
    drawing::drawBitmapStringAligned(context::getBitmapFontSmall(), "\xC2\xA2 Cancel", theme::getFooterTextColor(),
        horizAlignmentRight, 40, theme::getFooterY(), 640);
}

void hexpadScene::keyboardButton(bool selected, bool active, char key, int x, int y, int width, int height) {
    char* keyString = stringUtility::formatString("%c", key);
    component::button(selected, active, keyString, x, y, width, height);
    free(keyString);
}

char* hexpadScene::getText() {
    return strdup(mText);
}

void hexpadScene::clearText() {
    free(mText);
    mText = strdup("");
    mCursorPos = 0;
}

void hexpadScene::calcShortStringRange(const char* value, int& startPos, int& endPos) {
    int textWidth;
    int textHeight;

    int maxWidth = 640 - 30;

    startPos = mCursorPos;
    while (startPos >= 0) {
        char* leftString = stringUtility::substr(value, startPos, mCursorPos - startPos);
        drawing::measureBitmapString(context::getBitmapFontSmall(), leftString, &textWidth, &textHeight);
        free(leftString);
        if (textWidth > maxWidth) {
            break;
        }
        startPos--;
    }
    startPos++;

    endPos = startPos;
    while (endPos <= (int)strlen(value)) {
        char* rightString = stringUtility::substr(value, startPos, endPos - startPos);
        drawing::measureBitmapString(context::getBitmapFontSmall(), rightString, &textWidth, &textHeight);
        free(rightString);
        if (textWidth > maxWidth) {
            break;
        }
        endPos++;
    }
    endPos--;
}
