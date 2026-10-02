#include "autoBootScene.h"
#include "sceneManager.h"
#include "mainScene.h"

#include "..\context.h"
#include "..\drawing.h"
#include "..\component.h"
#include "..\ssfn.h"
#include "..\inputManager.h"
#include "..\settingsManager.h"
#include "..\stringUtility.h"
#include "..\xboxConfig.h"
#include "..\theme.h"
#include "..\driveManager.h"
#include "..\Threads\countdown.h"
#include "..\sdcard.h"

autoBootScene::autoBootScene() {
    utils::setLedStates(
        SMC_LED_STATES_RED_STATE0 | SMC_LED_STATES_RED_STATE1 | SMC_LED_STATES_RED_STATE2 | SMC_LED_STATES_RED_STATE3);
    countdown::startThread(settingsManager::getAutoBootDelay() * 1000);
}

void autoBootScene::close() {
    countdown::closeThread();
    HalWriteSMBusByte(SMC_SLAVE_ADDRESS, 0x0C, 1);

    utils::setLedStates(SMC_LED_STATES_GREEN_STATE0 | SMC_LED_STATES_GREEN_STATE1 | SMC_LED_STATES_GREEN_STATE2 |
        SMC_LED_STATES_GREEN_STATE3);
    sceneManager::popScene();
}

void autoBootScene::update() {
    uint32_t trayState = 0;
    uint32_t ejectCount = 0x11223344;
    HalReadSMCTrayState(&trayState, &ejectCount);
    bool safeMode = (trayState == SMC_TRAY_STATE_UNLOADING || ejectCount > 0);

    // Exit Action

    if (inputManager::buttonPressed(JoystickButtonA)) {
        continueBoot();
    }

    if (inputManager::buttonPressed(JoystickButtonB)) {
        safeMode = true;
    }

    if (safeMode == true) {
        close();
    }
}

void autoBootScene::render() {
    pointerVector<bankDetails*>* banks = settingsManager::getBankInfos(true);

    component::panel(theme::getPanelFillColor(), theme::getPanelStrokeColor(), 16, 16, 688, 448);
    drawing::drawBitmapStringAligned(context::getBitmapFontMedium(), "Auto booting...", theme::getHeaderTextColor(),
        theme::getHeaderAlign(), 40, theme::getHeaderY(), 640);

    int yPos = (context::getBufferHeight() - (40 + 44)) / 2;
    yPos += theme::getCenterOffset();

    for (uint32_t i = 0; i < banks->count(); i++) {
        bankDetails* bank = banks->get(i);
        if (bank->autoBoot == false) {
            continue;
        }

        char* bankName = NULL;
        if (bank->category == (uint8_t)bankCategorySdCard) {
            char* launchPath = settingsManager::getSdCardLaunchPath();
            if (launchPath != NULL && launchPath[0] != '\0') {
                char* sdPath = utils::toSdCardLpcPath(launchPath);
                if (sdPath != NULL) {
                    char* path = sdPath;
                    if(path[0] == '\\') path++;
                    bankName = stringUtility::formatString("%s:\\%s", bank->name, path);
                    free(sdPath);
                }
            }
        }

        if (bankName == NULL) {
            bankName = strdup(bank->name);
        }

        drawing::drawBitmapStringAligned(context::getBitmapFontMedium(), bankName, theme::getTextColor(), horizAlignmentCenter, 40, yPos, 640);

        free(bankName);
    }

    yPos += 40;

    char* progress = stringUtility::formatString("Booting in %.1f seconds", countdown::getTimeRemaining() / 1000.0f);
    component::textBox(progress, false, false, horizAlignmentCenter, 193, yPos, 322, 44, false, false, false);
    free(progress);

    delete (banks);

    if (countdown::getTimeRemaining() == 0) {
        continueBoot();
    }

    drawing::drawBitmapString(
        context::getBitmapFontSmall(), "\xC2\xA1 Continue", theme::getFooterTextColor(), 40, theme::getFooterY());
    drawing::drawBitmapStringAligned(context::getBitmapFontSmall(), "\xC2\xA2 Cancel", theme::getFooterTextColor(),
        horizAlignmentRight, 40, theme::getFooterY(), 640);
}

void autoBootScene::continueBoot() {
    countdown::closeThread();

    pointerVector<bankDetails*>* banks = settingsManager::getBankInfos(true);

    for (uint32_t i = 0; i < banks->count(); i++) {
        bankDetails* bank = banks->get(i);
        if (bank->slots > 0 && bank->autoBoot == true) {
            utils::setLedStates(SMC_LED_STATES_GREEN_STATE0 | SMC_LED_STATES_GREEN_STATE1 |
                SMC_LED_STATES_GREEN_STATE2 | SMC_LED_STATES_GREEN_STATE3);
            if (bank->category == (uint8_t)bankCategoryFlash) {
                settingsManager::launchBank(bank->id, false);
            } else if (bank->category == (uint8_t)bankCategoryTsop) {
                settingsManager::launchTsop(false);
            } else if (bank->category == (uint8_t)bankCategoryRecovery) {
                settingsManager::launchRecovery(false);
            } else if (bank->category == (uint8_t)bankCategorySdCard) {
                settingsManager::launchSdCard(false);
            }
            break;
        }
    }

    delete (banks);
    close();
}
