#include "component.h"
#include "drawing.h"
#include "ssfn.h"
#include "theme.h"

#include <string>

namespace {
    char* previousLabel = NULL;
    pointerVector<utils::textContainer*>* lines = NULL;
}

void component::panel(uint32_t fill, uint32_t stroke, int x, int y, int width, int height) {
    drawing::drawNinePatch("panel-fill", fill, x, y, width, height);
    drawing::drawNinePatch("panel-stroke", stroke, x, y, width, height);
}

void component::button(bool selected, bool active, const char* label, int x, int y, int width, int height) {
    int textWidth;
    int textHeight;
    drawing::measureBitmapString(context::getBitmapFontSmall(), label, &textWidth, &textHeight);

    if (active) {
        if (selected) {
            panel(
                theme::getButtonActiveHoverFillColor(), theme::getButtonActiveHoverStrokeColor(), x, y, width, height);
            drawing::drawBitmapString(context::getBitmapFontSmall(), label, theme::getButtonActiveHoverTextColor(),
                x + ((width - textWidth) / 2), y + ((height - textHeight) / 2) - 3);
        } else {
            panel(theme::getButtonActiveFillColor(), theme::getButtonActiveStrokeColor(), x, y, width, height);
            drawing::drawBitmapString(context::getBitmapFontSmall(), label, theme::getButtonActiveTextColor(),
                x + ((width - textWidth) / 2), y + ((height - textHeight) / 2) - 3);
        }
    } else {
        if (selected) {
            panel(theme::getButtonInactiveHoverFillColor(), theme::getButtonInactiveHoverStrokeColor(), x, y, width,
                height);
            drawing::drawBitmapString(context::getBitmapFontSmall(), label, theme::getButtonInactiveHoverTextColor(),
                x + ((width - textWidth) / 2), y + ((height - textHeight) / 2) - 3);
        } else {
            panel(theme::getButtonInactiveFillColor(), theme::getButtonInactiveStrokeColor(), x, y, width, height);
            drawing::drawBitmapString(context::getBitmapFontSmall(), label, theme::getButtonInactiveTextColor(),
                x + ((width - textWidth) / 2), y + ((height - textHeight) / 2) - 3);
        }
    }
}

void component::splitButton(bool selected, bool active, const char* label1, int label1Width, const char* label2, int x,
    int y, int width, int height, bool label2RightAlign) {
    int textWidth;
    int textHeight;
    drawing::measureBitmapString(context::getBitmapFontSmall(), label2, &textWidth, &textHeight);

    int label2Pos = label2RightAlign ? (x + width - 10 - textWidth) : (x + 10 + label1Width);

    if (active) {
        if (selected) {
            panel(
                theme::getButtonActiveHoverFillColor(), theme::getButtonActiveHoverStrokeColor(), x, y, width, height);
            drawing::drawBitmapString(context::getBitmapFontSmall(), label1, theme::getButtonActiveHoverTextColor(),
                x + 10, y + ((height - textHeight) / 2) - 3);
            drawing::drawBitmapString(context::getBitmapFontSmall(), label2, theme::getButtonActiveHoverTextColor(),
                label2Pos, y + ((height - textHeight) / 2) - 3);
        } else {
            panel(theme::getButtonActiveFillColor(), theme::getButtonActiveStrokeColor(), x, y, width, height);
            drawing::drawBitmapString(context::getBitmapFontSmall(), label1, theme::getButtonActiveTextColor(), x + 10,
                y + ((height - textHeight) / 2) - 3);
            drawing::drawBitmapString(context::getBitmapFontSmall(), label2, theme::getButtonActiveTextColor(),
                label2Pos, y + ((height - textHeight) / 2) - 3);
        }
    } else {
        if (selected) {
            panel(theme::getButtonInactiveHoverFillColor(), theme::getButtonInactiveHoverStrokeColor(), x, y, width,
                height);
            drawing::drawBitmapString(context::getBitmapFontSmall(), label1, theme::getButtonInactiveHoverTextColor(),
                x + 10, y + ((height - textHeight) / 2) - 3);
            drawing::drawBitmapString(context::getBitmapFontSmall(), label2, theme::getButtonInactiveHoverTextColor(),
                label2Pos, y + ((height - textHeight) / 2) - 3);
        } else {
            panel(theme::getButtonInactiveFillColor(), theme::getButtonInactiveStrokeColor(), x, y, width, height);
            drawing::drawBitmapString(context::getBitmapFontSmall(), label1, theme::getButtonInactiveTextColor(),
                x + 10, y + ((height - textHeight) / 2) - 3);
            drawing::drawBitmapString(context::getBitmapFontSmall(), label2, theme::getButtonInactiveTextColor(),
                label2Pos, y + ((height - textHeight) / 2) - 3);
        }
    }
}

