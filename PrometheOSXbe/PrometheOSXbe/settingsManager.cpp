#include "settingsManager.h"
#include "modchip.h"
#include "modchipXenium.h"
#include "context.h"
#include "versioning.h"
#include "crc32.h"
#include "stringUtility.h"
#include "utils.h"
#include "Threads\lcdRender.h"
#include "xboxInternals.h"
#include "driveManager.h"
#include "sdcard.h"

#include <stdio.h>
#include <algorithm>

#define SETTINGS_OFFSET (0x1f8000 - 0x1c0000)
#define INSTALLER_LOGO_OFFSET (0x1f0000 - 0x1c0000)

namespace {
settingsState mSettingsCurrent;
settingsState mSettingsShadow;
} // namespace

semver settingsManager::getVersion() {
    semver version;
    memset(&version, 0, sizeof(version));
    version.major = 1;
    version.minor = 6;
    version.patch = 0;
    return version;
}

char* settingsManager::getVersionSting(semver version) {
    return stringUtility::formatString("%i.%i.%i", version.major, version.minor, version.patch);
}

#ifdef _DEBUG
#define TEST
#endif

void settingsManager::initSettings() {
    memset(&mSettingsCurrent, 0, sizeof(mSettingsCurrent));
    mSettingsCurrent.version = getVersion();
    mSettingsCurrent.autoBootDelay = 5;
    mSettingsCurrent.musicVolume = 75;
    mSettingsCurrent.soundVolume = 100;
    mSettingsCurrent.minFanSpeed = 10;
    mSettingsCurrent.ledColor = 2;
    mSettingsCurrent.lcdBacklight = 100;
    mSettingsCurrent.lcdContrast = 80;
    mSettingsCurrent.driveSetup = 1;
    mSettingsCurrent.udmaModeMaster = 2;
    mSettingsCurrent.udmaModeSlave = 2;
    mSettingsCurrent.splashDelay = 6;
    // #ifdef TEST
    // mSettingsCurrent.banks[0].slots = 1;
    // stringUtility::copyString(&mSettingsCurrent.banks[0].name[0], "Bank 1 Rom", 41);
    // mSettingsCurrent.banks[1].slots = 1;
    // stringUtility::copyString(&mSettingsCurrent.banks[1].name[0], "Bank 2 Rom", 41);
    // mSettingsCurrent.banks[2].slots = 1;
    // stringUtility::copyString(&mSettingsCurrent.banks[2].name[0], "Bank 3 Rom", 41);
    // mSettingsCurrent.banks[3].slots = 1;
    // stringUtility::copyString(&mSettingsCurrent.banks[3].name[0], "Bank 4 Rom", 41);
    // mSettingsCurrent.banks[4].slots = 1;
    // stringUtility::copyString(&mSettingsCurrent.banks[4].name[0], "Bank 5 Rom", 41);
    // mSettingsCurrent.banks[5].slots = 1;
    // stringUtility::copyString(&mSettingsCurrent.banks[5].name[0], "Bank 6 Rom", 41);

    //
    //	stringUtility::copyString(&mSettingsCurrent.skinName[0], "TeamResurgent-Animated", 51);
    //	stringUtility::copyString(&mSettingsCurrent.soundPackName[0], "Cybernoid", 51);
    //
    // #endif
    memcpy(&mSettingsShadow, &mSettingsCurrent, sizeof(mSettingsCurrent));
}

void settingsManager::loadSettings() {
#ifndef TOOLS
    context::getModchip()->loadSettings(mSettingsCurrent);
    memcpy(&mSettingsShadow, &mSettingsCurrent, sizeof(mSettingsCurrent));
#else
    initSettings();
#endif
}

void settingsManager::saveSettings() {
#ifndef TOOLS
    context::getModchip()->saveSettings(mSettingsShadow);
    memcpy(&mSettingsCurrent, &mSettingsShadow, sizeof(mSettingsCurrent));
#endif
}

