#include "modchipSmartxxOpx.h"
#include "crc32.h"
#include "settingsManager.h"
#include "globalDefines.h"
#include "stringUtility.h"
#include "fileSystem.h"

//Pattern repeats every 0x40 banks

//id     start      end             k   mapping
//------------------------------------------------------
//  0x00  0x1C0000   0x1FFFFF      256  Bank_Bootloader + has xor
//  0x01  0x180000   0x1BFFFF      256  PrometheOS3 + has xor
//  0x02  0x140000   0x17FFFF      256  PrometheOS2
//  0x03  0x100000   0x13FFFF      256  Settings + PrometheOS1
//  0x04  0x0C0000   0x0FFFFF      256  Bank4_256k
//  0x05  0x080000   0x0BFFFF      256  Bank3_256k
//  0x06  0x040000   0x07FFFF      256  Bank2_256k
//  0x07  0x000000   0x03FFFF      256  Bank1_256k
//  0x08  0x000000   0x07FFFF      512  Bank1_512k
//  0x09  0x080000   0x0FFFFF      512  Bank2_512k
//  0x0A  0x100000   0x17FFFF      512
//  0x0B  0x000000   0x0FFFFF     1024  Bank1_1024k
//  0x20  0x000000   0x1FFFFF     2048

//  Repates evert 0x40, missing are 0x1C0000 - 0x1FFFFF

//  0x000000 - 0x00FFFF  Sector Size 8192
//  0x200000 - 0x3FFFFF  Sector Size 65536

//  0x190000 - 0x1BFFFF  Xor'ed with 0xD7
//  0x1D0000 - 0x1FFFFF  Xor'ed with 0xD7

//  0x100000 - 0x10FFFF  Settings

modchipSmartxxOpx::modchipSmartxxOpx() {
    mBank = SMARTXXOPX_BANK_BOOTLOADER;
    mSysConVer = inputByte(0xf701) & 0x0f;
    mSequenceAddressMangled = utils::smartxxMangleFlashAddress(0x555);
    mSequenceCommandMangled = utils::smartxxMangleCommand(0x55);
    mUnlockCycleAddressMangled = utils::smartxxMangleFlashAddress(0xAAA);
    mUnlockCycleCommandMangled = utils::smartxxMangleCommand(0xAA);
}

void modchipSmartxxOpx::setLedColor(uint8_t ledColor) {
    // outputByte(SMARTXXOPX_REGISTER_LED, ledColor);
}

char* modchipSmartxxOpx::getName() {
    return strdup("SmartXX");
}

uint32_t modchipSmartxxOpx::getSlotCount() {
    return 4;
}

uint32_t modchipSmartxxOpx::getFlashSize(bool recovery) {
    return recovery ? 0 : (2 * 1024 * 1024);
}

void modchipSmartxxOpx::disableRecovery() {
}

bool modchipSmartxxOpx::isValidBankSize(uint32_t size) {
    return size == (256 * 1024) || size == (512 * 1024) || size == (1024 * 1024);
}

bool modchipSmartxxOpx::isValidFlashSize(bool recovery, uint32_t size) {
    return size == getFlashSize(recovery);
}

uint32_t modchipSmartxxOpx::getBankSize(uint8_t bank) {
    if (bank == SMARTXXOPX_BANK_SLOT1_256K) {
        return 256 * 1024;
    }
    if (bank == SMARTXXOPX_BANK_SLOT2_256K) {
        return 256 * 1024;
    }
    if (bank == SMARTXXOPX_BANK_SLOT3_256K) {
        return 256 * 1024;
    }
    if (bank == SMARTXXOPX_BANK_SLOT4_256K) {
        return 256 * 1024;
    }
    if (bank == SMARTXXOPX_BANK_SLOT1_512K) {
        return 512 * 1024;
    }
    if (bank == SMARTXXOPX_BANK_SLOT2_512K) {
        return 512 * 1024;
    }
    if (bank == SMARTXXOPX_BANK_SLOT1_1024K) {
        return 1024 * 1024;
    }
    if (bank == SMARTXXOPX_BANK_PROMETHEOS1) {
        return 256 * 1024;
    }
    if (bank == SMARTXXOPX_BANK_PROMETHEOS2) {
        return 256 * 1024;
    }
    if (bank == SMARTXXOPX_BANK_PROMETHEOS3) {
        return 256 * 1024;
    }
    if (bank == SMARTXXOPX_BANK_BOOTLOADER) {
        return 256 * 1024;
    }
    return 0;
}

