#include "mainScene.h"
#include "sceneManager.h"
#include "menuScene.h"
#include "launchScene.h"

#include "..\context.h"
#include "..\drawing.h"
#include "..\component.h"
#include "..\ssfn.h"
#include "..\inputManager.h"
#include "..\settingsManager.h"
#include "..\stringUtility.h"
#include "..\socketUtility.h"
#include "..\ftpServer.h"
#include "..\httpServer.h"
#include "..\temperatureManager.h"
#include "..\xboxConfig.h"
#include "..\theme.h"
#include "..\network.h"
#include "..\audioPlayer.h"

mainScene::mainScene() {
    mSelectedControl = 0;
	mCpuUsage = 0;
	mFanPct = 0;
	mCounter = 0;
    refreshBypass();
}

void mainScene::update() {
#ifdef TOOLS

    if (inputManager::buttonPressed(JoystickButtonY)) {
        context::setModchipType(modchip::detectModchip());
        refreshBypass();
    }

#endif

    if (inputManager::buttonPressed(JoystickButtonX) && needsBypass()) {
        mNeedsBypass = !attemptBypass();
        refreshBypass();
    }

    // Select Actions

    if (inputManager::buttonPressed(JoystickButtonA)) {
#ifdef TOOLS

        if (mSelectedControl == 0) {
            if (context::getModchipType() == modchipTypeXenium) {
                context::setModchipType(modchipTypeXecuter);
            } else if (context::getModchipType() == modchipTypeXecuter) {
                context::setModchipType(modchipTypeAladdin1mb);
            } else if (context::getModchipType() == modchipTypeAladdin1mb) {
                context::setModchipType(modchipTypeAladdin2mb);
            } else if (context::getModchipType() == modchipTypeAladdin2mb) {
                context::setModchipType(modchipTypeXchanger);
            } else if (context::getModchipType() == modchipTypeXchanger) {
                context::setModchipType(modchipTypeModxo);
            } else if (context::getModchipType() == modchipTypeModxo) {
                context::setModchipType(modchipTypeSmartxxOpx);
            } else if (context::getModchipType() == modchipTypeSmartxxOpx) {
                context::setModchipType(modchipTypeSmartxx);
            } else if (context::getModchipType() == modchipTypeSmartxx) {
                context::setModchipType(modchipTypeXenium);
            }
            refreshBypass();
        } else if (mSelectedControl == 1) {
            sceneManager::pushScene(sceneItemFlashToolsScene);
            return;
        }

#else

        if (mSelectedControl == 0) {
            sceneManager::pushScene(sceneItemLaunchScene);
            return;
        } else if (mSelectedControl == 1) {
            sceneManager::pushScene(sceneItemBankManagementScene);
            return;
        }

#endif

        if (mSelectedControl == 2) {
            sceneManager::pushScene(sceneItemSystemScene);
            return;
        } else if (mSelectedControl == 3) {
            utils::reboot();
            return;
        } else if (mSelectedControl == 4) {
            utils::shutdown();
            return;
        }
    }

    // Down Actions

    if (inputManager::buttonPressed(JoystickButtonDpadDown)) {
        mSelectedControl = mSelectedControl < 4 ? mSelectedControl + 1 : 0;
    }

    // Up Actions

    if (inputManager::buttonPressed(JoystickButtonDpadUp)) {
        mSelectedControl = mSelectedControl > 0 ? mSelectedControl - 1 : 4;
    }

    network::init();
}

