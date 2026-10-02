#include "modchipSmartxx.h"
#include "crc32.h"
#include "settingsManager.h"
#include "globalDefines.h"
#include "stringUtility.h"
#include "fileSystem.h"

//id     start      end             k
//-------------------------------------
//  0x00  0x1C0000   0x1FFFFF      256  SMARTXX_BANK_BOOTLOADER
//  0x01  0x180000   0x1BFFFF      256  SMARTXX_BANK_SLOT7_256K
//  0x02  0x140000   0x17FFFF      256  SMARTXX_BANK_SLOT6_256K
//  0x03  0x100000   0x13FFFF      256  SMARTXX_BANK_SLOT5_256K
//  0x04  0x0C0000   0x0FFFFF      256  SMARTXX_BANK_SLOT4_256K
//  0x05  0x080000   0x0BFFFF      256  SMARTXX_BANK_SLOT3_256K
//  0x06  0x040000   0x07FFFF      256  SMARTXX_BANK_SLOT2_256K
//  0x07  0x000000   0x03FFFF      256  SMARTXX_BANK_SLOT1_256K

//  0x08  0x000000   0x07FFFF      512  SMARTXX_BANK_SLOT1_512K
//  0x09  0x080000   0x0FFFFF      512  SMARTXX_BANK_SLOT2_512K
//  0x0A  0x100000   0x17FFFF      512  SMARTXX_BANK_SLOT3_512K
//  0x0B  0x000000   0x0FFFFF     1024  SMARTXX_BANK_SLOT1_1024K
//  0x0C  0x200000   0x2FFFFF     1024
//  0x0D  0x300000   0x3FFFFF     1024
//  0x0E  0x200000   0x3FFFFF     2048
//  0x0F  0x200000   0x27FFFF      512  SMARTXX_BANK_PROMETHEOS1
//  0x10  0x280000   0x2FFFFF      512  SMARTXX_BANK_PROMETHEOS2
//  0x11  0x300000   0x37FFFF      512
//  0x12  0x380000   0x3FFFFF      512
//  0x14  0x100000   0x1FFFFF     1024
//  0x17  0x000000   0x1FFFFF     2048
//  0x19  0x000000   0x3FFFFF     4096
//  0x20  0x000000   0x3FFFFF     4096

// #define SMARTXX_BANK_TSOP 0

// Bank1_256k       =  7  0x000000 - 0x03ffff  See Below          Total  262144
// Bank2_256k       =  6  0x040000 - 0x07ffff  Sector Size 65536  Total  262144
// Bank3_256k       =  5  0x080000 - 0x0bffff  Sector Size 65536  Total  262144
// Bank4_256k       =  4  0x0c0000 - 0x0fffff  Sector Size 65536  Total  262144
// Bank5_256k       =  3  0x100000 - 0x13ffff  Sector Size 65536  Total  262144
// Bank6_256k       =  2  0x140000 - 0x17ffff  Sector Size 65536  Total  262144
// Bank7_256k       =  1  0x180000 - 0x1bffff  Sector Size 65536  Total  262144 (not part of a 512/1024)  + has xor

// Bank1_512k       =  8  0x000000 - 0x07ffff  Sector Size 65536  Total  524288
// Bank2_512k       =  9  0x080000 - 0x0fffff  Sector Size 65536  Total  524288
// Bank3_512k       = 10  0x100000 - 0x17ffff  Sector Size 65536  Total  524288 (not part of a 1024)

// Bank1_1024       = 11  0x000000 - 0x0fffff  Sector Size 65536  Total  1048576

// Bank_Bootloader  =  0  0x1c0000 - 0x1fffff  Sector Size 65536  Total  262144  + has xor
// Bank_PrometheOS1 = 15  0x200000 - 0x27ffff  Sector Size 65536  Total  524288
// Bank_PrometheOS2 = 16  0x280000 - 0x2fffff  Sector Size 65536  Total  524288
// Reserved2        = 17  0x300000 - 0x37ffff  Sector Size 65536  Total  524288
// Reserved3        = 18  0x380000 - 0x3fffff  Sector Size 65536  Total  524288