uint32_t modchipSmartxxOpx::getBankMemOffset(uint8_t bank) {
    if (bank == SMARTXXOPX_BANK_SLOT1_256K) {
        return 0x000000;
    }
    if (bank == SMARTXXOPX_BANK_SLOT2_256K) {
        return 0x040000;
    }
    if (bank == SMARTXXOPX_BANK_SLOT3_256K) {
        return 0x080000;
    }
    if (bank == SMARTXXOPX_BANK_SLOT4_256K) {
        return 0x0c0000;
    }
    if (bank == SMARTXXOPX_BANK_SLOT1_512K) {
        return 0x000000;
    }
    if (bank == SMARTXXOPX_BANK_SLOT2_512K) {
        return 0x080000;
    }
    if (bank == SMARTXXOPX_BANK_SLOT1_1024K) {
        return 0x000000;
    }
    if (bank == SMARTXXOPX_BANK_PROMETHEOS1) {
        return 0x100000;
    }
    if (bank == SMARTXXOPX_BANK_PROMETHEOS2) {
        return 0x140000;
    }
    if (bank == SMARTXXOPX_BANK_PROMETHEOS3) {
        return 0x180000;
    }
    if (bank == SMARTXXOPX_BANK_BOOTLOADER) {
        return 0x1c0000;
    }
    return 0;
}

uint32_t modchipSmartxxOpx::getBankStartOffset(uint8_t bank) {
    return 0;
}

uint8_t modchipSmartxxOpx::getBankFromIdAndSlots(uint8_t id, uint8_t slots) {
    if (id == 0 && slots == 1) {
        return SMARTXXOPX_BANK_SLOT1_256K;
    }
    if (id == 1 && slots == 1) {
        return SMARTXXOPX_BANK_SLOT2_256K;
    }
    if (id == 2 && slots == 1) {
        return SMARTXXOPX_BANK_SLOT3_256K;
    }
    if (id == 3 && slots == 1) {
        return SMARTXXOPX_BANK_SLOT4_256K;
    }
    if (id == 0 && slots == 2) {
        return SMARTXXOPX_BANK_SLOT1_512K;
    }
    if (id == 2 && slots == 2) {
        return SMARTXXOPX_BANK_SLOT2_512K;
    }
    if (id == 0 && slots == 4) {
        return SMARTXXOPX_BANK_SLOT1_1024K;
    }
    return 0;
}

utils::dataContainer* modchipSmartxxOpx::readBank(uint8_t bank, bool is_backup) {
    setBank(SMARTXXOPX_BANK_SLOT1_2048K);
    uint32_t bankSize = getBankSize(bank);
    utils::dataContainer* dataContainer = new utils::dataContainer(bankSize);
    volatile uint8_t* lpcMemMap = (volatile uint8_t*)LPC_MEMORY_BASE;
    memcpy(dataContainer->data, (void*)&lpcMemMap[getBankMemOffset(bank)], bankSize);

    if (is_backup && (bank == SMARTXXOPX_BANK_BOOTLOADER || bank == SMARTXXOPX_BANK_PROMETHEOS3)) {
        uint32_t bankSize = getBankSize(bank);
        for (uint32_t i = 65536; i < bankSize; i++)
        {
            dataContainer->data[i] = dataContainer->data[i] ^ 0xD7;
        }
    }

    setBank(SMARTXXOPX_BANK_BOOTLOADER);
    return dataContainer;
}

void modchipSmartxxOpx::eraseBank(uint8_t bank, bool isUpdate) {
    setBank(SMARTXXOPX_BANK_SLOT1_2048K);

    setLedColor(LED_COLOR_AMBER);

    volatile uint8_t* lpcMemMap = (volatile uint8_t*)LPC_MEMORY_BASE;

    uint32_t memOffset = getBankMemOffset(bank);
    uint32_t bankSize = getBankSize(bank);

    uint32_t offset = 0;
    while (offset < bankSize) {
        uint32_t currentOffset = memOffset + offset;
        if (!(isUpdate && isSettingsMemOffset(currentOffset)) && isEraseMemOffset(currentOffset)) {
            sectorErase(memOffset + offset);
        }
        offset += 8192;
    }

    setBank(SMARTXXOPX_BANK_BOOTLOADER);

#ifndef TOOLS
    setLedColor(settingsManager::getLedColor());
#else
    setLedColor(LED_COLOR_GREEN);
#endif
}

