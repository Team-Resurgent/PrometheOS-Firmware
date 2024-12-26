#include "displayDriver.h"	

void displayDriver::setAddress(uint8_t address)
{
	mAddress = address;
}

uint8_t displayDriver::getAddressValue()
{
	return mAddress;
}

void displayDriver::setBankValue(uint8_t bank)
{
	mBank = bank;
}

uint8_t displayDriver::getBankValue()
{
	return mBank;
}

void displayDriver::setSpiValue(uint8_t spi)
{
	mSpi = spi;
}

uint8_t displayDriver::getSpiValue()
{
	return mSpi;
}