void component::splitButtonWithColor(bool selected, bool active, const char* label1, int label1Width, const char* label2,
    uint32_t colorValue, int x, int y, int width, int height, bool label2RightAlign) {
    int textWidth;
    int textHeight;
    drawing::measureBitmapString(context::getBitmapFontSmall(), label2, &textWidth, &textHeight);

    // Color preview box size
    const int colorBoxSize = 24;
    const int colorBoxSpacing = 8;

    // Calculate label2 position, leaving space for color preview
    int label2Pos = label2RightAlign 
        ? (x + width - 10 - textWidth - colorBoxSize - colorBoxSpacing) 
        : (x + 10 + label1Width);

    // Color preview box position (right side, centered vertically)
    int colorBoxX = x + width - 10 - colorBoxSize;
    int colorBoxY = y + ((height - colorBoxSize) / 2);

    // Draw the split button background
    if (active) {
        if (selected) {
            panel(
                theme::getButtonActiveHoverFillColor(), theme::getButtonActiveHoverStrokeColor(), x, y, width, height);
            drawing::drawBitmapString(context::getBitmapFontSmall(), label1, theme::getButtonActiveHoverTextColor(),
                x + 10, y + ((height - textHeight) / 2) - 3);
            drawing::drawBitmapString(context::getBitmapFontSmall(), label2, theme::getButtonActiveHoverTextColor(),
                label2Pos, y + ((height - textHeight) / 2) - 3);
        } else {
            panel(theme::getButtonActiveFillColor(), theme::getButtonActiveStrokeColor(), x, y, width, height);
            drawing::drawBitmapString(context::getBitmapFontSmall(), label1, theme::getButtonActiveTextColor(), x + 10,
                y + ((height - textHeight) / 2) - 3);
            drawing::drawBitmapString(context::getBitmapFontSmall(), label2, theme::getButtonActiveTextColor(),
                label2Pos, y + ((height - textHeight) / 2) - 3);
        }
    } else {
        if (selected) {
            panel(theme::getButtonInactiveHoverFillColor(), theme::getButtonInactiveHoverStrokeColor(), x, y, width,
                height);
            drawing::drawBitmapString(context::getBitmapFontSmall(), label1, theme::getButtonInactiveHoverTextColor(),
                x + 10, y + ((height - textHeight) / 2) - 3);
            drawing::drawBitmapString(context::getBitmapFontSmall(), label2, theme::getButtonInactiveHoverTextColor(),
                label2Pos, y + ((height - textHeight) / 2) - 3);
        } else {
            panel(theme::getButtonInactiveFillColor(), theme::getButtonInactiveStrokeColor(), x, y, width, height);
            drawing::drawBitmapString(context::getBitmapFontSmall(), label1, theme::getButtonInactiveTextColor(),
                x + 10, y + ((height - textHeight) / 2) - 3);
            drawing::drawBitmapString(context::getBitmapFontSmall(), label2, theme::getButtonInactiveTextColor(),
                label2Pos, y + ((height - textHeight) / 2) - 3);
        }
    }

    // Draw color preview box (ensure color has alpha channel)
    uint32_t colorWithAlpha = colorValue | 0xff000000;
    panel(colorWithAlpha, 0xff000000, colorBoxX, colorBoxY, colorBoxSize, colorBoxSize);
}