void modchipSmartxxOpx::writeBank(uint8_t bank, utils::dataContainer* dataContainer, bool isUpdate) {
    setBank(SMARTXXOPX_BANK_SLOT1_2048K);

    setLedColor(LED_COLOR_BLUE);

    volatile uint8_t* lpcMemMap = (volatile uint8_t*)LPC_MEMORY_BASE;

    uint32_t memOffset = getBankMemOffset(bank);

    outputByte(SMARTXXOPX_REGISTER_FLASH_ENABLE, 1);
    for (uint32_t i = 0; i < dataContainer->size; i++) {
        if ((isUpdate && isSettingsMemOffset(memOffset + i)) || isProtectedMemOffset(memOffset + i) == true) {
            continue;
        }
        uint32_t flashOffset = memOffset + i;
        uint8_t value = (uint8_t)dataContainer->data[i];
        if ((bank == SMARTXXOPX_BANK_BOOTLOADER || bank == SMARTXXOPX_BANK_PROMETHEOS3) && i >= 65536) {
            value = value ^ 0xD7;
        }
        lpcMemMap[mUnlockCycleAddressMangled] = mUnlockCycleCommandMangled;
        lpcMemMap[mSequenceAddressMangled] = mSequenceCommandMangled;
        lpcMemMap[mUnlockCycleAddressMangled] = utils::smartxxMangleCommand(0xA0);
        lpcMemMap[flashOffset] = value;
        while (lpcMemMap[flashOffset] != value)
            ;
    }
    lpcMemMap[0] = utils::smartxxMangleCommand(0xF0);
    outputByte(SMARTXXOPX_REGISTER_FLASH_ENABLE, 0);

    setBank(SMARTXXOPX_BANK_BOOTLOADER);

#ifndef TOOLS
    setLedColor(settingsManager::getLedColor());
#else
    setLedColor(LED_COLOR_GREEN);
#endif
}

bool modchipSmartxxOpx::verifyBank(uint8_t bank, utils::dataContainer* dataContainer, bool isUpdate) {
    setBank(SMARTXXOPX_BANK_SLOT1_2048K);

    utils::dataContainer* writtenData = readBank(bank, false);

    setLedColor(LED_COLOR_PURPLE);

    volatile uint8_t* lpcMemMap = (volatile uint8_t*)LPC_MEMORY_BASE;

    uint32_t memOffset = getBankMemOffset(bank);

    bool ok = true;
    for (uint32_t i = 0; i < dataContainer->size; i++) {
        if ((isUpdate && isSettingsMemOffset(memOffset + i)) || isProtectedMemOffset(memOffset + i) == true) {
            continue;
        }
        uint8_t value = (uint8_t)dataContainer->data[i];
        if ((bank == SMARTXXOPX_BANK_BOOTLOADER || bank == SMARTXXOPX_BANK_PROMETHEOS3) && i >= 65536) {
            value = value ^ 0xD7;
        }
        if ((uint8_t)writtenData->data[i] == value) {
            continue;
        }
        ok = false;
        break;
    }

    delete (writtenData);

    setBank(SMARTXXOPX_BANK_BOOTLOADER);

#ifndef TOOLS
    setLedColor(settingsManager::getLedColor());
#else
    setLedColor(LED_COLOR_GREEN);
#endif
    return ok;
}

uint8_t modchipSmartxxOpx::getFlashBankCount(bool recovery) {
    return recovery ? 0 : 8;
}

uint8_t modchipSmartxxOpx::getFlashBank(bool recovery, uint8_t bank) {
    const uint8_t banks[] = {
        SMARTXXOPX_BANK_SLOT1_256K,
        SMARTXXOPX_BANK_SLOT2_256K,
        SMARTXXOPX_BANK_SLOT3_256K,
        SMARTXXOPX_BANK_SLOT4_256K,
        SMARTXXOPX_BANK_PROMETHEOS1,
        SMARTXXOPX_BANK_PROMETHEOS2,
        SMARTXXOPX_BANK_PROMETHEOS3,
        SMARTXXOPX_BANK_BOOTLOADER,
    };
    return recovery ? 0 : banks[bank];
}

bankType modchipSmartxxOpx::getFlashBankType(bool recovery, uint8_t bank) {
    const bankType banks[] = {
        bankTypeUser,
        bankTypeUser,
        bankTypeUser,
        bankTypeUser,
        bankTypeSystem,
        bankTypeSystem,
        bankTypeSystem,
        bankTypeSystem,
    };
    return recovery ? bankTypeUser : banks[bank];
}

