#include "displayHD44780Xecuter.h"

static displayDriver* mInstance = NULL;

displayDriver* displayHD44780Xecuter::getInstance()
{
	if (mInstance == NULL)
	{
		mInstance = new displayHD44780Xecuter();
	}
	return mInstance;
}

bool displayHD44780Xecuter::isSpi2Par()
{
	return false;
}

char* displayHD44780Xecuter::getModel()
{
	return strdup("HD44780");
}

void displayHD44780Xecuter::init()
{
	utils::ioOutputByte(XECUTER_REGISTER_DISP_O_DAT, 0);
	utils::ioOutputByte(XECUTER_REGISTER_DISP_O_CMD, 0);
	utils::ioOutputByte(XECUTER_REGISTER_DISP_O_DIR_DAT, 0xFF);
	utils::ioOutputByte(XECUTER_REGISTER_DISP_O_DIR_CMD, 0x07);

	writeValue(XECUTER_DISP_FUNCTION_SET | XECUTER_DISP_DL_FLAG, XECUTER_DISP_INI);
	Sleep(XECUTER_LCD_DELAY);
	writeValue(XECUTER_DISP_FUNCTION_SET | XECUTER_DISP_DL_FLAG, XECUTER_DISP_INI);	
	Sleep(XECUTER_LCD_DELAY);
	writeValue(XECUTER_DISP_FUNCTION_SET | XECUTER_DISP_DL_FLAG, XECUTER_DISP_INI);	
	Sleep(XECUTER_LCD_DELAY);
	writeValue(XECUTER_DISP_FUNCTION_SET, XECUTER_DISP_INI);								
	Sleep(XECUTER_LCD_DELAY);
	writeValue(XECUTER_DISP_FUNCTION_SET | XECUTER_DISP_N_FLAG, XECUTER_DISP_CMD);	
	Sleep(XECUTER_LCD_DELAY);
	writeValue(XECUTER_DISP_CONTROL | XECUTER_DISP_D_FLAG, XECUTER_DISP_CMD);		
	Sleep(XECUTER_LCD_DELAY);
	writeValue(XECUTER_DISP_CLEAR, XECUTER_DISP_CMD);			
	Sleep(XECUTER_LCD_DELAY);
	writeValue(XECUTER_DISP_ENTRY_MODE_SET | XECUTER_DISP_ID_FLAG,XECUTER_DISP_CMD);	
	Sleep(XECUTER_LCD_DELAY);
	utils::ioOutputByte(XECUTER_REGISTER_DISP_O_LIGHT, (int)(settingsManager::getLcdBacklight(true) * 1.27f));
	Sleep(XECUTER_LCD_DELAY); 
}

void displayHD44780Xecuter::printMessage(const char* message)
{
	for (int i = 0; i < (int)strlen(message); ++i)
	{
		writeValue(message[i], XECUTER_DISP_DAT);
	}
}

void displayHD44780Xecuter::setCursorPosition(uint8_t row, uint8_t col)
{
	int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };
	uint8_t cursor = XECUTER_DISP_DDRAM_SET | (col + row_offsets[row]);
	writeValue(cursor, XECUTER_DISP_CMD);
	Sleep(XECUTER_LCD_DELAY);
}

void displayHD44780Xecuter::getSupport(bool& lcdInfo, bool& backlight, bool& contrast)
{
	lcdInfo = false;
	backlight = true;
	contrast = false;
}

void displayHD44780Xecuter::setBacklight(uint8_t backlight)
{
	utils::ioOutputByte(XECUTER_REGISTER_DISP_O_LIGHT, (int)(backlight * 1.27f));
	Sleep(XECUTER_LCD_DELAY); 
}

void displayHD44780Xecuter::setContrast(uint8_t contrast)
{
}

uint8_t displayHD44780Xecuter::getI2cAddressCount()
{
	return 0;
}

uint8_t displayHD44780Xecuter::getI2cAddress(uint8_t index)
{
	return 0;
}

// Private

void displayHD44780Xecuter::writeValue(uint8_t value, uint8_t command) 
{
	uint8_t cmd = 0;
	if (command & XECUTER_DISP_DAT)
	{
		cmd |= XECUTER_DISP_CON_RS;
	}

	utils::ioOutputByte(XECUTER_REGISTER_DISP_O_DAT, value & 0xF0);
	utils::ioOutputByte(XECUTER_REGISTER_DISP_O_CMD, cmd);	
	utils::ioOutputByte(XECUTER_REGISTER_DISP_O_CMD, XECUTER_DISP_CON_E | cmd);	
	utils::ioOutputByte(XECUTER_REGISTER_DISP_O_CMD, cmd);	

	if ((command & XECUTER_DISP_INI) == 0) 
	{							
		utils::ioOutputByte(XECUTER_REGISTER_DISP_O_DAT, (value << 4) & 0xF0);
		utils::ioOutputByte(XECUTER_REGISTER_DISP_O_CMD, cmd);
		utils::ioOutputByte(XECUTER_REGISTER_DISP_O_CMD, XECUTER_DISP_CON_E | cmd);
		utils::ioOutputByte(XECUTER_REGISTER_DISP_O_CMD, cmd);
	}
}