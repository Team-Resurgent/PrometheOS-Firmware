#pragma once

#include "scene.h"

#include "..\xboxInternals.h"

class integerInputScene : public scene {
  public:
    integerInputScene(const char* title, const char* label, unsigned int initialValue, unsigned int maxValue);
    ~integerInputScene();
    void update();
    void render();
    unsigned int getValue();
    char* getText();
    void clearText();
    void calcShortStringRange(const char* value, int& startPos, int& endPos);

  private:
    void keyboardButton(bool selected, bool active, char key, int x, int y, int width, int height);
    char* addChar(char* value, int charIndex);
    char* deleteChar(char* value);

  private:
    int mSelectedControl;
    unsigned int mMaxValue;
    char* mTitle;
    char* mLabel;
    char* mText;
    int mCounter;
    int mCursorPos;
    int mShortTextStart;
    int mShortTextEnd;
};