bool settingsManager::shouldSave() {
    uint8_t* first = ((uint8_t*)&mSettingsCurrent) + sizeof(uint32_t);
    uint8_t* second = ((uint8_t*)&mSettingsShadow) + sizeof(uint32_t);
    for (int i = 0; i < sizeof(mSettingsCurrent) - sizeof(uint32_t); i++) {
        if (first[i] != second[i]) {
            return true;
        }
    }
    return false;
}

uint32_t settingsManager::getFreeSlots() {
    uint32_t slotCount = context::getModchip()->getSlotCount();
    uint32_t freeSlots = slotCount;
    for (uint32_t i = 0; i < slotCount; i++) {
        freeSlots = freeSlots - mSettingsCurrent.banks[i].slots;
    }
    return freeSlots;
}

char* settingsManager::getFreeSlotsJson() {
    char* freeSlotsJson = stringUtility::formatString("{\"freeslots\":%lu}", getFreeSlots());
    return freeSlotsJson;
}

bankInfo settingsManager::getBankInfo(uint8_t id) {
    return mSettingsCurrent.banks[id];
}

pointerVector<bankDetails*>* settingsManager::getBankInfos(bool includeOthers) {
    pointerVector<bankDetails*>* bankInfos = new pointerVector<bankDetails*>(true);

    uint32_t totalSlots = context::getModchip()->getSlotCount();
    for (uint32_t i = 0; i < totalSlots; i++) {
        if (mSettingsCurrent.banks[i].slots == 0) {
            continue;
        }

        bankDetails* bank = new bankDetails();
        bank->id = (uint8_t)i;
        bank->ledColor = mSettingsCurrent.banks[i].ledColor;
        bank->slots = mSettingsCurrent.banks[i].slots;
        bank->name = strdup(&mSettingsCurrent.banks[i].name[0]);
        bank->autoBoot = mSettingsCurrent.banks[i].autoBoot == 1;
        bank->category = bankCategoryFlash;
        bankInfos->add(bank);
    }

    if (includeOthers) {
        bankDetails* tsopBank = new bankDetails();
        tsopBank->id = (uint8_t)autoBootOtherTsop;
        tsopBank->ledColor = 0;
        tsopBank->slots = 1;
        tsopBank->name = strdup("TSOP");
        tsopBank->autoBoot = mSettingsCurrent.autoBootOther == (uint8_t)autoBootOtherTsop;
        tsopBank->category = bankCategoryTsop;
        bankInfos->add(tsopBank);

        if (context::getModchip()->getSupportInfo(true).supportsRecovery == true) {
            bankDetails* recoveryBank = new bankDetails();
            recoveryBank->id = (uint8_t)autoBootOtherRecovery;
            recoveryBank->ledColor = 0;
            recoveryBank->slots = 1;
            recoveryBank->name = strdup("Recovery");
            recoveryBank->autoBoot = mSettingsCurrent.autoBootOther == (uint8_t)autoBootOtherRecovery;
            recoveryBank->category = bankCategoryRecovery;
            bankInfos->add(recoveryBank);
        }

        if (context::getModchipType() == modchipTypeModxo && driveManager::mountDrive("SDCARD")) {
            bankDetails* sdCardBank = new bankDetails();
            sdCardBank->id = (uint8_t)autoBootOtherSdCard;
            sdCardBank->ledColor = 0;
            sdCardBank->slots = 1;
            sdCardBank->name = strdup("SDCARD");
            sdCardBank->autoBoot = mSettingsCurrent.autoBootOther == (uint8_t)autoBootOtherSdCard;
            sdCardBank->category = bankCategorySdCard;
            bankInfos->add(sdCardBank);
        }
    }

    return bankInfos;
}

char* settingsManager::getBankInfosJson() {
    pointerVector<bankDetails*>* bankInfos = getBankInfos(true);

    char* bankInfoJson = strdup("[");
    for (uint32_t i = 0; i < bankInfos->count(); i++) {
        bankDetails* bank = bankInfos->get(i);
        if (bank->slots > 0) {
            char* temp = stringUtility::formatString("%s%s{\"id\":%i,\"name\":\"%s\",\"slots\":%i,\"category\":%i,\"autoBoot\":%s}",
                bankInfoJson, i == 0 ? "" : ",", bank->id, bank->name, bank->slots, bank->category,
                bank->autoBoot ? "true" : "false");
            free(bankInfoJson);
            bankInfoJson = temp;
        }
    }

    char* result = stringUtility::formatString("%s]", bankInfoJson);
    free(bankInfoJson);

    delete (bankInfos);
    return result;
}

