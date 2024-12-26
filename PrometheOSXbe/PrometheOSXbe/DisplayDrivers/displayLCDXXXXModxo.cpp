#include "displayLCDXXXXModxo.h"

static displayDriver* mInstance = NULL;

displayDriver* displayLCDXXXXModxo::getInstance()
{
	if (mInstance == NULL)
	{
		mInstance = new displayLCDXXXXModxo();
	}
	return mInstance;
}

bool displayLCDXXXXModxo::isSpi2Par()
{
	return false;
}

char* displayLCDXXXXModxo::getModel()
{
	return strdup("LCDXXXX");
}

void displayLCDXXXXModxo::init()
{
	uint8_t address = getI2cAddress(getAddressValue());
	utils::ioOutputByte(MODXO_REGISTER_LCD_COMMAND, MODXO_LCD_SET_CLK);
	utils::ioOutputByte(MODXO_REGISTER_LCD_COMMAND, 100);
	utils::ioOutputByte(MODXO_REGISTER_LCD_COMMAND, MODXO_LCD_REMOVE_I2C_PREFIX);
	utils::ioOutputByte(MODXO_REGISTER_LCD_COMMAND, MODXO_LCD_I2C);
	utils::ioOutputByte(MODXO_REGISTER_LCD_COMMAND, address);
	utils::uSleep(2000);

	write4bits(0x03 << 4);
	utils::uSleep(4500);
	write4bits(0x03 << 4);
	utils::uSleep(4500);
	write4bits(0x03 << 4);
	utils::uSleep(4500);
	write4bits(0x02 << 4);
	command(0x20 | 0x08);
	command(0x08 | 0x04);
	command(0x01);
	utils::uSleep(2000);
	command(0x04 | 0x02);
	command(0x02);
	utils::uSleep(2000);
}

void displayLCDXXXXModxo::printMessage(const char* message)
{
	for (int i = 0; i < (int)strlen(message); ++i)
	{
		writeChar(message[i]);
	}
}

void displayLCDXXXXModxo::setCursorPosition(uint8_t row, uint8_t col)
{
	int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };
	uint8_t cursor = 0x80 | (col + row_offsets[row]);
	command(cursor);
}

void displayLCDXXXXModxo::getSupport(bool& lcdInfo, bool& backlight, bool& contrast)
{
	lcdInfo = false;
	backlight = false;
	contrast = false;
}

void displayLCDXXXXModxo::setBacklight(uint8_t backlight)
{
}

void displayLCDXXXXModxo::setContrast(uint8_t contrast)
{
}

uint8_t displayLCDXXXXModxo::getI2cAddressCount()
{
	return 16;
}

uint8_t displayLCDXXXXModxo::getI2cAddress(uint8_t index)
{
	int lcdAddressArray[] = { 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F };
	return lcdAddressArray[index];
}


//Private

void displayLCDXXXXModxo::expanderWrite(uint8_t value) 
{
    uint8_t bl = 0x08;
	uint8_t data = (uint8_t)(value | bl);
	utils::ioOutputByte(MODXO_REGISTER_LCD_DATA, data);
}

void displayLCDXXXXModxo::pulseEnable(uint8_t value) 
{
    uint8_t En = 0x04;

    expanderWrite(value | En);
	utils::uSleep(1);

    expanderWrite(value & ~En);
    utils::uSleep(50);
}

void displayLCDXXXXModxo::write4bits(uint8_t value) 
{
    expanderWrite(value);
    pulseEnable(value);
}

void displayLCDXXXXModxo::send(uint8_t value, uint8_t mode) 
{
    uint8_t highnib = value & 0xf0;
    uint8_t lownib = (value << 4) & 0xf0;
    write4bits((highnib) | mode);
    write4bits((lownib) | mode); 
}

void displayLCDXXXXModxo::command(uint8_t value) 
{
    send(value, 0);
}

void displayLCDXXXXModxo::writeChar(uint8_t value) 
{
    send(value, 1);
}