void component::fileButton(
    bool selected, bool active, bool isFile, const char* label, int x, int y, int width, int height) {
    int textWidth;
    int textHeight;
    drawing::measureBitmapString(context::getBitmapFontSmall(), label, &textWidth, &textHeight);

    if (active) {
        if (selected) {
            panel(
                theme::getButtonActiveHoverFillColor(), theme::getButtonActiveHoverStrokeColor(), x, y, width, height);
            drawing::drawBitmapString(context::getBitmapFontSmall(), label, theme::getButtonActiveHoverTextColor(),
                x + 30, y + ((height - textHeight) / 2) - 3);
            drawing::drawBitmapString(context::getBitmapFontSmall(), isFile ? "\xC2\xA6" : "\xC2\xA5",
                theme::getButtonActiveHoverTextColor(), x + 10, y + ((height - textHeight) / 2) - 3);
        } else {
            panel(theme::getButtonActiveFillColor(), theme::getButtonActiveStrokeColor(), x, y, width, height);
            drawing::drawBitmapString(context::getBitmapFontSmall(), label, theme::getButtonActiveTextColor(), x + 30,
                y + ((height - textHeight) / 2) - 3);
            drawing::drawBitmapString(context::getBitmapFontSmall(), isFile ? "\xC2\xA6" : "\xC2\xA5",
                theme::getButtonActiveTextColor(), x + 10, y + ((height - textHeight) / 2) - 3);
        }
    } else {
        if (selected) {
            panel(theme::getButtonInactiveHoverFillColor(), theme::getButtonInactiveHoverStrokeColor(), x, y, width,
                height);
            drawing::drawBitmapString(context::getBitmapFontSmall(), label, theme::getButtonInactiveHoverTextColor(),
                x + 30, y + ((height - textHeight) / 2) - 3);
            drawing::drawBitmapString(context::getBitmapFontSmall(), isFile ? "\xC2\xA6" : "\xC2\xA5",
                theme::getButtonInactiveHoverTextColor(), x + 10, y + ((height - textHeight) / 2) - 3);
        } else {
            panel(theme::getButtonInactiveFillColor(), theme::getButtonInactiveStrokeColor(), x, y, width, height);
            drawing::drawBitmapString(context::getBitmapFontSmall(), label, theme::getButtonInactiveTextColor(), x + 30,
                y + ((height - textHeight) / 2) - 3);
            drawing::drawBitmapString(context::getBitmapFontSmall(), isFile ? "\xC2\xA6" : "\xC2\xA5",
                theme::getButtonInactiveTextColor(), x + 10, y + ((height - textHeight) / 2) - 3);
        }
    }
}