void settingsManager::toggleAutoBootBank(uint8_t id, bool isOther) {
    for (int i = 0; i < MAX_SLOTS; i++) {
        if (i == id && isOther == false) {
            mSettingsShadow.banks[i].autoBoot = !mSettingsShadow.banks[i].autoBoot;
        } else {
            mSettingsShadow.banks[i].autoBoot = false;
        }
    }
    if (isOther) {
        mSettingsShadow.autoBootOther = mSettingsShadow.autoBootOther == (uint8_t)autoBootOtherNone ? id : (uint8_t)autoBootOtherNone;
    } else {
        mSettingsShadow.autoBootOther = (uint8_t)autoBootOtherNone;
    }
    saveSettings();
}

bool settingsManager::hasAutoBootBank() {
    for (int i = 0; i < MAX_SLOTS; i++) {
        if (mSettingsCurrent.banks[i].slots > 0 && mSettingsCurrent.banks[i].autoBoot) {
            return true;
        }
    }
    return mSettingsCurrent.autoBootOther != (uint8_t)autoBootOtherNone;
}

autoBootOtherEnum settingsManager::getAutoBootOther() {
    return (autoBootOtherEnum)mSettingsCurrent.autoBootOther;
}

void settingsManager::deleteBank(uint8_t id) {
    if (id >= MAX_SLOTS) {
        return;
    }
    memset(&mSettingsShadow.banks[id], 0, sizeof(bankInfo));
    saveSettings();
}

namespace {

// A bank is "slots" consecutive 256K slots and always starts on a slot index that is a
// multiple of its own size, so any 1MB region of flash can hold 4 x 256K, 2 x 512K,
// 1 x 512K + 2 x 256K, 1 x 1024K and so on.

void buildSlotMap(uint8_t slotCount, uint8_t* slotMap) {
    memset(slotMap, 0, MAX_SLOTS);
    for (uint8_t bank = 0; bank < slotCount; bank++) {
        uint8_t slots = mSettingsCurrent.banks[bank].slots;
        for (uint8_t i = 0; i < slots && (uint8_t)(bank + i) < slotCount; i++) {
            slotMap[bank + i] = 1;
        }
    }
}

bool isRegionFree(const uint8_t* slotMap, uint8_t slotCount, uint8_t start, uint8_t slots) {
    if (slots == 0 || start + slots > slotCount) {
        return false;
    }
    for (uint8_t i = 0; i < slots; i++) {
        if (slotMap[start + i] != 0) {
            return false;
        }
    }
    return true;
}

// Highest aligned slot index that can hold a bank of "slots" size.
bool tryGetHighestFreeSlot(const uint8_t* slotMap, uint8_t slotCount, uint8_t slots, uint8_t& start) {
    if (slots == 0 || slots > slotCount) {
        return false;
    }
    uint8_t position = (uint8_t)(((slotCount - slots) / slots) * slots);
    while (true) {
        if (isRegionFree(slotMap, slotCount, position, slots) == true) {
            start = position;
            return true;
        }
        if (position < slots) {
            return false;
        }
        position = (uint8_t)(position - slots);
    }
}

// Swaps a single bank up into free flash. Only one bank is ever in ram at a time, and it
// is the same read/erase/write a normal flash of that bank would do.
void moveBank(uint8_t sourceId, uint8_t destId, uint8_t slots) {
    uint8_t sourceBank = context::getModchip()->getBankFromIdAndSlots(sourceId, slots);
    uint8_t destBank = context::getModchip()->getBankFromIdAndSlots(destId, slots);

    utils::dataContainer* bankData = context::getModchip()->readBank(sourceBank, false);
    context::getModchip()->eraseBank(destBank, false);
    context::getModchip()->writeBank(destBank, bankData, false);
    delete (bankData);

    memcpy(&mSettingsShadow.banks[destId], &mSettingsShadow.banks[sourceId], sizeof(bankInfo));
    memset(&mSettingsShadow.banks[sourceId], 0, sizeof(bankInfo));
    settingsManager::saveSettings();

    utils::debugPrint("Moved bank %i to %i (%i slots)\n", sourceId, destId, slots);
}

} // namespace

