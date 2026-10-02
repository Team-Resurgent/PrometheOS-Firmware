#pragma once

#include "scene.h"

#include "..\xboxInternals.h"

enum hexpadType {
    hexpadTypeCoeff = 0,
    hexpadTypeIGR = 1,
    hexpadTypeColor = 2
};

class hexpadScene : public scene {
  public:
    hexpadScene(hexpadType hexpadType, const char* title, const char* label, const char* text);
    ~hexpadScene();
    void update();
    void render();
    char* getText();
    void clearText();
    void calcShortStringRange(const char* value, int& startPos, int& endPos);

  private:
    void keyboardButton(bool selected, bool active, char key, int x, int y, int width, int height);

  private:
    int mSelectedControl;
    uint32_t mMaxLen;
    char* mTitle;
    char* mLabel;
    char* mText;
    int mCounter;
    int mCursorPos;
    int mShortTextStart;
    int mShortTextEnd;
    hexpadType mHexpadType;
};