void component::ledButton(bool selected, uint8_t ledColor, const char* label, int x, int y, int width, int height) {
    int textWidth;
    int textHeight;
    drawing::measureBitmapString(context::getBitmapFontSmall(), label, &textWidth, &textHeight);

    if (selected) {
        if (ledColor == 0) {
            panel(
                theme::getButtonLedOffHoverFillColor(), theme::getButtonLedOffHoverStrokeColor(), x, y, width, height);
            drawing::drawBitmapString(context::getBitmapFontSmall(), label, theme::getButtonLedOffHoverTextColor(),
                x + 10, y + ((height - textHeight) / 2) - 3);
        } else if (ledColor == 1) {
            panel(
                theme::getButtonLedRedHoverFillColor(), theme::getButtonLedRedHoverStrokeColor(), x, y, width, height);
            drawing::drawBitmapString(context::getBitmapFontSmall(), label, theme::getButtonLedRedHoverTextColor(),
                x + 10, y + ((height - textHeight) / 2) - 3);
        } else if (ledColor == 2) {
            panel(theme::getButtonLedGreenHoverFillColor(), theme::getButtonLedGreenHoverStrokeColor(), x, y, width,
                height);
            drawing::drawBitmapString(context::getBitmapFontSmall(), label, theme::getButtonLedGreenHoverTextColor(),
                x + 10, y + ((height - textHeight) / 2) - 3);
        } else if (ledColor == 3) {
            panel(theme::getButtonLedYellowHoverFillColor(), theme::getButtonLedYellowHoverStrokeColor(), x, y, width,
                height);
            drawing::drawBitmapString(context::getBitmapFontSmall(), label, theme::getButtonLedYellowHoverTextColor(),
                x + 10, y + ((height - textHeight) / 2) - 3);
        } else if (ledColor == 4) {
            panel(theme::getButtonLedBlueHoverFillColor(), theme::getButtonLedBlueHoverStrokeColor(), x, y, width,
                height);
            drawing::drawBitmapString(context::getBitmapFontSmall(), label, theme::getButtonLedBlueHoverTextColor(),
                x + 10, y + ((height - textHeight) / 2) - 3);
        } else if (ledColor == 5) {
            panel(theme::getButtonLedPurpleHoverFillColor(), theme::getButtonLedPurpleHoverStrokeColor(), x, y, width,
                height);
            drawing::drawBitmapString(context::getBitmapFontSmall(), label, theme::getButtonLedPurpleHoverTextColor(),
                x + 10, y + ((height - textHeight) / 2) - 3);
        } else if (ledColor == 6) {
            panel(theme::getButtonLedTurquoiseHoverFillColor(), theme::getButtonLedTurquoiseHoverStrokeColor(), x, y,
                width, height);
            drawing::drawBitmapString(context::getBitmapFontSmall(), label,
                theme::getButtonLedTurquoiseHoverTextColor(), x + 10, y + ((height - textHeight) / 2) - 3);
        } else if (ledColor == 7) {
            panel(theme::getButtonLedWhiteHoverFillColor(), theme::getButtonLedWhiteHoverStrokeColor(), x, y, width,
                height);
            drawing::drawBitmapString(context::getBitmapFontSmall(), label, theme::getButtonLedWhiteHoverTextColor(),
                x + 10, y + ((height - textHeight) / 2) - 3);
        }
    } else {
        if (ledColor == 0) {
            panel(theme::getButtonLedOffFillColor(), theme::getButtonLedOffStrokeColor(), x, y, width, height);
            drawing::drawBitmapString(context::getBitmapFontSmall(), label, theme::getButtonLedOffTextColor(), x + 10,
                y + ((height - textHeight) / 2) - 3);
        } else if (ledColor == 1) {
            panel(theme::getButtonLedRedFillColor(), theme::getButtonLedRedStrokeColor(), x, y, width, height);
            drawing::drawBitmapString(context::getBitmapFontSmall(), label, theme::getButtonLedRedTextColor(), x + 10,
                y + ((height - textHeight) / 2) - 3);
        } else if (ledColor == 2) {
            panel(theme::getButtonLedGreenFillColor(), theme::getButtonLedGreenStrokeColor(), x, y, width, height);
            drawing::drawBitmapString(context::getBitmapFontSmall(), label, theme::getButtonLedGreenTextColor(), x + 10,
                y + ((height - textHeight) / 2) - 3);
        } else if (ledColor == 3) {
            panel(theme::getButtonLedYellowFillColor(), theme::getButtonLedYellowStrokeColor(), x, y, width, height);
            drawing::drawBitmapString(context::getBitmapFontSmall(), label, theme::getButtonLedYellowTextColor(),
                x + 10, y + ((height - textHeight) / 2) - 3);
        } else if (ledColor == 4) {
            panel(theme::getButtonLedBlueFillColor(), theme::getButtonLedBlueStrokeColor(), x, y, width, height);
            drawing::drawBitmapString(context::getBitmapFontSmall(), label, theme::getButtonLedBlueTextColor(), x + 10,
                y + ((height - textHeight) / 2) - 3);
        } else if (ledColor == 5) {
            panel(theme::getButtonLedPurpleFillColor(), theme::getButtonLedPurpleStrokeColor(), x, y, width, height);
            drawing::drawBitmapString(context::getBitmapFontSmall(), label, theme::getButtonLedPurpleTextColor(),
                x + 10, y + ((height - textHeight) / 2) - 3);
        } else if (ledColor == 6) {
            panel(theme::getButtonLedTurquoiseFillColor(), theme::getButtonLedTurquoiseStrokeColor(), x, y, width,
                height);
            drawing::drawBitmapString(context::getBitmapFontSmall(), label, theme::getButtonLedTurquoiseTextColor(),
                x + 10, y + ((height - textHeight) / 2) - 3);
        } else if (ledColor == 7) {
            panel(theme::getButtonLedWhiteFillColor(), theme::getButtonLedWhiteStrokeColor(), x, y, width, height);
            drawing::drawBitmapString(context::getBitmapFontSmall(), label, theme::getButtonLedWhiteTextColor(), x + 10,
                y + ((height - textHeight) / 2) - 3);
        }
    }
}

void component::text(const char* label, bool disabled, horizAlignment hAlign, int x, int y, int width, int height) {
    int textWidth;
    int textHeight;
    drawing::measureBitmapString(context::getBitmapFontSmall(), label, &textWidth, &textHeight);

    int xPos = x;
    if (hAlign == horizAlignmentCenter) {
        xPos = x + ((width - textWidth) / 2);
    } else if (hAlign == horizAlignmentRight) {
        xPos = x + (width - textWidth);
    }

    drawing::drawBitmapString(context::getBitmapFontSmall(), label,
        disabled ? theme::getTextDisabledColor() : theme::getTextColor(), xPos, y + ((height - textHeight) / 2) - 3);
}

