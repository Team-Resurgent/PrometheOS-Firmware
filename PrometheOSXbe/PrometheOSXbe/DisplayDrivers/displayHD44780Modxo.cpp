#include "displayHD44780Modxo.h"

static displayDriver* mInstance = NULL;

displayDriver* displayHD44780Modxo::getInstance()
{
	if (mInstance == NULL)
	{
		mInstance = new displayHD44780Modxo();
	}
	return mInstance;
}

bool displayHD44780Modxo::isSpi2Par()
{
	return false;
}

char* displayHD44780Modxo::getModel()
{
	return strdup("HD44780");
}

void displayHD44780Modxo::init()
{
	uint8_t address = getI2cAddress(getAddressValue());

	utils::ioOutputByte(MODXO_REGISTER_LCD_COMMAND, MODXO_LCD_SET_CLK);
	utils::ioOutputByte(MODXO_REGISTER_LCD_COMMAND, 100);
	utils::ioOutputByte(MODXO_REGISTER_LCD_COMMAND, MODXO_LCD_I2C);
	utils::ioOutputByte(MODXO_REGISTER_LCD_COMMAND, address);

	utils::ioOutputByte(MODXO_REGISTER_LCD_COMMAND, MODXO_LCD_SET_I2C_PREFIX);
	utils::ioOutputByte(MODXO_REGISTER_LCD_COMMAND, MODXO_LCD_COMMAND_MODE);

	utils::ioOutputByte(MODXO_REGISTER_LCD_DATA, 0x2a);
	utils::ioOutputByte(MODXO_REGISTER_LCD_DATA, 0x71);

	utils::ioOutputByte(MODXO_REGISTER_LCD_COMMAND, MODXO_LCD_SET_I2C_PREFIX);
	utils::ioOutputByte(MODXO_REGISTER_LCD_COMMAND, MODXO_LCD_DATA_MODE);

	utils::ioOutputByte(MODXO_REGISTER_LCD_DATA, 0x00);

	utils::ioOutputByte(MODXO_REGISTER_LCD_COMMAND, MODXO_LCD_SET_I2C_PREFIX);
	utils::ioOutputByte(MODXO_REGISTER_LCD_COMMAND, MODXO_LCD_COMMAND_MODE);

	utils::ioOutputByte(MODXO_REGISTER_LCD_DATA, 0x28);  
	utils::ioOutputByte(MODXO_REGISTER_LCD_DATA, 0x08);
	utils::ioOutputByte(MODXO_REGISTER_LCD_DATA, 0x2a); 
	utils::ioOutputByte(MODXO_REGISTER_LCD_DATA, 0x79); 
	utils::ioOutputByte(MODXO_REGISTER_LCD_DATA, 0xd5);
	utils::ioOutputByte(MODXO_REGISTER_LCD_DATA, 0x70);
	utils::ioOutputByte(MODXO_REGISTER_LCD_DATA, 0x78);
	utils::ioOutputByte(MODXO_REGISTER_LCD_DATA, 0x08 | 0x01); //0x01 is 3-4 rows
	utils::ioOutputByte(MODXO_REGISTER_LCD_DATA, 0x06);
	utils::ioOutputByte(MODXO_REGISTER_LCD_DATA, 0x72);

	utils::ioOutputByte(MODXO_REGISTER_LCD_COMMAND, MODXO_LCD_SET_I2C_PREFIX);
	utils::ioOutputByte(MODXO_REGISTER_LCD_COMMAND, MODXO_LCD_DATA_MODE);

	utils::ioOutputByte(MODXO_REGISTER_LCD_DATA, 0x00);

	utils::ioOutputByte(MODXO_REGISTER_LCD_COMMAND, MODXO_LCD_SET_I2C_PREFIX);
	utils::ioOutputByte(MODXO_REGISTER_LCD_COMMAND, MODXO_LCD_COMMAND_MODE);
	
	utils::ioOutputByte(MODXO_REGISTER_LCD_DATA, 0x2a); 
	utils::ioOutputByte(MODXO_REGISTER_LCD_DATA, 0x79); 
	utils::ioOutputByte(MODXO_REGISTER_LCD_DATA, 0xda);
	utils::ioOutputByte(MODXO_REGISTER_LCD_DATA, 0x10);
	utils::ioOutputByte(MODXO_REGISTER_LCD_DATA, 0xdc);
	utils::ioOutputByte(MODXO_REGISTER_LCD_DATA, 0x00);
	utils::ioOutputByte(MODXO_REGISTER_LCD_DATA, 0x81);
	utils::ioOutputByte(MODXO_REGISTER_LCD_DATA, (uint8_t)(settingsManager::getLcdContrast(true) * 2.55f));
	utils::ioOutputByte(MODXO_REGISTER_LCD_DATA, 0xd9);
	utils::ioOutputByte(MODXO_REGISTER_LCD_DATA, 0xf1);
	utils::ioOutputByte(MODXO_REGISTER_LCD_DATA, 0xdb); 
	utils::ioOutputByte(MODXO_REGISTER_LCD_DATA, 0x40);
	utils::ioOutputByte(MODXO_REGISTER_LCD_DATA, 0x78); 
	utils::ioOutputByte(MODXO_REGISTER_LCD_DATA, 0x28);
	utils::ioOutputByte(MODXO_REGISTER_LCD_DATA, 0x01);
	utils::ioOutputByte(MODXO_REGISTER_LCD_DATA, 0x80);
	utils::ioOutputByte(MODXO_REGISTER_LCD_DATA, 0x08 | 0x04);
}