// 0x000000 - 0x00ffff  Sector Size 8192
// 0x010000 - 0x3fffff  Sector Size 65536

modchipSmartxx::modchipSmartxx() {
    mBank = SMARTXX_BANK_BOOTLOADER;
    mSysConVer = inputByte(0xf701) & 0x0f;
    mSequenceAddressMangled = utils::smartxxMangleFlashAddress(0x555);
    mSequenceCommandMangled = utils::smartxxMangleCommand(0x55);
    mUnlockCycleAddressMangled = utils::smartxxMangleFlashAddress(0xAAA);
    mUnlockCycleCommandMangled = utils::smartxxMangleCommand(0xAA);
}

void modchipSmartxx::setLedColor(uint8_t ledColor) {
    // outputByte(SMARTXX_REGISTER_LED, ledColor);
}

char* modchipSmartxx::getName() {
    return strdup("SmartXX");
}

uint32_t modchipSmartxx::getSlotCount() {
    return 7;
}

uint32_t modchipSmartxx::getFlashSize(bool recovery) {
    return recovery ? 0 : (4 * 1024 * 1024);
}

void modchipSmartxx::disableRecovery() {
}

bool modchipSmartxx::isValidBankSize(uint32_t size) {
    return size == (256 * 1024) || size == (512 * 1024) || size == (1024 * 1024);
}

bool modchipSmartxx::isValidFlashSize(bool recovery, uint32_t size) {
    return size == getFlashSize(recovery);
}

uint32_t modchipSmartxx::getBankSize(uint8_t bank) {
    if (bank == SMARTXX_BANK_SLOT1_256K) {
        return 256 * 1024;
    }
    if (bank == SMARTXX_BANK_SLOT2_256K) {
        return 256 * 1024;
    }
    if (bank == SMARTXX_BANK_SLOT3_256K) {
        return 256 * 1024;
    }
    if (bank == SMARTXX_BANK_SLOT4_256K) {
        return 256 * 1024;
    }
    if (bank == SMARTXX_BANK_SLOT5_256K) {
        return 256 * 1024;
    }
    if (bank == SMARTXX_BANK_SLOT6_256K) {
        return 256 * 1024;
    }
    if (bank == SMARTXX_BANK_SLOT7_256K) {
        return 256 * 1024;
    }
    if (bank == SMARTXX_BANK_SLOT1_512K)
    {
        return 512 * 1024;
    }
    if (bank == SMARTXX_BANK_SLOT2_512K)
    {
        return 512 * 1024;
    }
    if (bank == SMARTXX_BANK_SLOT3_512K)
    {
        return 512 * 1024;
    }
    if (bank == SMARTXX_BANK_SLOT1_1024K)
    {
        return 1024 * 1024;
    }
    if (bank == SMARTXX_BANK_BOOTLOADER) {
        return 256 * 1024;
    }
    if (bank == SMARTXX_BANK_PROMETHEOS1) {
        return 512 * 1024;
    }
    if (bank == SMARTXX_BANK_PROMETHEOS2) {
        return 512 * 1024;
    }
    if (bank == SMARTXX_BANK_RESERVED1) {
        return 512 * 1024;
    }
    if (bank == SMARTXX_BANK_RESERVED2) {
        return 512 * 1024;
    }
    return 0;
}

