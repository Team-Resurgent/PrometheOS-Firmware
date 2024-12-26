#include "displaySPI2PARAladdin.h"

static displayDriver* mInstance = NULL;

displayDriver* displaySPI2PARAladdin::getInstance()
{
	if (mInstance == NULL)
	{
		mInstance = new displaySPI2PARAladdin();
	}
	return mInstance;
}

bool displaySPI2PARAladdin::isSpi2Par()
{
	return true;
}

char* displaySPI2PARAladdin::getModel()
{
	return strdup("SPI2PAR");
}

void displaySPI2PARAladdin::init()
{
	utils::ioOutputByte(ALADDIN_REGISTER_IO, ALADDIN_DISP_EN5V);

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

void displaySPI2PARAladdin::printMessage(const char* message)
{
	for (int i = 0; i < (int)strlen(message); ++i)
	{
		writeValue(message[i]);
		utils::uSleep(2000);
	}
}

void displaySPI2PARAladdin::setCursorPosition(uint8_t row, uint8_t col)
{
	writeValue(SPI2PAR_SETCURSORPOSITION);
	writeValue(col);
	writeValue(row);
	utils::uSleep(2000);
}

void displaySPI2PARAladdin::getSupport(bool& lcdInfo, bool& backlight, bool& contrast)
{
	lcdInfo = false;
	backlight = true;
	contrast = true;
}

void displaySPI2PARAladdin::setBacklight(uint8_t backlight)
{
	writeValue(SPI2PAR_SETBACKLIGHT); 
	writeValue(backlight);
	utils::uSleep(2000);
}

void displaySPI2PARAladdin::setContrast(uint8_t contrast)
{
	writeValue(SPI2PAR_SETCONTRAST); 
	writeValue(contrast); 
	utils::uSleep(2000);
}

uint8_t displaySPI2PARAladdin::getI2cAddressCount()
{
	return 0;
}

uint8_t displaySPI2PARAladdin::getI2cAddress(uint8_t index)
{
	return 0;
}

// Private

void displaySPI2PARAladdin::writeValue(uint8_t value) 
{
	uint8_t spi = getSpiValue();

	spi |= ALADDIN_SPI_CLK_BIT;
	spi &= ~ALADDIN_SPI_CS_BIT;
	utils::ioOutputByte(ALADDIN_REGISTER_DISP_O, spi);
	utils::uSleep(0x1);

	uint8_t i = 8;
	do
	{
		if ((value & 0x80) > 0)
		{
			spi |= ALADDIN_SPI_MOSI_BIT;
		} 
		else
		{
			spi &= ~ALADDIN_SPI_MOSI_BIT;
		}

		spi &= ~ALADDIN_SPI_CLK_BIT;
		utils::ioOutputByte(ALADDIN_REGISTER_DISP_O, spi);
		utils::uSleep(0x1);

		spi |= ALADDIN_SPI_CLK_BIT;
		utils::ioOutputByte(ALADDIN_REGISTER_DISP_O, spi);
		utils::uSleep(0x1);

		value = value << 1;
		i--;
	}
	while (i > 0);

	spi |= ALADDIN_SPI_CS_BIT;
	utils::ioOutputByte(ALADDIN_REGISTER_DISP_O, spi);
	utils::uSleep(0x1);

	setSpiValue(spi);
}