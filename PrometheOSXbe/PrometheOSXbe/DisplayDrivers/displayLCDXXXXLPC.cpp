#include "displayLCDXXXXLPC.h"

static displayDriver* mInstance = NULL;

displayDriver* displayLCDXXXXLPC::getInstance()
{
	if (mInstance == NULL)
	{
		mInstance = new displayLCDXXXXLPC();
	}
	return mInstance;
}

bool displayLCDXXXXLPC::isSpi2Par()
{
	return false;
}

char* displayLCDXXXXLPC::getModel()
{
	return strdup("LCDXXXX");
}

void displayLCDXXXXLPC::init()
{
	uint8_t address = getI2cAddress(getAddressValue());

	write4bits(address, 0x03 << 4);
	utils::uSleep(4500);
	write4bits(address, 0x03 << 4);
	utils::uSleep(4500);
	write4bits(address, 0x03 << 4);
	utils::uSleep(4500);
	write4bits(address, 0x02 << 4);
	command(address, 0x20 | 0x08);
	command(address, 0x08 | 0x04);
	command(address, 0x01);
	utils::uSleep(2000);
	command(address, 0x04 | 0x02); 
	command(address, 0x02);
	utils::uSleep(2000);
}

void displayLCDXXXXLPC::printMessage(const char* message)
{
	uint8_t address = getI2cAddress(getAddressValue());

	for (int i = 0; i < (int)strlen(message); ++i)
	{
		writeChar(address, message[i]);
	}
}

void displayLCDXXXXLPC::setCursorPosition(uint8_t row, uint8_t col)
{
	uint8_t address = getI2cAddress(getAddressValue());

	int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };
	uint8_t cursor = 0x80 | (col + row_offsets[row]);
	command(address, cursor);
}

void displayLCDXXXXLPC::getSupport(bool& lcdInfo, bool& backlight, bool& contrast)
{
	lcdInfo = false;
	backlight = false;
	contrast = false;
}

void displayLCDXXXXLPC::setBacklight(uint8_t backlight)
{
}

void displayLCDXXXXLPC::setContrast(uint8_t contrast)
{
}

uint8_t displayLCDXXXXLPC::getI2cAddressCount()
{
	return 16;
}

uint8_t displayLCDXXXXLPC::getI2cAddress(uint8_t index)
{
	int lcdAddressArray[] = { 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F };
	return lcdAddressArray[index];
}

//Private

void displayLCDXXXXLPC::expanderWrite(uint8_t address, uint8_t value) 
{
    uint8_t bl = 0x08;
	HalWriteSMBusValue(address << 1, 0, 0xff, (value | bl));
}

void displayLCDXXXXLPC::pulseEnable(uint8_t address, uint8_t value) 
{
    uint8_t En = 0x04;

    expanderWrite(address, value | En);
	utils::uSleep(1);

    expanderWrite(address, value & ~En);
    utils::uSleep(50);
}

void displayLCDXXXXLPC::write4bits(uint8_t address, uint8_t value) 
{
    expanderWrite(address, value);
    pulseEnable(address, value);
}

void displayLCDXXXXLPC::send(uint8_t address, uint8_t value, uint8_t mode) 
{
    uint8_t highnib = value & 0xf0;
    uint8_t lownib = (value << 4) & 0xf0;
    write4bits(address, (highnib) | mode);
    write4bits(address, (lownib) | mode); 
}

void displayLCDXXXXLPC::command(uint8_t address, uint8_t value) 
{
    send(address, value, 0);
}

void displayLCDXXXXLPC::writeChar(uint8_t address, uint8_t value) 
{
    send(address, value, 1);
}