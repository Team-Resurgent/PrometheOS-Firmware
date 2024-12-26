#include "displaySPI2PARModxo.h"

static displayDriver* mInstance = NULL;

displayDriver* displaySPI2PARModxo::getInstance()
{
	if (mInstance == NULL)
	{
		mInstance = new displaySPI2PARModxo();
	}
	return mInstance;
}

bool displaySPI2PARModxo::isSpi2Par()
{
	return true;
}

char* displaySPI2PARModxo::getModel()
{
	return strdup("SPI2PAR");
}

void displaySPI2PARModxo::init()
{
	utils::ioOutputByte(MODXO_REGISTER_LCD_COMMAND, MODXO_LCD_SET_CLK);
	utils::ioOutputByte(MODXO_REGISTER_LCD_COMMAND, 10);
	utils::ioOutputByte(MODXO_REGISTER_LCD_COMMAND, MODXO_LCD_SET_SPI_MODE);
	utils::ioOutputByte(MODXO_REGISTER_LCD_COMMAND, 3);
	utils::ioOutputByte(MODXO_REGISTER_LCD_COMMAND, MODXO_LCD_SPI);
	utils::ioOutputByte(MODXO_REGISTER_LCD_COMMAND, 0);

	utils::ioOutputByte(MODXO_REGISTER_LCD_DATA, SPI2PAR_SHOWDISPLAY); 
	utils::uSleep(2000);
	utils::ioOutputByte(MODXO_REGISTER_LCD_DATA, SPI2PAR_FORMFEED);  
	utils::uSleep(2000);
	utils::ioOutputByte(MODXO_REGISTER_LCD_DATA, SPI2PAR_HIDECURSOR);
	utils::uSleep(2000);
	utils::ioOutputByte(MODXO_REGISTER_LCD_DATA, SPI2PAR_SCROLLOFF); 
	utils::uSleep(2000);
	utils::ioOutputByte(MODXO_REGISTER_LCD_DATA, SPI2PAR_WRAPOFF);
	utils::uSleep(2000);
	utils::ioOutputByte(MODXO_REGISTER_LCD_DATA, SPI2PAR_SETBACKLIGHT); 
	utils::ioOutputByte(MODXO_REGISTER_LCD_DATA, settingsManager::getLcdBacklight(true)); 
	utils::uSleep(2000);
	utils::ioOutputByte(MODXO_REGISTER_LCD_DATA, SPI2PAR_SETCONTRAST); 
	utils::ioOutputByte(MODXO_REGISTER_LCD_DATA, settingsManager::getLcdContrast(true)); 
	utils::uSleep(2000);
}

void displaySPI2PARModxo::printMessage(const char* message)
{
	for (int i = 0; i < (int)strlen(message); ++i)
	{
		utils::ioOutputByte(MODXO_REGISTER_LCD_DATA, message[i]);
		utils::uSleep(2000);
	}
}

void displaySPI2PARModxo::setCursorPosition(uint8_t row, uint8_t col)
{
	utils::ioOutputByte(MODXO_REGISTER_LCD_DATA, SPI2PAR_SETCURSORPOSITION); 
	utils::ioOutputByte(MODXO_REGISTER_LCD_DATA, col); 
	utils::ioOutputByte(MODXO_REGISTER_LCD_DATA, row);
	utils::uSleep(2000);
}

void displaySPI2PARModxo::getSupport(bool& lcdInfo, bool& backlight, bool& contrast)
{
	lcdInfo = false;
	backlight = true;
	contrast = true;
}

void displaySPI2PARModxo::setBacklight(uint8_t backlight)
{
	utils::ioOutputByte(MODXO_REGISTER_LCD_DATA, SPI2PAR_SETBACKLIGHT); 
	utils::ioOutputByte(MODXO_REGISTER_LCD_DATA, backlight); 
	utils::uSleep(2000);
}

void displaySPI2PARModxo::setContrast(uint8_t contrast)
{
	utils::ioOutputByte(MODXO_REGISTER_LCD_DATA, SPI2PAR_SETCONTRAST); 
	utils::ioOutputByte(MODXO_REGISTER_LCD_DATA, contrast);
	utils::uSleep(2000);
}

uint8_t displaySPI2PARModxo::getI2cAddressCount()
{
	return 0;
}

uint8_t displaySPI2PARModxo::getI2cAddress(uint8_t index)
{
	return 0;
}