void settingsManager::optimizeBanks(uint8_t slotsNeeded) {
    uint8_t slotCount = (uint8_t)context::getModchip()->getSlotCount();
    if (slotCount > MAX_SLOTS) {
        slotCount = MAX_SLOTS;
    }

    // A 256K bank fits any free slot, so there is never anything to gain by moving things.
    if (slotsNeeded < 2 || slotsNeeded > slotCount) {
        utils::debugPrint("No need to optimize for %i slot(s)\n", slotsNeeded);
        return;
    }

    if (getFreeSlots() < slotsNeeded) {
        utils::debugPrint("Not enough free slots to be worth optimizing\n");
        return;
    }

    // Only shuffle anything about if the bank we are flashing does not already fit.
    uint8_t bankId = 0;
    if (tryGetFreeBank(slotsNeeded, bankId) == true) {
        utils::debugPrint("Already have room for %i slot(s) at bank %i\n", slotsNeeded, bankId);
        return;
    }

    // Shuffle banks up towards the top of flash, 256K first then 512K then 1024K, so the
    // free space is left as a run at the bottom of flash where the alignment the bigger
    // banks need lives. A bank only ever moves into flash that is already free, so no more
    // than one bank is in ram at any point and there is nothing to unwind if a move fails.
    // Every move raises a bank's slot index, so the passes always run out of work.
    uint8_t slotMap[MAX_SLOTS];

    for (uint8_t slots = 1; slots <= slotCount; slots = (uint8_t)(slots << 1)) {
        bool moved = true;
        while (moved == true) {
            moved = false;

            // Walk from the top down so the banks nearest the top settle first.
            uint8_t bank = slotCount;
            while (bank > 0) {
                bank--;
                if (mSettingsCurrent.banks[bank].slots != slots) {
                    continue;
                }

                buildSlotMap(slotCount, slotMap);
                uint8_t destId = 0;
                if (tryGetHighestFreeSlot(slotMap, slotCount, slots, destId) == false) {
                    break;
                }
                if (destId <= bank) {
                    continue;
                }

                moveBank(bank, destId, slots);
                moved = true;

                // Stop the moment the bank we are flashing has somewhere to live, so we
                // never wear the flash out tidying up space nothing has asked for.
                if (tryGetFreeBank(slotsNeeded, bankId) == true) {
                    return;
                }
            }
        }
    }

    utils::debugPrint("Unable to make room for %i slots\n", slotsNeeded);
}

bool settingsManager::tryGetLastUsedBank(uint8_t slotsNeeded, uint8_t& bankId) {
    bool found = false;
    uint8_t lastBank = 0;

    uint8_t bank = 0;
    while (bank < context::getModchip()->getSlotCount()) {
        uint8_t slotsUsed = mSettingsCurrent.banks[bank].slots;
        if (slotsUsed == slotsNeeded) {
            found = true;
            lastBank = bank;
        }
        bank += slotsNeeded;
    }

    bankId = lastBank;
    return found;
}