void displayHD44780Modxo::printMessage(const char* message)
{
	utils::ioOutputByte(MODXO_REGISTER_LCD_COMMAND, MODXO_LCD_SET_I2C_PREFIX);
	utils::ioOutputByte(MODXO_REGISTER_LCD_COMMAND, MODXO_LCD_DATA_MODE);

	for (int i = 0; i < (int)strlen(message); ++i)
	{
		utils::ioOutputByte(MODXO_REGISTER_LCD_DATA, message[i]);
	}
}

void displayHD44780Modxo::setCursorPosition(uint8_t row, uint8_t col)
{
	utils::ioOutputByte(MODXO_REGISTER_LCD_COMMAND, MODXO_LCD_SET_I2C_PREFIX);
	utils::ioOutputByte(MODXO_REGISTER_LCD_COMMAND, MODXO_LCD_COMMAND_MODE);

	int row_offsets[] = { 0x00, 0x20, 0x40, 0x60 };
	uint8_t cursor = 0x80 | (col + row_offsets[row]);
	utils::ioOutputByte(MODXO_REGISTER_LCD_DATA, cursor);
}

void displayHD44780Modxo::getSupport(bool& lcdInfo, bool& backlight, bool& contrast)
{
	lcdInfo = false;
	backlight = false;
	contrast = true;
}

void displayHD44780Modxo::setBacklight(uint8_t contrast)
{
}

void displayHD44780Modxo::setContrast(uint8_t contrast)
{
	utils::ioOutputByte(MODXO_REGISTER_LCD_COMMAND, MODXO_LCD_SET_I2C_PREFIX);
	utils::ioOutputByte(MODXO_REGISTER_LCD_COMMAND, MODXO_LCD_COMMAND_MODE);

	utils::ioOutputByte(MODXO_REGISTER_LCD_DATA, 0x2A);
	utils::ioOutputByte(MODXO_REGISTER_LCD_DATA, 0x79);
	utils::ioOutputByte(MODXO_REGISTER_LCD_DATA, 0x81);
	utils::ioOutputByte(MODXO_REGISTER_LCD_DATA, (uint8_t)(contrast * 2.55f));
	utils::ioOutputByte(MODXO_REGISTER_LCD_DATA, 0x78);
	utils::ioOutputByte(MODXO_REGISTER_LCD_DATA, 0x28);
}

uint8_t displayHD44780Modxo::getI2cAddressCount()
{
	return 16;
}

uint8_t displayHD44780Modxo::getI2cAddress(uint8_t index)
{
	int lcdAddressArray[] = { 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F };
	return lcdAddressArray[index];
}