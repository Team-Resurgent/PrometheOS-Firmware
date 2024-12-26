#include "displayHD44780Aladdin.h"

static displayDriver* mInstance = NULL;

displayDriver* displayHD44780Aladdin::getInstance()
{
	if (mInstance == NULL)
	{
		mInstance = new displayHD44780Aladdin();
	}
	return mInstance;
}

bool displayHD44780Aladdin::isSpi2Par()
{
	return false;
}

char* displayHD44780Aladdin::getModel()
{
	return strdup("HD44780");
}

void displayHD44780Aladdin::init()
{
	utils::ioOutputByte(ALADDIN_REGISTER_IO, ALADDIN_DISP_EN5V);
	writeValue(0x33, 0);
	Sleep(ALADDIN_LCD_DELAY);
	writeValue(0x32, 0);  
	Sleep(ALADDIN_LCD_DELAY);
	writeValue(ALADDIN_DISP_FUNCTION_SET | ALADDIN_DISP_N_FLAG | ALADDIN_DISP_RE_FLAG, ALADDIN_DISP_CMD);
	Sleep(ALADDIN_LCD_DELAY);
	writeValue(ALADDIN_DISP_SEGRAM_SET, ALADDIN_DISP_CMD);
	Sleep(ALADDIN_LCD_DELAY);
	writeValue(ALADDIN_DISP_EXT_CONTROL | ALADDIN_DISP_NW_FLAG, ALADDIN_DISP_CMD);
	Sleep(ALADDIN_LCD_DELAY);
	writeValue(ALADDIN_DISP_FUNCTION_SET | ALADDIN_DISP_N_FLAG, ALADDIN_DISP_CMD); 
	Sleep(ALADDIN_LCD_DELAY);
	writeValue(ALADDIN_DISP_CONTROL | ALADDIN_DISP_D_FLAG, ALADDIN_DISP_CMD);
	Sleep(ALADDIN_LCD_DELAY);
	writeValue(ALADDIN_DISP_CLEAR, ALADDIN_DISP_CMD);
	Sleep(ALADDIN_LCD_DELAY);
	writeValue(ALADDIN_DISP_ENTRY_MODE_SET | ALADDIN_DISP_ID_FLAG, ALADDIN_DISP_CMD);
	Sleep(ALADDIN_LCD_DELAY);
	writeValue(ALADDIN_DISP_HOME, ALADDIN_DISP_CMD);  
	Sleep(ALADDIN_LCD_DELAY);
	utils::ioOutputByte(ALADDIN_REGISTER_LCD_BL, (int)(settingsManager::getLcdBacklight(true) * 1.27f));
	utils::ioOutputByte(ALADDIN_REGISTER_LCD_CT, (int)(settingsManager::getLcdContrast(true) * 0.64f));
	Sleep(ALADDIN_LCD_DELAY);
}

void displayHD44780Aladdin::printMessage(const char* message)
{
	for (int i = 0; i < (int)strlen(message); ++i)
	{
		writeValue(message[i], ALADDIN_DISP_DAT);
	}
}

void displayHD44780Aladdin::setCursorPosition(uint8_t row, uint8_t col)
{
	int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };
	uint8_t cursor = ALADDIN_DISP_DDRAM_SET | (col + row_offsets[row]);
	writeValue(cursor, ALADDIN_DISP_CMD);
	Sleep(ALADDIN_LCD_DELAY);
}

void displayHD44780Aladdin::getSupport(bool& lcdInfo, bool& backlight, bool& contrast)
{
	lcdInfo = false;
	backlight = true;
	contrast = true;
}

void displayHD44780Aladdin::setBacklight(uint8_t backlight)
{
	utils::ioOutputByte(ALADDIN_REGISTER_LCD_BL, (int)(backlight * 1.27f));
	Sleep(ALADDIN_LCD_DELAY); 
}

void displayHD44780Aladdin::setContrast(uint8_t contrast)
{
	utils::ioOutputByte(ALADDIN_REGISTER_LCD_CT, (int)(contrast * 0.64f));
	Sleep(ALADDIN_LCD_DELAY); 
}

uint8_t displayHD44780Aladdin::getI2cAddressCount()
{
	return 0;
}

uint8_t displayHD44780Aladdin::getI2cAddress(uint8_t index)
{
	return 0;
}

// Private

void displayHD44780Aladdin::writeValue(uint8_t value, uint8_t command) 
{
	uint8_t oDataHigh  = (value >> 2) & 0x28;
	oDataHigh |= (value >> 0) & 0x50;
	utils::ioOutputByte(ALADDIN_REGISTER_DISP_O, command | oDataHigh);	
	utils::ioOutputByte(ALADDIN_REGISTER_DISP_O, command | ALADDIN_DISP_CON_E | oDataHigh);	
	utils::ioOutputByte(ALADDIN_REGISTER_DISP_O, command | oDataHigh);	
	Sleep(ALADDIN_LCD_DELAY);
	
	uint8_t oDataLow  = (value << 2) & 0x28;
	oDataLow |= (value << 4) & 0x50;
	utils::ioOutputByte(ALADDIN_REGISTER_DISP_O, command | oDataLow);	
	utils::ioOutputByte(ALADDIN_REGISTER_DISP_O, command | ALADDIN_DISP_CON_E | oDataLow);	
	utils::ioOutputByte(ALADDIN_REGISTER_DISP_O, command | oDataLow);	
	Sleep(ALADDIN_LCD_DELAY);
}