#pragma once

#include "modchip.h"
#include "xboxInternals.h"

#ifndef TOOLS
#include "settingsManager.h"
#endif

class modchipDummy : public modchip {
  public:
    modchipDummy();
    ~modchipDummy();
    void setLedColor(uint8_t ledColor);

    char* getName();
    uint32_t getSlotCount();
    uint32_t getFlashSize(bool recovery);
    void disableRecovery();
    bool isValidBankSize(uint32_t size);
    bool isValidFlashSize(bool recovery, uint32_t size);
    uint32_t getBankSize(uint8_t bank);
    uint32_t getBankMemOffset(uint8_t bank);
    uint32_t getBankStartOffset(uint8_t bank);
    uint8_t getBankFromIdAndSlots(uint8_t id, uint8_t slots);
    utils::dataContainer* readBank(uint8_t bank, bool is_backup = false);
    void eraseBank(uint8_t bank, bool isUpdate);
    void writeBank(uint8_t bank, utils::dataContainer* dataContainer, bool isUpdate);
    bool verifyBank(uint8_t bank, utils::dataContainer* dataContainer, bool isUpdate);

    uint8_t getFlashBankCount(bool recovery);
    uint8_t getFlashBank(bool recovery, uint8_t bank);
    bankType getFlashBankType(bool recovery, uint8_t bank);
    utils::dataContainer* readFlash(bool recovery);

    uint8_t getFlashManufacturerId();
    uint8_t getFlashDeviceId();
    uint8_t getRecoveryFlashManufacturerId();
    uint8_t getRecoveryFlashDeviceId();

    void launchBank(uint8_t bank, uint8_t ledColor, bool safeMode);
    void launchTsop(bool safeMode);
    void launchRecovery(bool safeMode);

#ifndef TOOLS
    void loadSettings(settingsState& settings);
    void saveSettings(settingsState& settings);
#endif

    displayDriver* getDisplayDriver(bool current);
    supportInfo getSupportInfo(bool current);
    uint8_t getLcdModeCount();
    char* getLcdModeString(uint8_t lcdMode);
    uint8_t getLcdModelCount(bool current);
    char* getLcdModelString(bool current, uint8_t lcdModel);
    uint8_t getLcdAddressCount(bool current);
    char* getLcdAddressString(bool current, uint8_t lcdAddress);

    void lcdSetCursorPosition(uint8_t row, uint8_t col);
    void lcdInit();
    void lcdPrintMessage(const char* message);
    void lcdSetBacklight(uint8_t value);
    void lcdSetContrast(uint8_t value);

  private:
    void setBank(uint8_t bank);
    uint8_t getBank();
    bool isEraseMemOffset(uint32_t memOffset);
    bool isProtectedMemOffset(uint32_t memOffset);
    bool isSettingsMemOffset(uint32_t memOffset);
    void sectorErase(uint32_t offset);

  private:
    uint8_t* mFlashData;
};
