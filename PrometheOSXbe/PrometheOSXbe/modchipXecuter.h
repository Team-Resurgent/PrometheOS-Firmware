#pragma once

#include "modchip.h"
#include "xboxInternals.h"

#ifndef TOOLS
#include "settingsManager.h"
#endif

class modchipXecuter : public modchip
{
public:

	modchipXecuter();
	void setLedColor(uint8_t ledColor);

	uint32_t getSlotCount();
	uint32_t getFlashSize(bool recovery);
	void disableRecovery();
	bool isValidBankSize(uint32_t size);
	bool isValidFlashSize(bool recovery, uint32_t size);
	uint32_t getBankSize(uint8_t bank);
	uint32_t getBankMemOffset(uint8_t bank);
	uint32_t getBankStartOffset(uint8_t bank);
	uint8_t getBankFromIdAndSlots(uint8_t id, uint8_t slots);
	utils::dataContainer* readBank(uint8_t bank);
	void eraseBank(uint8_t bank);
	void writeBank(uint8_t bank, utils::dataContainer* dataContainer);
	bool verifyBank(uint8_t bank, utils::dataContainer* dataContainer);

	uint8_t getFlashBankCount(bool recovery);
	uint8_t getFlashBank(bool recovery, uint8_t bank);
	bankType getFlashBankType(bool recovery, uint8_t bank);
	utils::dataContainer* readFlash(bool recovery);

	void launchBank(uint8_t bank, uint8_t ledColor);
	void launchTsop();
	void launchRecovery();

#ifndef TOOLS
	void loadSettings(settingsState& settings);
	void saveSettings(settingsState& settings);
#endif

	utils::dataContainer* getInstallerLogo();

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
	void sectorErase(uint32_t offset);
	bool checkWriteProtect();

private:

	uint8_t mBank;
};