uint32_t modchipSmartxx::getBankMemOffset(uint8_t bank) {
    if (bank == SMARTXX_BANK_SLOT1_256K) {
        return 0x000000;
    }
    if (bank == SMARTXX_BANK_SLOT2_256K) {
        return 0x040000;
    }
    if (bank == SMARTXX_BANK_SLOT3_256K) {
        return 0x080000;
    }
    if (bank == SMARTXX_BANK_SLOT4_256K) {
        return 0x0c0000;
    }
    if (bank == SMARTXX_BANK_SLOT5_256K) {
        return 0x100000;
    }
    if (bank == SMARTXX_BANK_SLOT6_256K) {
        return 0x140000;
    }
    if (bank == SMARTXX_BANK_SLOT7_256K) {
        return 0x180000;
    }
    if (bank == SMARTXX_BANK_SLOT1_512K)
    {
        return 0x000000;
    }
    if (bank == SMARTXX_BANK_SLOT2_512K)
    {
        return 0x080000;
    }
    if (bank == SMARTXX_BANK_SLOT3_512K)
    {
        return 0x100000;
    }
    if (bank == SMARTXX_BANK_SLOT1_1024K)
    {
        return 0x000000;
    }
    if (bank == SMARTXX_BANK_BOOTLOADER) {
        return 0x1c0000;
    }
    if (bank == SMARTXX_BANK_PROMETHEOS1) {
        return 0x200000;
    }
    if (bank == SMARTXX_BANK_PROMETHEOS2) {
        return 0x280000;
    }
    if (bank == SMARTXX_BANK_RESERVED1) {
        return 0x300000;
    }
    if (bank == SMARTXX_BANK_RESERVED2) {
        return 0x380000;
    }
    return 0;
}

uint32_t modchipSmartxx::getBankStartOffset(uint8_t bank) {
    return 0;
}

uint8_t modchipSmartxx::getBankFromIdAndSlots(uint8_t id, uint8_t slots) {
    if (id == 0 && slots == 1) {
        return SMARTXX_BANK_SLOT1_256K;
    }
    if (id == 1 && slots == 1) {
        return SMARTXX_BANK_SLOT2_256K;
    }
    if (id == 2 && slots == 1) {
        return SMARTXX_BANK_SLOT3_256K;
    }
    if (id == 3 && slots == 1) {
        return SMARTXX_BANK_SLOT4_256K;
    }
    if (id == 4 && slots == 1) {
        return SMARTXX_BANK_SLOT5_256K;
    }
    if (id == 5 && slots == 1) {
        return SMARTXX_BANK_SLOT6_256K;
    }
    if (id == 6 && slots == 1) {
        return SMARTXX_BANK_SLOT7_256K;
    }
    if (id == 0 && slots == 2)
    {
        return SMARTXX_BANK_SLOT1_512K;
    }
    if (id == 2 && slots == 2)
    {
        return SMARTXX_BANK_SLOT2_512K;
    }
    if (id == 4 && slots == 2)
    {
        return SMARTXX_BANK_SLOT3_512K;
    }
    if (id == 0 && slots == 4)
    {
        return SMARTXX_BANK_SLOT1_1024K;
    }
    return 0;
}

utils::dataContainer* modchipSmartxx::readBank(uint8_t bank, bool is_backup) {
    setBank(SMARTXX_BANK_SLOT1_4096K);
    uint32_t bankSize = getBankSize(bank);
    utils::dataContainer* dataContainer = new utils::dataContainer(bankSize);
    volatile uint8_t* lpcMemMap = (volatile uint8_t*)LPC_MEMORY_BASE;
    memcpy(dataContainer->data, (void*)&lpcMemMap[getBankMemOffset(bank)], bankSize);

    if (is_backup && (bank == SMARTXX_BANK_BOOTLOADER || bank == SMARTXX_BANK_SLOT7_256K)) {
        uint32_t bankSize = getBankSize(bank);
        for (uint32_t i = 65536; i < bankSize; i++)
        {
            dataContainer->data[i] = dataContainer->data[i] ^ 0xD7;
        }
    }

    setBank(SMARTXX_BANK_BOOTLOADER);
    return dataContainer;
}

void modchipSmartxx::eraseBank(uint8_t bank, bool isUpdate) {
    setBank(SMARTXX_BANK_SLOT1_4096K);

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

    setBank(SMARTXX_BANK_BOOTLOADER);

#ifndef TOOLS
    setLedColor(settingsManager::getLedColor());
#else
    setLedColor(LED_COLOR_GREEN);
#endif
}

