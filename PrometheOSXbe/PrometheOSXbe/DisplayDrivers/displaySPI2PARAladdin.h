#pragma once

#include "displayDriver.h"

//D7 = CLK
//D6 = MOSI
//D5 = CS

//WriteToIO(XBLAST_IO, (GenPurposeIOs.GPO3 << 7) | (GenPurposeIOs.GPO2 << 6) | (GenPurposeIOs.GPO1 << 5) | (GenPurposeIOs.GPO0 << 4) | GenPurposeIOs.EN_5V);

class displaySPI2PARAladdin : displayDriver
{
public:
	static displayDriver* getInstance();
	bool isSpi2Par();
	char* getModel();
	void init();
	void printMessage(const char* message);
	void setCursorPosition(uint8_t row, uint8_t col);
	void getSupport(bool& lcdInfo, bool& backlight, bool& contrast);
	void setBacklight(uint8_t backlight);
	void setContrast(uint8_t contrast);
	uint8_t getI2cAddressCount();
	uint8_t getI2cAddress(uint8_t index);
private:
	void writeValue(uint8_t value);
};