void mainScene::render() {
    component::panel(theme::getPanelFillColor(), theme::getPanelStrokeColor(), 16, 16, 688, 448);
#ifndef TOOLS
    drawing::drawBitmapStringAligned(context::getBitmapFontLarge(),
        "\xC2\xA7\xC2\xA8\xC2\xA9\xC2\xAA\xC2\xAB\xC2\xAC\xC2\xAD\xC2\xAB\xC2\xA9\xC2\xAE", theme::getPrometheosColor(),
        theme::getPrometheosAlign(), 40, theme::getPrometheosY(), 640);
#else
    drawing::drawBitmapStringAligned(context::getBitmapFontLarge(),
        "\xC2\xA7\xC2\xA8\xC2\xA9\xC2\xAA\xC2\xAB\xC2\xAC\xC2\xAD\xC2\xAB\xC2\xA9\xC2\xAE "
        "\xC2\xAC\xC2\xA9\xC2\xA9\xC2\xB4\xC2\xAE",
        theme::getPrometheosColor(), theme::getPrometheosAlign(), 40, theme::getPrometheosY(), 640);
#endif

    int32_t yPos = (context::getBufferHeight() - (5 * 40) - 10) / 2;
    yPos += theme::getCenterOffset();

#ifdef TOOLS

    if (context::getModchipType() == modchipTypeDummy) {
        component::button(mSelectedControl == 0, false, "Modchip: Dummy", 193, yPos, 322, 30);
    } else if (context::getModchipType() == modchipTypeXenium) {
        component::button(mSelectedControl == 0, false, "Modchip: Xenium", 193, yPos, 322, 30);
    } else if (context::getModchipType() == modchipTypeXecuter) {
        component::button(mSelectedControl == 0, false, "Modchip: Xecuter", 193, yPos, 322, 30);
    } else if (context::getModchipType() == modchipTypeAladdin1mb) {
        component::button(mSelectedControl == 0, false, "Modchip: Aladdin 1MB", 193, yPos, 322, 30);
    } else if (context::getModchipType() == modchipTypeAladdin2mb) {
        component::button(mSelectedControl == 0, false, "Modchip: Aladdin 2MB", 193, yPos, 322, 30);
    } else if (context::getModchipType() == modchipTypeXchanger) {
        component::button(mSelectedControl == 0, false, "Modchip: Xchanger", 193, yPos, 322, 30);
    } else if (context::getModchipType() == modchipTypeModxo) {
        component::button(mSelectedControl == 0, false, "Modchip: Modxo", 193, yPos, 322, 30);
    } else if (context::getModchipType() == modchipTypeSmartxxOpx) {
        component::button(mSelectedControl == 0, false,
            mNeedsBypass ? "Modchip: SmartXX OPX *" : "Modchip: SmartXX OPX", 193, yPos, 322, 30);
    } else if (context::getModchipType() == modchipTypeSmartxx) {
        component::button(
            mSelectedControl == 0, false, mNeedsBypass ? "Modchip: SmartXX *" : "Modchip: SmartXX", 193, yPos, 322, 30);
    }

    yPos += 40;

    component::button(mSelectedControl == 1, false, "Flash Tools", 193, yPos, 322, 30);

    yPos += 40;

#else

    component::button(mSelectedControl == 0, false, "Launch Bank", 193, yPos, 322, 30);

    yPos += 40;

    component::button(mSelectedControl == 1, false, "Bank Management", 193, yPos, 322, 30);

    yPos += 40;

#endif

    component::button(mSelectedControl == 2, false, "System", 193, yPos, 322, 30);

    yPos += 40;

    component::button(mSelectedControl == 3, false, "Reboot", 193, yPos, 322, 30);

    yPos += 40;

    component::button(mSelectedControl == 4, false, "Shutdown", 193, yPos, 322, 30);

#ifdef TOOLS
    if (mNeedsBypass == true && (context::getModchipType() == modchipTypeSmartxx || context::getModchipType() == modchipTypeSmartxxOpx)) {
        drawing::drawBitmapStringAligned(context::getBitmapFontSmall(),
            "* Hot-Swap and Press \xC2\xA3 to Attempt Unlock.", theme::getFooterTextColor(), horizAlignmentCenter, 40,
            theme::getFooterY() - 30, 640);
    }
#endif

	const int ipWidth = 150;
	//const int cpuUsageWidth = 100;
	const int cpuUsageWidth = 0;
	const int cpuTempWidth = 100;
	const int mbTempWidth = 100;
	const int freeMemWidth = 150;
	const int totalWidth = ipWidth + cpuUsageWidth + cpuTempWidth + mbTempWidth + freeMemWidth;
	const int padding = (720 - totalWidth) / 2;

	int x = padding;

    char* currentIp = context::getCurrentIp();
    char* ip = stringUtility::formatString("IP %s", currentIp);
    drawing::drawBitmapStringAligned(context::getBitmapFontSmall(), ip, theme::getFooterTextColor(),
        horizAlignmentCenter, x, theme::getFooterY(), ipWidth);
    free(ip);
    free(currentIp);

	x += ipWidth;

	// if (mCounter == 0) {
	// 	//AvSendTVEncoderOption(NULL, 25, 0, &mCpuUsage);
    //     HalReadSMBusByte(PIC_ADDRESS, FAN_READBACK, (DWORD*)&mFanPct);
	// }
	// mCounter = (mCounter + 1) % 90;
	
	// char* cpuUsage = stringUtility::formatString("FAN %i%%", mFanPct*2);
    // drawing::drawBitmapStringAligned(context::getBitmapFontSmall(), cpuUsage, theme::getFooterTextColor(),
    //     horizAlignmentCenter, x, theme::getFooterY(), cpuUsageWidth);
    // free(cpuUsage);

	// x += cpuUsageWidth;

    char* cpuTemp = stringUtility::formatString("CPU %i\xC2\xB1" "C", context::getCurrentCpuTemp());
    drawing::drawBitmapStringAligned(context::getBitmapFontSmall(), cpuTemp, theme::getFooterTextColor(),
        horizAlignmentCenter, x, theme::getFooterY(), cpuTempWidth);
    free(cpuTemp);

	x += cpuTempWidth;

    char* mbTemp = stringUtility::formatString("M/B %i\xC2\xB1"
                                                "C",
        context::getCurrentMbTemp());
    drawing::drawBitmapStringAligned(context::getBitmapFontSmall(), mbTemp, theme::getFooterTextColor(),
        horizAlignmentCenter, x, theme::getFooterY(), mbTempWidth);
    free(mbTemp);

	x += mbTempWidth;

    char* memoryFormat = stringUtility::formatSize(context::getCurrentFreeMem());
    char* memory = stringUtility::formatString("Free Mem %s", memoryFormat);
    drawing::drawBitmapStringAligned(context::getBitmapFontSmall(), memory, theme::getFooterTextColor(),
        horizAlignmentCenter, x, theme::getFooterY(), freeMemWidth);
    free(memory);
    free(memoryFormat);
}

