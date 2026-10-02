#include "launchScene.h"
#include "audioSettingsScene.h"
#include "videoSettingsScene.h"
#include "mainScene.h"
#include "sceneManager.h"
#include "filePickerScene.h"

#include "..\context.h"
#include "..\drawing.h"
#include "..\component.h"
#include "..\ssfn.h"
#include "..\inputManager.h"
#include "..\stringUtility.h"
#include "..\alignment.h"
#include "..\theme.h"

#ifndef TOOLS
#include "..\driveManager.h"
#include "..\sdcard.h"
#include "..\Threads\lcdRender.h"
#include "..\utils.h"
#include "..\xboxInternals.h"
#endif

launchScene::launchScene() {
    mSelectedControl = 0;
    setupBanks();
}

launchScene::~launchScene() {
    delete (mBanks);
}

void launchScene::setupBanks() {
    mBanks = settingsManager::getBankInfos(true);
}
#ifndef TOOLS
void launchScene::onSdCardPickerClosingCallback(sceneResult result, void* context, scene* scene) {
    if (result == sceneResultCancelled) {
        return;
    }
    filePickerScene* closingScene = (filePickerScene*)scene;
    char* filePath = closingScene->getFilePath();
    settingsManager::setSdCardLaunchPath(filePath);
    free(filePath);
}

void launchScene::launchSdCardPicker() {
    sceneManager::pushScene(new sceneContainer(sceneItemGenericScene, new filePickerScene(filePickerTypeBios, false, true, "SDCARD:\\"), "SD Card", this, onSdCardPickerClosingCallback));
}

void launchScene::launchSdCard(bool safeMode) {
    char* launchPath = settingsManager::getSdCardLaunchPath();
    if (launchPath == NULL || launchPath[0] == '\0') {
        free(launchPath);
        return;
    }

    char* sdPath = utils::toSdCardLpcPath(launchPath);
    free(launchPath);
    if (sdPath != NULL) {
        launchSdCardFromPath(sdPath, safeMode);
        free(sdPath);
    }
}

void launchScene::launchSdCardFromPath(const char* sdPath, bool safeMode) {
    sdcard::open();
    bool result = sdcard::open_file_from_path(sdPath);
    if (result) {
        sdcard::map_bios_to_mem();
#ifndef TOOLS
        lcdRender::waitStop();
#endif
        utils::prepareScratch(safeMode);
        HalReturnToFirmware(RETURN_FIRMWARE_REBOOT);
    } else {
        sdcard::close();
    }
}
#endif
void launchScene::update() {
    if (inputManager::buttonPressed(JoystickButtonB)) {
        sceneManager::popScene();
        return;
    }
#ifndef TOOLS
    if (inputManager::buttonPressed(JoystickButtonWhite)) {
        if (mSelectedControl >= 0 && mSelectedControl < (int)mBanks->count() &&
            mBanks->get(mSelectedControl)->category == bankCategorySdCard) {
            launchSdCardPicker();
            return;
        }
    }
#endif
    if (inputManager::buttonPressed(JoystickButtonY)) {
        for (uint32_t i = 0; i < mBanks->count(); i++) {
            bankDetails* bank = mBanks->get(i);
            if (mSelectedControl == (int)i) {
                bank->autoBoot = !bank->autoBoot;
                settingsManager::toggleAutoBootBank(bank->id, bank->category != bankCategoryFlash);
            } else {
                bank->autoBoot = false;
            }
        }
    }

    if (inputManager::buttonPressed(JoystickButtonA) || inputManager::buttonPressed(JoystickButtonX)) {
        if (mSelectedControl < 0 || mSelectedControl >= (int)mBanks->count()) {
            return;
        }

        bankDetails* bank = mBanks->get(mSelectedControl);
#ifndef TOOLS
        if (bank->category == bankCategorySdCard) {
            if (inputManager::buttonPressed(JoystickButtonA)) {
                launchSdCard(false);
            } else if (inputManager::buttonPressed(JoystickButtonX)) {
                launchSdCard(true);
            }
            return;
        }
#endif
        bool safeMode = inputManager::buttonPressed(JoystickButtonX);
        if (bank->category == bankCategoryFlash) {
            settingsManager::launchBank(bank->id, safeMode);
        } else if (bank->category == bankCategoryTsop) {
            settingsManager::launchTsop(safeMode);
        } else if (bank->category == bankCategoryRecovery) {
            settingsManager::launchRecovery(safeMode);
        }
    }

    uint32_t bankCount = mBanks->count();

    if (inputManager::buttonPressed(JoystickButtonDpadDown)) {
        mSelectedControl = mSelectedControl < (int)(bankCount - 1) ? mSelectedControl + 1 : 0;
    }

    if (inputManager::buttonPressed(JoystickButtonDpadUp)) {
        mSelectedControl = mSelectedControl > 0 ? mSelectedControl - 1 : (int)(bankCount - 1);
    }
}

