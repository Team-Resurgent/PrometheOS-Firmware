#include "displayHD44780LPC.h"

static displayDriver* mInstance = NULL;

displayDriver* displayHD44780LPC::getInstance()
{
	if (mInstance == NULL)
	{
		mInstance = new displayHD44780LPC();
	}
	return mInstance;
}

bool displayHD44780LPC::isSpi2Par()
{
	return false;
}

char* displayHD44780LPC::getModel()
{
	return strdup("HD44780");
}

void displayHD44780LPC::init()
{
	uint8_t address = getI2cAddress(getAddressValue());

	HalWriteSMBusByte(address << 1, 0x80, 0x2a); 
	HalWriteSMBusByte(address << 1, 0x80, 0x71); 
	HalWriteSMBusByte(address << 1, 0x40, 0x00);
	HalWriteSMBusByte(address << 1, 0x80, 0x28);
	HalWriteSMBusByte(address << 1, 0x80, 0x08);
	HalWriteSMBusByte(address << 1, 0x80, 0x2a);
	HalWriteSMBusByte(address << 1, 0x80, 0x79); 
	HalWriteSMBusByte(address << 1, 0x80, 0xd5);
	HalWriteSMBusByte(address << 1, 0x80, 0x70);
	HalWriteSMBusByte(address << 1, 0x80, 0x78);
	HalWriteSMBusByte(address << 1, 0x80, 0x08 | 0x01); //0x01 is 3-4 rows
	HalWriteSMBusByte(address << 1, 0x80, 0x06);
	HalWriteSMBusByte(address << 1, 0x80, 0x72);
	HalWriteSMBusByte(address << 1, 0x40, 0x00); 
	HalWriteSMBusByte(address << 1, 0x80, 0x2a); 
	HalWriteSMBusByte(address << 1, 0x80, 0x79);
	HalWriteSMBusByte(address << 1, 0x80, 0xda);
	HalWriteSMBusByte(address << 1, 0x80, 0x10);
	HalWriteSMBusByte(address << 1, 0x80, 0xdc);
	HalWriteSMBusByte(address << 1, 0x80, 0x00);
	HalWriteSMBusByte(address << 1, 0x80, 0x81);
	HalWriteSMBusByte(address << 1, 0x80, (uint8_t)(settingsManager::getLcdContrast(true) * 2.55f));
	HalWriteSMBusByte(address << 1, 0x80, 0xd9);
	HalWriteSMBusByte(address << 1, 0x80, 0xf1);
	HalWriteSMBusByte(address << 1, 0x80, 0xdb); 
	HalWriteSMBusByte(address << 1, 0x80, 0x40);
	HalWriteSMBusByte(address << 1, 0x80, 0x78); 
	HalWriteSMBusByte(address << 1, 0x80, 0x28); 
	HalWriteSMBusByte(address << 1, 0x80, 0x01);
	HalWriteSMBusByte(address << 1, 0x80, 0x80);
	HalWriteSMBusByte(address << 1, 0x80, 0x08 | 0x04);
}

void displayHD44780LPC::printMessage(const char* message)
{
	uint8_t address = getI2cAddress(getAddressValue());

	for (int i = 0; i < (int)strlen(message); ++i)
	{
		HalWriteSMBusByte(address << 1, 0x40, message[i]);
	}
}

void displayHD44780LPC::setCursorPosition(uint8_t row, uint8_t col)
{
	uint8_t address = getI2cAddress(getAddressValue());

	int row_offsets[] = { 0x00, 0x20, 0x40, 0x60 };
	uint8_t cursor = 0x80 | (col + row_offsets[row]);
	HalWriteSMBusByte(address << 1, 0x80, cursor);
}

void displayHD44780LPC::getSupport(bool& lcdInfo, bool& backlight, bool& contrast)
{
	lcdInfo = false;
	backlight = false;
	contrast = true;
}

void displayHD44780LPC::setBacklight(uint8_t backlight)
{
}

void displayHD44780LPC::setContrast(uint8_t contrast)
{
	uint8_t address = getI2cAddress(getAddressValue());

	HalWriteSMBusByte(address << 1, 0x80, 0x2A);
	HalWriteSMBusByte(address << 1, 0x80, 0x79);
	HalWriteSMBusByte(address << 1, 0x80, 0x81);
	HalWriteSMBusByte(address << 1, 0x80, (uint8_t)(contrast * 2.55f));
	HalWriteSMBusByte(address << 1, 0x80, 0x78);
	HalWriteSMBusByte(address << 1, 0x80, 0x28);
}

uint8_t displayHD44780LPC::getI2cAddressCount()
{
	return 16;
}

uint8_t displayHD44780LPC::getI2cAddress(uint8_t index)
{
	int lcdAddressArray[] = { 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F };
	return lcdAddressArray[index];
}