bool settingsManager::tryGetFreeBank(uint8_t slotsNeeded, uint8_t& bankId) {
    uint32_t slotCount = context::getModchip()->getSlotCount();
    if (slotCount > MAX_SLOTS) {
        slotCount = MAX_SLOTS;
    }

    if (slotsNeeded == 0 || slotsNeeded > slotCount) {
        return false;
    }

    uint32_t freeSlots = getFreeSlots();

    if (freeSlots < slotsNeeded) {
        utils::debugPrint("Not enough free slots\n");
        return false;
    }

    // A bank always starts on a slot index that is a multiple of its own size.
    uint32_t bank = 0;
    while (bank + slotsNeeded <= slotCount) {
        uint32_t slotsToSkip = slotsNeeded;
        bool spaceFound = true;
        for (uint32_t i = 0; i < slotsNeeded; i++) {
            uint8_t slotsUsed = mSettingsCurrent.banks[bank + i].slots;
            if (slotsUsed == 0) {
                continue;
            }
            slotsToSkip = max(slotsUsed, slotsNeeded);
            spaceFound = false;
            break;
        }
        if (spaceFound == true) {
            bankId = (uint8_t)bank;
            return true;
        }
        bank += slotsToSkip;
    }

    return false;
}

void settingsManager::eraseBank(uint8_t bankId, uint32_t size) {
    uint8_t slotsNeeded = (uint8_t)(size >> 18);
    uint8_t bank = context::getModchip()->getBankFromIdAndSlots(bankId, slotsNeeded);
    context::getModchip()->eraseBank(bank, false);
}

void settingsManager::writeBank(
    uint8_t bankId, utils::dataContainer* dataContainer, const char* name, uint8_t ledColor) {
    uint8_t slotsNeeded = (uint8_t)(dataContainer->size >> 18);
    uint8_t bank = context::getModchip()->getBankFromIdAndSlots(bankId, slotsNeeded);
    context::getModchip()->writeBank(bank, dataContainer, false);
    mSettingsShadow.banks[bankId].ledColor = ledColor;
    mSettingsShadow.banks[bankId].slots = slotsNeeded;
    char* nameCopy = strdup(name);
    stringUtility::copyString((char*)&mSettingsShadow.banks[bankId].name[0], nameCopy, 40);
    free(nameCopy);
    saveSettings();
}

bool settingsManager::verifyBank(uint8_t bankId, utils::dataContainer* dataContainer) {
    uint8_t slotsNeeded = (uint8_t)(dataContainer->size >> 18);
    uint8_t bank = context::getModchip()->getBankFromIdAndSlots(bankId, slotsNeeded);
    bool result = context::getModchip()->verifyBank(bank, dataContainer, false);
    return result;
}

utils::dataContainer* settingsManager::readBank(uint8_t bankId) {
    if (bankId >= MAX_SLOTS) {
        return NULL;
    }
    uint8_t slots = mSettingsCurrent.banks[bankId].slots;
    uint8_t bank = context::getModchip()->getBankFromIdAndSlots(bankId, slots);
    utils::dataContainer* result = context::getModchip()->readBank(bank, false);
    return result;
}

void settingsManager::launchBank(uint8_t bankId, bool safeMode) {
    if (bankId >= MAX_SLOTS) {
        return;
    }
    lcdRender::waitStop();
    bankInfo bankInfo = settingsManager::getBankInfo(bankId);
    if (bankInfo.slots > 0) {
        uint8_t bank = context::getModchip()->getBankFromIdAndSlots(bankId, bankInfo.slots);
        context::getModchip()->launchBank(bank, bankInfo.ledColor, safeMode);
    }
}

void settingsManager::launchTsop(bool safeMode) {
    lcdRender::waitStop();
    context::getModchip()->launchTsop(safeMode);
}

void settingsManager::launchRecovery(bool safeMode) {
    lcdRender::waitStop();
    context::getModchip()->launchRecovery(safeMode);
}

void settingsManager::launchSdCard(bool safeMode) {
    lcdRender::waitStop();

    //Todo add to modchip
    char* launchPath = settingsManager::getSdCardLaunchPath();
    if (launchPath == NULL || launchPath[0] == '\0') {
        free(launchPath);
        return;
    }
    char* sdPath = utils::toSdCardLpcPath(launchPath);
    free(launchPath);
    if (sdPath != NULL) {
        sdcard::open();
        bool result = sdcard::open_file_from_path(sdPath);
        if (result) {
            sdcard::map_bios_to_mem();
            utils::prepareScratch(safeMode);
            HalReturnToFirmware(RETURN_FIRMWARE_REBOOT);
        } else {
            sdcard::close();
        }
        free(sdPath);
    }

}

