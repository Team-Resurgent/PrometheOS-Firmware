#pragma once

#include "displayDriver.h"

class displayLCDXXXXModxo : displayDriver
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
	void expanderWrite(uint8_t value);
	void pulseEnable(uint8_t value); 
	void write4bits(uint8_t value); 
	void send(uint8_t val, uint8_t mode);  
	void command(uint8_t value); 
	void writeChar(uint8_t value);
};