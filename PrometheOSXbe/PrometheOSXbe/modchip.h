#pragma once

#include "utils.h"
#include "xboxInternals.h"
#include "globalTypes.h"
#include "DisplayDrivers\displayDriver.h"
#include "DisplayDrivers\displayFactory.h"

#ifndef TOOLS
#include "settingsManager.h"
#endif

#define LPC_MEMORY_BASE 0xFF000000u

#define LED_COLOR_OFF 0
#define LED_COLOR_RED 1
#define LED_COLOR_GREEN 2
#define LED_COLOR_AMBER 3
#define LED_COLOR_BLUE 4
#define LED_COLOR_PURPLE 5
#define LED_COLOR_TEAL 6
#define LED_COLOR_WHITE 7

typedef struct supportInfo 
{
	bool supportsLed;
	bool supportsLcd;
	bool supportsLcdInfo;
	bool supportsLcdBacklight;
	bool supportsLcdContrast;
	bool supportsRecovery;
} supportInfo;

class modchip
{
public:

	static uint8_t inputByte(uint16_t port);
	static void outputByte(uint16_t port, uint8_t value); 
	static modchipType detectModchip();

	virtual void setLedColor(uint8_t ledColor) = 0;

	virtual uint32_t getSlotCount() = 0;
	virtual uint32_t getFlashSize(bool recovery) = 0;
	virtual void disableRecovery() = 0;
	virtual bool isValidBankSize(uint32_t size) = 0;
	virtual bool isValidFlashSize(bool recovery, uint32_t size) = 0;
	virtual uint32_t getBankSize(uint8_t bank) = 0;
	virtual uint32_t getBankMemOffset(uint8_t bank) = 0;
	virtual uint32_t getBankStartOffset(uint8_t bank) = 0;
	virtual uint8_t getBankFromIdAndSlots(uint8_t id, uint8_t slots) = 0;
	virtual utils::dataContainer* readBank(uint8_t bank) = 0;
	virtual void eraseBank(uint8_t bank) = 0;
	virtual void writeBank(uint8_t bank, utils::dataContainer* dataContainer) = 0;
	virtual bool verifyBank(uint8_t bank, utils::dataContainer* dataContainer) = 0;

	virtual uint8_t getFlashBankCount(bool recovery) = 0;
	virtual uint8_t getFlashBank(bool recovery, uint8_t bank) = 0;
	virtual bankType getFlashBankType(bool recovery, uint8_t bank) = 0;
	virtual utils::dataContainer* readFlash(bool recovery) = 0;

	virtual void launchBank(uint8_t bank, uint8_t ledColor) = 0;
	virtual void launchTsop() = 0;
	virtual void launchRecovery() = 0;

#ifndef TOOLS
	virtual void loadSettings(settingsState& settings) = 0;
	virtual void saveSettings(settingsState& settings) = 0;
#endif

	virtual utils::dataContainer* getInstallerLogo() = 0;

	virtual displayDriver* getDisplayDriver(bool current) = 0;
	virtual supportInfo getSupportInfo(bool current) = 0;
	virtual uint8_t getLcdModeCount() = 0;
	virtual char* getLcdModeString(uint8_t lcdMode) = 0;
	virtual uint8_t getLcdModelCount(bool current) = 0;
	virtual char* getLcdModelString(bool current, uint8_t lcdModel) = 0;
	virtual uint8_t getLcdAddressCount(bool current) = 0;
	virtual char* getLcdAddressString(bool current, uint8_t lcdAddress) = 0;

	virtual void lcdSetCursorPosition(uint8_t row, uint8_t col) = 0;
	virtual void lcdInit() = 0;
	virtual void lcdPrintMessage(const char* message) = 0;
	virtual void lcdSetBacklight(uint8_t value) = 0;
	virtual void lcdSetContrast(uint8_t value) = 0;

private:

	virtual void setBank(uint8_t bank) = 0;
	virtual uint8_t getBank() = 0;
	virtual bool isEraseMemOffset(uint32_t memOffset) = 0;
	virtual bool isProtectedMemOffset(uint32_t memOffset) = 0;
	virtual void sectorErase(uint32_t offset) = 0;
};