utils::dataContainer* modchipSmartxxOpx::readFlash(bool recovery) {
    utils::dataContainer* result = new utils::dataContainer(getFlashSize(recovery));
    for (uint8_t i = 0; i < getFlashBankCount(recovery); i++) {
        uint8_t bank = getFlashBank(recovery, i);
        uint32_t memOffset = getBankMemOffset(bank);
        utils::dataContainer* bankData = readBank(bank, true);
        memcpy(result->data + memOffset, bankData->data, bankData->size);
        delete (bankData);
    }
    return result;
}

namespace {

void readSmartxxFlashIds(volatile uint8_t* lpcMemMap, uint32_t unlockCycleAddressMangled, uint8_t unlockCycleCommandMangled,
    uint32_t sequenceAddressMangled, uint8_t sequenceCommandMangled, uint8_t& manuf, uint8_t& devid) {
    utils::ioOutputByte(SMARTXXOPX_REGISTER_FLASH_ENABLE, 1);

    lpcMemMap[unlockCycleAddressMangled] = unlockCycleCommandMangled;
    lpcMemMap[sequenceAddressMangled] = sequenceCommandMangled;
    lpcMemMap[unlockCycleAddressMangled] = utils::smartxxMangleCommand(0x90);
    manuf = utils::smartxxUnmangleCommandResponse(lpcMemMap[0]);
    devid = utils::smartxxUnmangleCommandResponse(lpcMemMap[2]);
    lpcMemMap[0] = utils::smartxxMangleCommand(0xF0);

    utils::ioOutputByte(SMARTXXOPX_REGISTER_FLASH_ENABLE, 0);
}

} // namespace

uint8_t modchipSmartxxOpx::getFlashManufacturerId() {
    volatile uint8_t* lpcMemMap = (volatile uint8_t*)LPC_MEMORY_BASE;
    uint8_t manuf = 0;
    uint8_t devid = 0;
    readSmartxxFlashIds(lpcMemMap, mUnlockCycleAddressMangled, mUnlockCycleCommandMangled, mSequenceAddressMangled,
        mSequenceCommandMangled, manuf, devid);
    return manuf;
}

uint8_t modchipSmartxxOpx::getFlashDeviceId() {
    volatile uint8_t* lpcMemMap = (volatile uint8_t*)LPC_MEMORY_BASE;
    uint8_t manuf = 0;
    uint8_t devid = 0;
    readSmartxxFlashIds(lpcMemMap, mUnlockCycleAddressMangled, mUnlockCycleCommandMangled, mSequenceAddressMangled,
        mSequenceCommandMangled, manuf, devid);
    return devid;
}

uint8_t modchipSmartxxOpx::getRecoveryFlashManufacturerId() {
    return 0;
}

uint8_t modchipSmartxxOpx::getRecoveryFlashDeviceId() {
    return 0;
}

void modchipSmartxxOpx::launchBank(uint8_t bank, uint8_t ledColor, bool safeMode) {
    utils::prepareScratch(safeMode);  
    setBank(bank);
    setLedColor(ledColor);
    HalReturnToFirmware(RETURN_FIRMWARE_REBOOT);
}

void modchipSmartxxOpx::launchTsop(bool safeMode) {
    utils::prepareScratch(safeMode);  

    outputByte(0xf705, 1);

    setLedColor(LED_COLOR_OFF);
    HalReturnToFirmware(RETURN_FIRMWARE_REBOOT);
}

void modchipSmartxxOpx::launchRecovery(bool safeMode) {
}

#ifndef TOOLS

void modchipSmartxxOpx::loadSettings(settingsState& settings) {
    setBank(SMARTXXOPX_SETTINGS_BANK);

    setLedColor(LED_COLOR_WHITE);

    volatile uint8_t* lpcMemMap = (volatile uint8_t*)LPC_MEMORY_BASE;

    uint8_t* settingsData = (uint8_t*)&settings;
    memcpy(settingsData, (void*)&lpcMemMap[SMARTXXOPX_SETTINGS_OFFSET], sizeof(settings));
    uint32_t checksum = crc32::calculate(((uint8_t*)&settings) + sizeof(uint32_t), sizeof(settings) - sizeof(uint32_t));

    setBank(SMARTXXOPX_BANK_BOOTLOADER);

    if (checksum != settings.checksum ||
        versioning::compareVersion(settings.version, settingsManager::getVersion()) != 0) {
        settingsManager::initSettings();
        settingsManager::saveSettings();
    }

    setLedColor(settingsManager::getLedColor());
}