void settingsManager::editBank(uint8_t bankId, const char* name, uint8_t ledColor) {
    mSettingsShadow.banks[bankId].ledColor = ledColor;
    char* nameCopy = strdup(name);
    stringUtility::copyString((char*)&mSettingsShadow.banks[bankId].name[0], nameCopy, 40);
    free(nameCopy);

    saveSettings();
}

networkModeEnum settingsManager::getNetworkMode() {
    return (networkModeEnum)mSettingsCurrent.network.networkMode;
}

uint32_t settingsManager::getNetworkAddress() {
    return mSettingsCurrent.network.address;
}

uint32_t settingsManager::getNetworkSubnet() {
    return mSettingsCurrent.network.subnet;
}

uint32_t settingsManager::getNetworkGateway() {
    return mSettingsCurrent.network.gateway;
}

uint32_t settingsManager::getNetworkPrimaryDns() {
    return mSettingsCurrent.network.primaryDns;
}

uint32_t settingsManager::getNetworkSecondaryDns() {
    return mSettingsCurrent.network.secondaryDns;
}

void settingsManager::setNetwork(networkModeEnum networkMode, uint32_t address, uint32_t subnet, uint32_t gateway,
    uint32_t primaryDns, uint32_t secondaryDns) {
    mSettingsShadow.network.networkMode = (uint32_t)networkMode;
    mSettingsShadow.network.address = address;
    mSettingsShadow.network.address = address;
    mSettingsShadow.network.subnet = subnet;
    mSettingsShadow.network.gateway = gateway;
    mSettingsShadow.network.primaryDns = primaryDns;
    mSettingsShadow.network.secondaryDns = secondaryDns;
    saveSettings();
}

char* settingsManager::getSkinName() {
    return strdup(&mSettingsCurrent.skinName[0]);
}

void settingsManager::setSkinName(const char* skinName) {
    memset(&mSettingsShadow.skinName[0], 0, sizeof(mSettingsShadow.skinName));
    strncpy(&mSettingsShadow.skinName[0], skinName, 50);
    saveSettings();
}

char* settingsManager::getSoundPackName() {
    return strdup(&mSettingsCurrent.soundPackName[0]);
}

void settingsManager::setSoundPackName(const char* soundPackName) {
    memset(&mSettingsShadow.soundPackName[0], 0, sizeof(mSettingsShadow.soundPackName));
    strncpy(&mSettingsShadow.soundPackName[0], soundPackName, 50);
    saveSettings();
}

uint32_t settingsManager::getSnakeHiScore() {
    return mSettingsCurrent.snakeHiScore;
}

void settingsManager::setSnakeHiScore(uint32_t playerScore) {
    mSettingsShadow.snakeHiScore = playerScore;
    saveSettings();
}

uint32_t settingsManager::getInvadersHiScore() {
    return mSettingsCurrent.invadersHiScore;
}

void settingsManager::setInvadersHiScore(uint32_t playerScore) {
    mSettingsShadow.invadersHiScore = playerScore;
    saveSettings();
}

uint8_t settingsManager::getAutoBootDelay() {
    return mSettingsCurrent.autoBootDelay;
}

void settingsManager::setAutoBootDelay(uint8_t autoBootDelay) {
    mSettingsShadow.autoBootDelay = autoBootDelay;
    saveSettings();
}

uint8_t settingsManager::getMusicVolume() {
    return mSettingsCurrent.musicVolume;
}

void settingsManager::setMusicVolume(uint8_t volume) {
    mSettingsShadow.musicVolume = volume;
    saveSettings();
}

uint8_t settingsManager::getSoundVolume() {
    return mSettingsCurrent.soundVolume;
}

void settingsManager::setSoundVolume(uint8_t volume) {
    mSettingsShadow.soundVolume = volume;
    saveSettings();
}

uint8_t settingsManager::getMinFanSpeed() {
    return mSettingsCurrent.minFanSpeed;
}