void modchipSmartxx::writeBank(uint8_t bank, utils::dataContainer* dataContainer, bool isUpdate) {
    setBank(SMARTXX_BANK_SLOT1_4096K);

    setLedColor(LED_COLOR_BLUE);

    volatile uint8_t* lpcMemMap = (volatile uint8_t*)LPC_MEMORY_BASE;

    uint32_t memOffset = getBankMemOffset(bank);

    outputByte(SMARTXX_REGISTER_FLASH_ENABLE, 1);
    for (uint32_t i = 0; i < dataContainer->size; i++) {
        if ((isUpdate && isSettingsMemOffset(memOffset + i)) || isProtectedMemOffset(memOffset + i) == true) {
            continue;
        }
        uint32_t flashOffset = memOffset + i;
        uint8_t value = (uint8_t)dataContainer->data[i];
        if ((bank == SMARTXX_BANK_BOOTLOADER || bank == SMARTXX_BANK_SLOT7_256K) && i >= 65536) {
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
    outputByte(SMARTXX_REGISTER_FLASH_ENABLE, 0);

    setBank(SMARTXX_BANK_BOOTLOADER);

#ifndef TOOLS
    setLedColor(settingsManager::getLedColor());
#else
    setLedColor(LED_COLOR_GREEN);
#endif
}

bool modchipSmartxx::verifyBank(uint8_t bank, utils::dataContainer* dataContainer, bool isUpdate) {
    setBank(SMARTXX_BANK_SLOT1_4096K);

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
        if ((bank == SMARTXX_BANK_BOOTLOADER || bank == SMARTXX_BANK_SLOT7_256K) && i >= 65536) {
            value = value ^ 0xD7;
        }
        if ((uint8_t)writtenData->data[i] == value) {
            continue;
        }
        ok = false;
        break;
    }

    delete (writtenData);

    setBank(SMARTXX_BANK_BOOTLOADER);

#ifndef TOOLS
    setLedColor(settingsManager::getLedColor());
#else
    setLedColor(LED_COLOR_GREEN);
#endif
    return ok;
}

uint8_t modchipSmartxx::getFlashBankCount(bool recovery) {
    return recovery ? 0 : 12;
}

uint8_t modchipSmartxx::getFlashBank(bool recovery, uint8_t bank) {
    const uint8_t banks[] = {
        SMARTXX_BANK_SLOT1_256K,
        SMARTXX_BANK_SLOT2_256K,
        SMARTXX_BANK_SLOT3_256K,
        SMARTXX_BANK_SLOT4_256K,
        SMARTXX_BANK_SLOT5_256K,
        SMARTXX_BANK_SLOT6_256K,
        SMARTXX_BANK_SLOT7_256K,
        SMARTXX_BANK_BOOTLOADER,
        SMARTXX_BANK_PROMETHEOS1,
        SMARTXX_BANK_PROMETHEOS2,
        SMARTXX_BANK_RESERVED1,
        SMARTXX_BANK_RESERVED2,
    };
    return recovery ? 0 : banks[bank];
}

bankType modchipSmartxx::getFlashBankType(bool recovery, uint8_t bank) {
    const bankType banks[] = {
        bankTypeUser,
        bankTypeUser,
        bankTypeUser,
        bankTypeUser,
        bankTypeUser,
        bankTypeUser,
        bankTypeUser,
        bankTypeSystem,
        bankTypeSystem,
        bankTypeSystem,
        bankTypeSystem,
        bankTypeSystem,
    };
    return recovery ? bankTypeUser : banks[bank];
}

utils::dataContainer* modchipSmartxx::readFlash(bool recovery) {
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
    utils::ioOutputByte(SMARTXX_REGISTER_FLASH_ENABLE, 1);

    lpcMemMap[unlockCycleAddressMangled] = unlockCycleCommandMangled;
    lpcMemMap[sequenceAddressMangled] = sequenceCommandMangled;
    lpcMemMap[unlockCycleAddressMangled] = utils::smartxxMangleCommand(0x90);
    manuf = utils::smartxxUnmangleCommandResponse(lpcMemMap[0]);
    devid = utils::smartxxUnmangleCommandResponse(lpcMemMap[2]);
    lpcMemMap[0] = utils::smartxxMangleCommand(0xF0);

    utils::ioOutputByte(SMARTXX_REGISTER_FLASH_ENABLE, 0);
}

} // namespace