void modchipSmartxxOpx::saveSettings(settingsState& settings) {
    setBank(SMARTXXOPX_SETTINGS_BANK);

    settings.checksum = crc32::calculate(((uint8_t*)&settings) + sizeof(uint32_t), sizeof(settings) - sizeof(uint32_t));
    utils::dataContainer* settingsData = new utils::dataContainer((char*)&settings, sizeof(settings), sizeof(settings));

    volatile uint8_t* lpcMemMap = (volatile uint8_t*)LPC_MEMORY_BASE;

    setLedColor(LED_COLOR_AMBER);
    sectorErase(SMARTXXOPX_SETTINGS_OFFSET);

    setLedColor(LED_COLOR_BLUE);

    outputByte(SMARTXXOPX_REGISTER_FLASH_ENABLE, 1);
    for (uint32_t i = 0; i < settingsData->size; i++) {
        uint8_t value = (uint8_t)settingsData->data[i];
        lpcMemMap[mUnlockCycleAddressMangled] = mUnlockCycleCommandMangled;
        lpcMemMap[mSequenceAddressMangled] = mSequenceCommandMangled;
        lpcMemMap[mUnlockCycleAddressMangled] = utils::smartxxMangleCommand(0xA0);
        lpcMemMap[SMARTXXOPX_SETTINGS_OFFSET + i] = value;
        while (lpcMemMap[SMARTXXOPX_SETTINGS_OFFSET + i] != value)
            ;
    }
    lpcMemMap[0] = utils::smartxxMangleCommand(0xF0);
    outputByte(SMARTXXOPX_REGISTER_FLASH_ENABLE, 0);

    delete (settingsData);

    setBank(SMARTXXOPX_BANK_BOOTLOADER);

    setLedColor(settingsManager::getLedColor());
}

#endif

displayDriver* modchipSmartxxOpx::getDisplayDriver(bool current) {
    uint8_t lcdMode = settingsManager::getLcdMode(current);
    uint8_t lcdModel = settingsManager::getLcdModel(current);
    if (lcdMode == 1) {
        if (lcdModel == 0) {
            return displayFactory::getDisplay(displayVariantHD44780Smartxx);
        }
        if (lcdModel == 1) {
            return displayFactory::getDisplay(displayVariantSPI2PARSmartxx);
        }
    }
    if (lcdMode == 2) {
        if (lcdModel == 0) {
            return displayFactory::getDisplay(displayVariantHD44780LPC);
        }
        if (lcdModel == 1) {
            return displayFactory::getDisplay(displayVariantLCDXXXXLPC);
        }
    }
    return NULL;
}

supportInfo modchipSmartxxOpx::getSupportInfo(bool current) {
    uint8_t lcdMode = settingsManager::getLcdMode(current);
    supportInfo info;
    info.supportsLed = true;
    info.supportsLcd = true;

    bool lcdInfo = false;
    bool backlight = false;
    bool contrast = false;
    displayDriver* driver = getDisplayDriver(current);
    if (driver != NULL) {
        driver->getSupport(lcdInfo, backlight, contrast);
    }
    info.supportsLcdInfo = lcdInfo;
    info.supportsLcdBacklight = backlight;
    info.supportsLcdContrast = contrast;

    info.supportsRecovery = false;
    return info;
}

uint8_t modchipSmartxxOpx::getLcdModeCount() {
    return 3;
}

char* modchipSmartxxOpx::getLcdModeString(uint8_t lcdMode) {
    if (lcdMode == 1) {
        return strdup("Smartxx");
    }
    if (lcdMode == 2) {
        return strdup("SMBUS");
    }
    return strdup("Disabled");
}

uint8_t modchipSmartxxOpx::getLcdModelCount(bool current) {
    uint8_t lcdMode = settingsManager::getLcdMode(current);
    if (lcdMode == 1) {
        return 2;
    }
    if (lcdMode == 2) {
        return 2;
    }
    return 0;
}

char* modchipSmartxxOpx::getLcdModelString(bool current, uint8_t lcdModel) {
    displayDriver* driver = getDisplayDriver(current);
    if (driver == NULL) {
        return strdup("");
    }
    return driver->getModel();
}

uint8_t modchipSmartxxOpx::getLcdAddressCount(bool current) {
    displayDriver* driver = getDisplayDriver(current);
    if (driver == NULL) {
        return 0;
    }
    return driver->getI2cAddressCount();
}

