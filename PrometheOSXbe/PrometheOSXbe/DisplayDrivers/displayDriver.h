#pragma once

#include "..\xboxinternals.h"
#include "..\globalDefines.h"
#include "..\utils.h"
#include "..\settingsManager.h"

class displayDriver
{
public:
	virtual bool isSpi2Par() = 0;
	virtual char* getModel() = 0;
	virtual void init() = 0;
	virtual void printMessage(const char* message) = 0;
	virtual void setCursorPosition(uint8_t row, uint8_t col) = 0;
	virtual void getSupport(bool& lcdInfo, bool& backlight, bool& contrast) = 0;
	virtual void setBacklight(uint8_t backlight) = 0;
	virtual void setContrast(uint8_t contrast) = 0;
	virtual uint8_t getI2cAddressCount() = 0;
	virtual uint8_t getI2cAddress(uint8_t index) = 0;

	void setAddress(uint8_t address);
	uint8_t getAddressValue();
	void setBankValue(uint8_t bank);
	uint8_t getBankValue();
	void setSpiValue(uint8_t spi);
	uint8_t getSpiValue();
private:
	uint8_t mAddress;
	uint8_t mBank;
	uint8_t mSpi;
};