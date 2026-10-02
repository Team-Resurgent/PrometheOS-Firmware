#include "xecuter4SettingsScene.h"
#include "sceneManager.h"

#include "..\context.h"
#include "..\drawing.h"
#include "..\component.h"
#include "..\ssfn.h"
#include "..\inputManager.h"
#include "..\settingsManager.h"
#include "..\stringUtility.h"
#include "..\xboxConfig.h"
#include "..\theme.h"
#include "..\fileSystem.h"

xecuter4SettingsScene::xecuter4SettingsScene() {
    mSelectedControl = 0;
    mBacklight = (uint8_t)((utils::ioInputByte(XECUTER_REGISTER_DISP_O_LIGHT) & 0x7f) / 1.27f);
    mContrast = (uint8_t)((utils::ioInputByte(XECUTER_REGISTER_DISP_O_CONTRAST) & 0x7f) / 1.27f);
}

void xecuter4SettingsScene::update() {
    // Exit Action

    if (inputManager::buttonPressed(JoystickButtonB)) {
        sceneManager::popScene();
        return;
    }

    // Down Actions

    if (inputManager::buttonPressed(JoystickButtonDpadDown)) {
        mSelectedControl = mSelectedControl < 1 ? mSelectedControl + 1 : 0;
    }

    // Up Actions

    if (inputManager::buttonPressed(JoystickButtonDpadUp)) {
        mSelectedControl = mSelectedControl > 0 ? mSelectedControl - 1 : 1;
    }

    // Left Trigger

    if (inputManager::buttonPressed(JoystickButtonLeftTrigger) || inputManager::buttonPressed(JoystickButtonX)) {

        if (mSelectedControl == 0) {
            mBacklight = (mBacklight >= 4) ? (mBacklight - 4) : 100;
            utils::ioOutputByte(XECUTER_REGISTER_DISP_O_LIGHT, (uint8_t)(mBacklight * 1.27f));
            return;
        }
        if (mSelectedControl == 1) {
            mContrast = (mContrast >= 4) ? (mContrast - 4) : 100;
            utils::ioOutputByte(XECUTER_REGISTER_DISP_O_CONTRAST, (uint8_t)(mContrast * 1.27f));
            return;
        }
    }

    // Right Trigger

    if (inputManager::buttonPressed(JoystickButtonRightTrigger) || inputManager::buttonPressed(JoystickButtonY)) {
       
        if (mSelectedControl == 0) {
            mBacklight = (mBacklight >= 100) ? 0 : (mBacklight + 4);
            utils::ioOutputByte(XECUTER_REGISTER_DISP_O_LIGHT, (uint8_t)(mBacklight * 1.27f));
            return;
        }
        if (mSelectedControl == 1) {
            mContrast = (mContrast >= 100) ? 0 : (mContrast + 4);
            utils::ioOutputByte(XECUTER_REGISTER_DISP_O_CONTRAST, (uint8_t)(mContrast * 1.27f));
            return;
        }
    }
}

void xecuter4SettingsScene::render() {
    component::panel(theme::getPanelFillColor(), theme::getPanelStrokeColor(), 16, 16, 688, 448);
    drawing::drawBitmapStringAligned(context::getBitmapFontMedium(), "LCD Options...", theme::getHeaderTextColor(),
        theme::getHeaderAlign(), 40, theme::getHeaderY(), 640);

    char* lcdBacklight = stringUtility::formatString("LCD Backlight: %i%%", settingsManager::getLcdBacklight(false));
    char* lcdContrast = stringUtility::formatString("LCD Contrast: %i%%", settingsManager::getLcdContrast(false));

    pointerVector<char*>* menuItems = new pointerVector<char*>(false);

    int32_t maxItems = 7;

    int32_t start = 0;
    if ((int32_t)menuItems->count() >= maxItems) {
        start = min(max(mSelectedControl - (maxItems / 2), 0), (int32_t)menuItems->count() - maxItems);
    }

    int32_t itemCount = min(start + maxItems, (int32_t)menuItems->count()) - start;
    if (itemCount > 0) {
        uint32_t yPos = (context::getBufferHeight() - ((itemCount * 40) - 10)) / 2;
        yPos += theme::getCenterOffset();

        for (int32_t i = 0; i < itemCount; i++) {
            uint32_t index = start + i;
            char* menuText = menuItems->get(index);
            component::button(mSelectedControl == index, false, menuText, 193, yPos, 322, 30);
            yPos += 40;
        }
    }

    free(lcdContrast);
    free(lcdBacklight);

    delete (menuItems);

    char* options = stringUtility::formatString("\xC2\xA3\xC2\xA4 or \xC2\xB2\xC2\xB3 Change Value");
    drawing::drawBitmapString(
        context::getBitmapFontSmall(), options, theme::getFooterTextColor(), 40, theme::getFooterY());
    free(options);
    drawing::drawBitmapStringAligned(context::getBitmapFontSmall(), "\xC2\xA2 Back", theme::getFooterTextColor(),
        horizAlignmentRight, 40, theme::getFooterY(), 640);
}