void settingsManager::setMinFanSpeed(uint8_t minFanSpeed) {
    mSettingsShadow.minFanSpeed = minFanSpeed;
    saveSettings();
}

uint8_t settingsManager::getLedColor() {
    return mSettingsCurrent.ledColor;
}

void settingsManager::setLedColor(uint8_t ledColor) {
    mSettingsShadow.ledColor = ledColor;
    saveSettings();
}

uint8_t settingsManager::getLcdMode(bool current) {
    return current ? mSettingsCurrent.lcdMode : mSettingsShadow.lcdMode;
}

void settingsManager::setLcdMode(uint8_t lcdMode) {
    mSettingsShadow.lcdMode = lcdMode;
}

uint8_t settingsManager::getLcdModel(bool current) {
    return current ? mSettingsCurrent.lcdModel : mSettingsShadow.lcdModel;
}

void settingsManager::setLcdModel(uint8_t lcdModel) {
    mSettingsShadow.lcdModel = lcdModel;
}

uint8_t settingsManager::getLcdAddress(bool current) {
    return current ? mSettingsCurrent.lcdAddress : mSettingsShadow.lcdAddress;
}

void settingsManager::setLcdAddress(uint8_t lcdAddress) {
    mSettingsShadow.lcdAddress = lcdAddress;
}

uint8_t settingsManager::getLcdBacklight(bool current) {
    return current ? mSettingsCurrent.lcdBacklight : mSettingsShadow.lcdBacklight;
}

void settingsManager::setLcdBacklight(uint8_t backlight) {
    mSettingsShadow.lcdBacklight = backlight;
}

uint8_t settingsManager::getLcdContrast(bool current) {
    return current ? mSettingsCurrent.lcdContrast : mSettingsShadow.lcdContrast;
}

void settingsManager::setLcdContrast(uint8_t contrast) {
    mSettingsShadow.lcdContrast = contrast;
}

bool settingsManager::getRtcEnable() {
    return mSettingsCurrent.rtcEnable == 1;
}

void settingsManager::setRtcEnable(bool enable) {
    mSettingsShadow.rtcEnable = enable ? 1 : 0;
    saveSettings();
}

uint8_t settingsManager::getDriveSetup() {
    return mSettingsCurrent.driveSetup;
}

void settingsManager::setDriveSetup(uint8_t driveSetup) {
    mSettingsShadow.driveSetup = driveSetup;
    saveSettings();
}

uint8_t settingsManager::getUdmaMode(bool master) {
    return master ? mSettingsCurrent.udmaModeMaster : mSettingsCurrent.udmaModeSlave;
}

void settingsManager::setUdmaMode(uint8_t udmaMode, bool master) {
    if (master) {
        mSettingsShadow.udmaModeMaster = udmaMode;
    } else {
        mSettingsShadow.udmaModeSlave = udmaMode;
    }
    saveSettings();
}

uint8_t settingsManager::getSplashDelay() {
    return mSettingsCurrent.splashDelay;
}

void settingsManager::setSplashDelay(uint8_t splashDelay) {
    mSettingsShadow.splashDelay = splashDelay;
    saveSettings();
}

bool settingsManager::getVgaEnable() {
    return mSettingsCurrent.vgaEnable == 1;
}

void settingsManager::setVgaEnable(bool enable) {
    mSettingsShadow.vgaEnable = enable ? 1 : 0;
    saveSettings();
}

char* settingsManager::getSdCardLaunchPath() {
    return strdup(&mSettingsCurrent.sdCardLaunchPath[0]);
}

void settingsManager::setSdCardLaunchPath(const char* sdCardLaunchPath) {
    memset(&mSettingsShadow.sdCardLaunchPath[0], 0, sizeof(mSettingsShadow.sdCardLaunchPath));
    if (sdCardLaunchPath != NULL) {
        strncpy(&mSettingsShadow.sdCardLaunchPath[0], sdCardLaunchPath, sizeof(mSettingsShadow.sdCardLaunchPath) - 1);
    }
    saveSettings();
}