uint8_t mainScene::smartxxGetManId() {
    utils::ioOutputByte(0xf704, 0x1);

    volatile uint8_t* lpcMemMap = (volatile uint8_t*)LPC_MEMORY_BASE;
    lpcMemMap[utils::smartxxMangleFlashAddress(0xAAA)] = utils::smartxxMangleCommand(0xAA);
    lpcMemMap[utils::smartxxMangleFlashAddress(0x555)] = utils::smartxxMangleCommand(0x55);
    lpcMemMap[utils::smartxxMangleFlashAddress(0xAAA)] = utils::smartxxMangleCommand(0x90);

    uint8_t id = utils::smartxxUnmangleCommandResponse(lpcMemMap[0]);

    lpcMemMap[0] = utils::smartxxMangleCommand(0xF0);
    utils::ioOutputByte(0xf704, 0x0);

    return id;
}

void mainScene::refreshBypass() {
    mNeedsBypass = needsBypass();
    context::setIsSmartxxSysconUnlocked(!mNeedsBypass);
}

bool mainScene::needsBypass() {
    if (context::getModchipType() == modchipTypeSmartxxOpx || context::getModchipType() == modchipTypeSmartxx) {
        uint8_t id = smartxxGetManId();
        return !(id == 0xC2 || id == 0x01); // MACRONIX or AMD TSOP
    } else {
        return false;
    }
}

bool mainScene::attemptBypass() {
    uint8_t syscon_ver = utils::ioInputByte(0xf701) & 0xf;

    if (context::getModchipType() == modchipTypeSmartxxOpx || syscon_ver == 8) {
        // SmartXX OPX or v3 Bypass

        utils::ioOutputByte(0xf7b6, 1);

        for (int i = 0; i < 0x746; i++) {
            utils::ioOutputByte(0xf7b6, 0);
            uint32_t val = utils::ioInputInt(0xf708);
            if (i == 0 && val != 0xD9D120DB) {
                return false;
            }
            if (i == 0x745 && val != 0xBED5CD65) {
                return false;
            }
        }

        utils::ioOutputByte(0xf7b6, 0x80);
        utils::ioOutputByte(0xf708, 0xfc);
        utils::ioOutputByte(0xf709, 0x40);
        utils::ioOutputByte(0xf707, 0xff);

        return true;
    }

    // SmartXX Bypass

    // 0xF7B6 = Syscon PRNG seed for next 4 key bytes (returns 0xFF if not supplied)
    utils::ioOutputByte(0xf7b6, 1);

    for (int i = 0; i < 0x746; i++) {
        utils::ioOutputByte(0xf7b6, 0);
        uint32_t val = utils::ioInputInt(0xf708);
        if (i == 0 && val != 0x1F623100) {
            return false;
        }
        if (i == 0x745 && val != 0x28B5E8CC) {
           return false;
        }
    }

    utils::ioOutputByte(0xf7b6, 0x80);
    utils::ioOutputByte(0xf708, 0xb0);
    utils::ioOutputByte(0xf709, 0xb0);
    utils::ioOutputByte(0xf707, 0xff);

    return true;
}