void component::textBox(const char* label, bool selected, bool disabled, horizAlignment hAlign, int x, int y, int width,
    int height, bool vAlignTop, bool solidBg, bool extraSmallFont) {

    bitmapFont* font = extraSmallFont ? context::getBitmapFontExtraSmall() : context::getBitmapFontSmall();

    if (previousLabel == NULL || strcmp(label, previousLabel) != 0) {
        utils::swapString(&previousLabel, label);
        if (lines != NULL) {
            delete (lines);
        }
        lines = drawing::wordWrapString(font, label, width);
    }

    uint32_t textColor = selected ? theme::getTextPanelHoverTextColor() : theme::getTextPanelTextColor();
    textColor = disabled ? theme::getTextDisabledColor() : textColor;
    uint32_t fillColor =
        solidBg ? theme::getTextPanelHoverFillColor() | 0xff000000 : theme::getTextPanelHoverFillColor();
    uint32_t strokeColor = selected ? theme::getTextPanelHoverStrokeColor() : theme::getTextPanelStrokeColor();

    uint32_t totalHeight = 0;
    for (uint32_t i = 0; i < lines->count(); i++) {
        totalHeight += lines->get(i)->height;
    }

    int yPos = vAlignTop ? (y + 16 - 3) : (y + ((height - totalHeight) / 2) - 3);

    panel(fillColor, strokeColor, x, y, width, height);

    for (uint32_t i = 0; i < lines->count(); i++) {
        utils::textContainer* line = lines->get(i);

        int xPos = x + 16;
        if (hAlign == horizAlignmentCenter) {
            xPos = x + ((width - line->width) / 2);
        } else if (hAlign == horizAlignmentRight) {
            xPos = (x + 16) + ((width - 32) - line->width);
        }

        drawing::drawBitmapString(font, line->message, textColor, xPos, yPos);

        yPos += line->height;
    }
}

void component::textPercentBox(const char* label, bool selected, bool disabled, horizAlignment hAlign, int x, int y,
    int width, int height, bool solidBg, uint32_t percent) {
    const int barPadding = 2;

    uint32_t textColor = selected ? theme::getTextPanelHoverTextColor() : theme::getTextPanelTextColor();
    textColor = disabled ? theme::getTextDisabledColor() : textColor;
    uint32_t fillColor =
        solidBg ? theme::getTextPanelHoverFillColor() | 0xff000000 : theme::getTextPanelHoverFillColor();
    uint32_t strokeColor = selected ? theme::getTextPanelHoverStrokeColor() : theme::getTextPanelStrokeColor();

    panel(fillColor, 0, x, y, width, height);

    int barX = x + barPadding;
    int barY = y + barPadding;
    int barWidth = width - (barPadding * 2);
    int barHeight = height - (barPadding * 2);


    if (percent > 100) {
        percent = 100;
    }

    int fillWidth = (int)((uint64_t)percent * (uint64_t)barWidth / 100ULL);
    if (fillWidth > barWidth) {
        fillWidth = barWidth;
    }
    if (fillWidth > 0) {
        uint32_t barFillColor = theme::getPercentBarFillColor();
        if (percent >= 95) {
            barFillColor = theme::getPercentBarFillCriticalColor();
        } else if (percent >= 80) {
            barFillColor = theme::getPercentBarFillWarnColor();
        }
        drawing::drawHorizontalLine(barFillColor, barX, barY, fillWidth, barHeight);
    }

    panel(0, strokeColor, x, y, width, height);

    int textWidth;
    int textHeight;
    drawing::measureBitmapString(context::getBitmapFontSmall(), label, &textWidth, &textHeight);

    int xPos = x + 16;
    if (hAlign == horizAlignmentCenter) {
        xPos = x + ((width - textWidth) / 2);
    } else if (hAlign == horizAlignmentRight) {
        xPos = x + (width - barPadding - textWidth);
    }

    drawing::drawBitmapString(context::getBitmapFontSmall(), label, textColor, xPos,
        y + ((height - textHeight) / 2) - 3);
}