void launchScene::render() {
    component::panel(theme::getPanelFillColor(), theme::getPanelStrokeColor(), 16, 16, 688, 448);
    drawing::drawBitmapStringAligned(context::getBitmapFontMedium(), "Please select a bank to launch...",
        theme::getHeaderTextColor(), theme::getHeaderAlign(), 40, theme::getHeaderY(), 640);

    int32_t maxItems = 7;

    int32_t start = 0;
    if ((int32_t)mBanks->count() >= maxItems) {
        start = min(max(mSelectedControl - (maxItems / 2), 0), (int32_t)mBanks->count() - maxItems);
    }

    int32_t itemCount = min(start + maxItems, (int32_t)mBanks->count()) - start;
    if (itemCount > 0) {
        uint32_t yPos = (context::getBufferHeight() - ((itemCount * 40) - 10)) / 2;
        yPos += theme::getCenterOffset();

        for (int32_t i = 0; i < itemCount; i++) {
            uint32_t index = start + i;
            bankDetails* bankDetails = mBanks->get(index);

            char* bankName = NULL;
            if (context::getModchipType() == modchipTypeXecuter && bankDetails->category == bankCategoryFlash) {
                bankInfo bankInfo = settingsManager::getBankInfo(bankDetails->id);
                if (bankInfo.slots > 0) {
                    uint8_t bank = context::getModchip()->getBankFromIdAndSlots(bankDetails->id, bankInfo.slots);
                    char* nibble = utils::x3NibbleToBinary(bank);
                    bankName = stringUtility::formatString(bankDetails->autoBoot ? "* %s - %s *" : "%s - %s", nibble, bankDetails->name);
                    free(nibble);
                }
            }
#ifndef TOOLS
            if (bankName == NULL && bankDetails->category == bankCategorySdCard) {
                char* launchPath = settingsManager::getSdCardLaunchPath();
                if (launchPath != NULL && launchPath[0] != '\0') {
                    char* sdPath = utils::toSdCardLpcPath(launchPath);
                    if (sdPath != NULL) {
                        char* path = sdPath;
                        bankName = stringUtility::formatString(bankDetails->autoBoot ? "* %s:%s *" : "%s:%s", bankDetails->name, path);
                        free(sdPath);
                    }
                }
                free(launchPath);
            }
#endif
            if (bankName == NULL) {
                bankName = stringUtility::formatString(bankDetails->autoBoot ? "* %s *" : "%s", bankDetails->name);
            }

            component::button(mSelectedControl == (int)index, false, bankName, 40, yPos, 640, 30);
            free(bankName);

            yPos += 40;
        }
    } else {
        int yPos = ((context::getBufferHeight() - 44) / 2);
        yPos += theme::getCenterOffset();

        component::textBox("No items", false, false, horizAlignmentCenter, 40, yPos, 640, 44, false, false, false);
    }
#ifndef TOOLS
    bool sdCardSelected = mSelectedControl >= 0 && mSelectedControl < (int)mBanks->count() && mBanks->get(mSelectedControl)->category == bankCategorySdCard;
    if (sdCardSelected) {
        drawing::drawBitmapString(context::getBitmapFontSmall(), "\xC2\xA1 Launch \xC2\xA3 Safe Mode \xC2\xA4 Toggle Auto Boot \xC2\xB5 Browse", theme::getFooterTextColor(), 40, theme::getFooterY());
    } else
#endif
    {
        drawing::drawBitmapString(context::getBitmapFontSmall(), "\xC2\xA1 Launch \xC2\xA3 Safe Mode \xC2\xA4 Toggle Auto Boot", theme::getFooterTextColor(), 40, theme::getFooterY());
    }
    drawing::drawBitmapStringAligned(context::getBitmapFontSmall(), "\xC2\xA2 Cancel", theme::getFooterTextColor(),  horizAlignmentRight, 40, theme::getFooterY(), 640);
}