uint8_t modchipSmartxx::getFlashManufacturerId() {
    volatile uint8_t* lpcMemMap = (volatile uint8_t*)LPC_MEMORY_BASE;
    uint8_t manuf = 0;
    uint8_t devid = 0;
    readSmartxxFlashIds(lpcMemMap, mUnlockCycleAddressMangled, mUnlockCycleCommandMangled, mSequenceAddressMangled,
        mSequenceCommandMangled, manuf, devid);
    return manuf;
}

uint8_t modchipSmartxx::getFlashDeviceId() {
    volatile uint8_t* lpcMemMap = (volatile uint8_t*)LPC_MEMORY_BASE;
    uint8_t manuf = 0;
    uint8_t devid = 0;
    readSmartxxFlashIds(lpcMemMap, mUnlockCycleAddressMangled, mUnlockCycleCommandMangled, mSequenceAddressMangled,
        mSequenceCommandMangled, manuf, devid);
    return devid;
}

uint8_t modchipSmartxx::getRecoveryFlashManufacturerId() {
    return 0;
}

uint8_t modchipSmartxx::getRecoveryFlashDeviceId() {
    return 0;
}

void modchipSmartxx::launchBank(uint8_t bank, uint8_t ledColor, bool safeMode) {
    utils::prepareScratch(safeMode);  
    setBank(bank);
    setLedColor(ledColor);
    HalReturnToFirmware(RETURN_FIRMWARE_REBOOT);
}

void modchipSmartxx::launchTsop(bool safeMode) {
    utils::prepareScratch(safeMode);  

    outputByte(0xf705, 1);

    setLedColor(LED_COLOR_OFF);
    HalReturnToFirmware(RETURN_FIRMWARE_REBOOT);
}

void modchipSmartxx::launchRecovery(bool safeMode) {
}

#ifndef TOOLS

void modchipSmartxx::loadSettings(settingsState& settings) {
    setBank(SMARTXX_SETTINGS_BANK);

    setLedColor(LED_COLOR_WHITE);

    volatile uint8_t* lpcMemMap = (volatile uint8_t*)LPC_MEMORY_BASE;

    memcpy(&settings, (void*)&lpcMemMap[SMARTXX_SETTINGS_OFFSET], sizeof(settings));
    uint32_t checksum = crc32::calculate(((uint8_t*)&settings) + sizeof(uint32_t), sizeof(settings) - sizeof(uint32_t));

    setBank(SMARTXX_BANK_BOOTLOADER);

    if (checksum != settings.checksum ||
        versioning::compareVersion(settings.version, settingsManager::getVersion()) != 0) {
        settingsManager::initSettings();
        settingsManager::saveSettings();
    }

    setLedColor(settingsManager::getLedColor());
}

void modchipSmartxx::saveSettings(settingsState& settings) {
    setBank(SMARTXX_SETTINGS_BANK);

    settings.checksum = crc32::calculate(((uint8_t*)&settings) + sizeof(uint32_t), sizeof(settings) - sizeof(uint32_t));
    utils::dataContainer* settingsData = new utils::dataContainer((char*)&settings, sizeof(settings), sizeof(settings));

    volatile uint8_t* lpcMemMap = (volatile uint8_t*)LPC_MEMORY_BASE;

    setLedColor(LED_COLOR_AMBER);
    sectorErase(SMARTXX_SETTINGS_OFFSET);

    setLedColor(LED_COLOR_BLUE);

    outputByte(SMARTXX_REGISTER_FLASH_ENABLE, 1);
    for (uint32_t i = 0; i < settingsData->size; i++) {
        uint8_t value = (uint8_t)settingsData->data[i];
        lpcMemMap[mUnlockCycleAddressMangled] = mUnlockCycleCommandMangled;
        lpcMemMap[mSequenceAddressMangled] = mSequenceCommandMangled;
        lpcMemMap[mUnlockCycleAddressMangled] = utils::smartxxMangleCommand(0xA0);
        lpcMemMap[SMARTXX_SETTINGS_OFFSET + i] = value;
        while (lpcMemMap[SMARTXX_SETTINGS_OFFSET + i] != value)
            ;
    }
    lpcMemMap[0] = utils::smartxxMangleCommand(0xF0);
    outputByte(SMARTXX_REGISTER_FLASH_ENABLE, 0);

    delete (settingsData);

    setBank(SMARTXX_BANK_BOOTLOADER);

    setLedColor(settingsManager::getLedColor());
}