char* modchipSmartxxOpx::getLcdAddressString(bool current, uint8_t lcdAddress) {
    displayDriver* driver = getDisplayDriver(current);
    if (driver == NULL) {
        return strdup("");
    }
    uint8_t address = driver->getI2cAddress(lcdAddress);
    char* result = stringUtility::formatString("0x%2X", address);
    return result;
}

void modchipSmartxxOpx::lcdSetCursorPosition(uint8_t row, uint8_t col) {
    if (row > 3) {
        row = 3;
    }
    if (col > 19) {
        col = 19;
    }
    displayDriver* driver = getDisplayDriver(true);
    if (driver == NULL) {
        return;
    }
    driver->setAddress(settingsManager::getLcdAddress(true));
    driver->setCursorPosition(row, col);
}

void modchipSmartxxOpx::lcdInit() {
    displayDriver* driver = getDisplayDriver(true);
    if (driver == NULL) {
        return;
    }
    driver->setAddress(settingsManager::getLcdAddress(true));
    driver->init();
}

void modchipSmartxxOpx::lcdPrintMessage(const char* message) {
    displayDriver* driver = getDisplayDriver(true);
    if (driver == NULL) {
        return;
    }
    driver->setAddress(settingsManager::getLcdAddress(true));
    driver->printMessage(message);
}

void modchipSmartxxOpx::lcdSetBacklight(uint8_t value) {
    if (value > 100) {
        value = 100;
    }
    displayDriver* driver = getDisplayDriver(true);
    if (driver == NULL) {
        return;
    }
    driver->setAddress(settingsManager::getLcdAddress(true));
    driver->setBacklight(value);
}

void modchipSmartxxOpx::lcdSetContrast(uint8_t value) {
    if (value > 100) {
        value = 100;
    }
    displayDriver* driver = getDisplayDriver(true);
    if (driver == NULL) {
        return;
    }
    driver->setAddress(settingsManager::getLcdAddress(true));
    driver->setContrast(value);
}

// Private

void modchipSmartxxOpx::setBank(uint8_t bank) {
    mBank = bank;
    outputByte(SMARTXXOPX_REGISTER_BANKING, bank);
}

uint8_t modchipSmartxxOpx::getBank() {
    return mBank;
}

bool modchipSmartxxOpx::isEraseMemOffset(uint32_t memOffset) {
    if (memOffset >= 0x000000 && memOffset <= 0x003fff) {
        return (memOffset % 16384) == 0;
    }
    if (memOffset >= 0x004000 && memOffset <= 0x007fff) {
        return (memOffset % 8192) == 0;
    }
    if (memOffset >= 0x008000 && memOffset <= 0x00ffff) {
        return (memOffset % 32768) == 0;
    }
    if (memOffset >= 0x010000 && memOffset <= 0x1fffff) {
        return (memOffset % 65536) == 0;
    }
    return false;
}

bool modchipSmartxxOpx::isProtectedMemOffset(uint32_t memOffset) {
    return false;
}

bool modchipSmartxxOpx::isSettingsMemOffset(uint32_t memOffset) {
    if (memOffset >= 0x100000 && memOffset <= 0x10ffff) {
        return true;
    }
    return false;
}

void modchipSmartxxOpx::sectorErase(uint32_t offset) {
    if (isProtectedMemOffset(offset)) {
        return;
    }

    uint32_t mangledOffset = utils::smartxxMangleFlashAddress(offset);

    outputByte(SMARTXXOPX_REGISTER_FLASH_ENABLE, 1);

    volatile uint8_t* lpcMemMap = (volatile uint8_t*)LPC_MEMORY_BASE;
    lpcMemMap[mUnlockCycleAddressMangled] = mUnlockCycleCommandMangled;
    lpcMemMap[mSequenceAddressMangled] = mSequenceCommandMangled;
    lpcMemMap[mUnlockCycleAddressMangled] = utils::smartxxMangleCommand(0x80);
    lpcMemMap[mUnlockCycleAddressMangled] = mUnlockCycleCommandMangled;
    lpcMemMap[mSequenceAddressMangled] = mSequenceCommandMangled;
    lpcMemMap[mangledOffset] = utils::smartxxMangleCommand(0x30);
    while (lpcMemMap[mangledOffset] != lpcMemMap[mangledOffset])
        ;
    lpcMemMap[0] = utils::smartxxMangleCommand(0xF0);

    outputByte(SMARTXXOPX_REGISTER_FLASH_ENABLE, 0);
}
