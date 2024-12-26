#include "displaySPI2PARXtremium.h"

static displayDriver* mInstance = NULL;

displayDriver* displaySPI2PARXtremium::getInstance()
{
	if (mInstance == NULL)
	{
		mInstance = new displaySPI2PARXtremium();
	}
	return mInstance;
}

bool displaySPI2PARXtremium::isSpi2Par()
{
	return true;
}

char* displaySPI2PARXtremium::getModel()
{
	return strdup("SPI2PAR");
}

void displaySPI2PARXtremium::init()
{
	writeValue(SPI2PAR_SHOWDISPLAY); 
	utils::uSleep(2000);
	writeValue(SPI2PAR_FORMFEED); 
	utils::uSleep(2000);
	writeValue(SPI2PAR_HIDECURSOR);
	utils::uSleep(2000);
	writeValue(SPI2PAR_SCROLLOFF); 
	utils::uSleep(2000);
	writeValue(SPI2PAR_WRAPOFF);
	utils::uSleep(2000);
	writeValue(SPI2PAR_SETBACKLIGHT); 
	writeValue(settingsManager::getLcdBacklight(true)); 
	utils::uSleep(2000);
	writeValue(SPI2PAR_SETCONTRAST); 
	writeValue(settingsManager::getLcdContrast(true));
	utils::uSleep(2000);
}

void displaySPI2PARXtremium::printMessage(const char* message)
{
	for (int i = 0; i < (int)strlen(message); ++i)
	{
		writeValue(message[i]);
		utils::uSleep(2000);
	}
}

void displaySPI2PARXtremium::setCursorPosition(uint8_t row, uint8_t col)
{
	writeValue(SPI2PAR_SETCURSORPOSITION);
	writeValue(col);
	writeValue(row);
	utils::uSleep(2000);
}

void displaySPI2PARXtremium::getSupport(bool& lcdInfo, bool& backlight, bool& contrast)
{
	lcdInfo = true;
	backlight = true;
	contrast = true;
}

void displaySPI2PARXtremium::setBacklight(uint8_t backlight)
{
	writeValue(SPI2PAR_SETBACKLIGHT); 
	writeValue(backlight);
	utils::uSleep(2000);
}

void displaySPI2PARXtremium::setContrast(uint8_t contrast)
{
	writeValue(SPI2PAR_SETCONTRAST); 
	writeValue(contrast); 
	utils::uSleep(2000);
}

uint8_t displaySPI2PARXtremium::getI2cAddressCount()
{
	return 0;
}

uint8_t displaySPI2PARXtremium::getI2cAddress(uint8_t index)
{
	return 0;
}

// Private

void displaySPI2PARXtremium::writeValue(uint8_t value) 
{
	uint8_t spi = getSpiValue();
	uint8_t bank = getBankValue();

	spi |= XTREMIUM_SPI_CLK_BIT;
	utils::ioOutputByte(XTREMIUM_REGISTER_BANKING, (spi | bank));
	utils::uSleep(0x1c);
	
	uint8_t i = 8;
	do
	{
		utils::uSleep(0x1c);

		if ((value & 0x80) > 0)
		{
			spi |= XTREMIUM_SPI_MOSI_BIT;
		} 
		else
		{
			spi &= ~XTREMIUM_SPI_MOSI_BIT;
		}
		utils::ioOutputByte(XTREMIUM_REGISTER_BANKING, (spi | bank));

		spi &= ~XTREMIUM_SPI_CLK_BIT;
		utils::ioOutputByte(XTREMIUM_REGISTER_BANKING, (spi | bank));
		utils::uSleep(0x1c);

		spi |= XTREMIUM_SPI_CLK_BIT;
		utils::ioOutputByte(XTREMIUM_REGISTER_BANKING, (spi | bank));

		value = value << 1;
		i--;
	}
	while (i > 0);

	setSpiValue(spi);
}