#endif

displayDriver* modchipSmartxx::getDisplayDriver(bool current) {
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

supportInfo modchipSmartxx::getSupportInfo(bool current) {
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

uint8_t modchipSmartxx::getLcdModeCount() {
    return 3;
}

char* modchipSmartxx::getLcdModeString(uint8_t lcdMode) {
    if (lcdMode == 1) {
        return strdup("Smartxx");
    }
    if (lcdMode == 2) {
        return strdup("SMBUS");
    }
    return strdup("Disabled");
}

uint8_t modchipSmartxx::getLcdModelCount(bool current) {
    uint8_t lcdMode = settingsManager::getLcdMode(current);
    if (lcdMode == 1) {
        return 2;
    }
    if (lcdMode == 2) {
        return 2;
    }
    return 0;
}

char* modchipSmartxx::getLcdModelString(bool current, uint8_t lcdModel) {
    displayDriver* driver = getDisplayDriver(current);
    if (driver == NULL) {
        return strdup("");
    }
    return driver->getModel();
}

uint8_t modchipSmartxx::getLcdAddressCount(bool current) {
    displayDriver* driver = getDisplayDriver(current);
    if (driver == NULL) {
        return 0;
    }
    return driver->getI2cAddressCount();
}

char* modchipSmartxx::getLcdAddressString(bool current, uint8_t lcdAddress) {
    displayDriver* driver = getDisplayDriver(current);
    if (driver == NULL) {
        return strdup("");
    }
    uint8_t address = driver->getI2cAddress(lcdAddress);
    char* result = stringUtility::formatString("0x%2X", address);
    return result;
}

void modchipSmartxx::lcdSetCursorPosition(uint8_t row, uint8_t col) {
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

void modchipSmartxx::lcdInit() {
    displayDriver* driver = getDisplayDriver(true);
    if (driver == NULL) {
        return;
    }
    driver->setAddress(settingsManager::getLcdAddress(true));
    driver->init();
}

void modchipSmartxx::lcdPrintMessage(const char* message) {
    displayDriver* driver = getDisplayDriver(true);
    if (driver == NULL) {
        return;
    }
    driver->setAddress(settingsManager::getLcdAddress(true));
    driver->printMessage(message);
}

void modchipSmartxx::lcdSetBacklight(uint8_t value) {
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

void modchipSmartxx::lcdSetContrast(uint8_t value) {
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

void modchipSmartxx::setBank(uint8_t bank) {
    mBank = bank;
    outputByte(SMARTXX_REGISTER_BANKING, bank);
}

uint8_t modchipSmartxx::getBank() {
    return mBank;
}

bool modchipSmartxx::isEraseMemOffset(uint32_t memOffset) {
    if (memOffset >= 0x000000 && memOffset <= 0x00ffff) {
        return (memOffset % 8192) == 0;
    }
    if (memOffset >= 0x010000 && memOffset <= 0x3fffff) {
        return (memOffset % 65536) == 0;
    }
    return false;
}

bool modchipSmartxx::isProtectedMemOffset(uint32_t memOffset) {
    return false;
}

bool modchipSmartxx::isSettingsMemOffset(uint32_t memOffset) {
    if (memOffset >= 0x2f0000 && memOffset <= 0x2fffff) {
        return true;
    }
    return false;
}

void modchipSmartxx::sectorErase(uint32_t offset) {
    if (isProtectedMemOffset(offset)) {
        return;
    }

    uint32_t mangledOffset = utils::smartxxMangleFlashAddress(offset);

    outputByte(SMARTXX_REGISTER_FLASH_ENABLE, 1);

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

    outputByte(SMARTXX_REGISTER_FLASH_ENABLE, 0);
}
