#include "xenium.h"
#include "utils.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <xtl.h>

//IO CONTROL INTERFACE

#define ENABLE_XENIUM

#ifdef ENABLE_XENIUM
static uint8_t sckReg = 0;
static uint8_t mosiReg = 0;
#endif

#ifndef ENABLE_XENIUM
static uint32_t bankOffset = 0;
static uint8_t xeniumMem[2048 * 1024];
#endif

uint8_t xenium::inputByte(uint16_t port) 
{
#ifdef ENABLE_XENIUM
	unsigned char value;
	__asm {
		mov dx, port
		in al, dx
		mov value, al
	}
	return value;
#else
	return 0;
#endif
}

void xenium::outputByte(uint16_t port, uint8_t value) 
{
#ifdef ENABLE_XENIUM
	__asm {
		mov     dx, port
		mov     al, value
		out dx, al
	}
#endif
}

void xenium::setBank(bankEnum bank)
{
#ifdef ENABLE_XENIUM
	if (bank <= 10) 
	{
        outputByte(XENIUM_REGISTER_BANKING, bank);
	}
#else
	bankOffset = sectorToAddress(bankToSector(bank));
#endif
}

xenium::bankEnum xenium::getBank()
{
	return (bankEnum)(inputByte(XENIUM_REGISTER_BANKING) & 0x0F);
}

void xenium::setLedColor(ledColorEnum ledColor)
{
    outputByte(XENIUM_REGISTER_LED, ledColor);
}

//FLASH MEMORY INTERFACE

void xenium::lpcSendByte(uint32_t address, uint8_t data)
{
#ifdef ENABLE_XENIUM
	volatile uint8_t* lpcMemMap = (volatile uint8_t *)LPC_MEMORY_BASE;
    lpcMemMap[address] = data;
#else
	xeniumMem[bankOffset + address] = data;
#endif
}

uint8_t xenium::flashReadByte(uint32_t address)
{
#ifdef ENABLE_XENIUM
    volatile uint8_t* lpcMemMap = (volatile uint8_t *)LPC_MEMORY_BASE;
    return lpcMemMap[address];
#else
	return xeniumMem[bankOffset + address];
#endif
}

void xenium::flashReset()
{
    lpcSendByte(0x00000000, 0xF0);
}

void xenium::flashReadStream(uint32_t address, uint8_t *data, uint32_t len)
{
#ifdef ENABLE_XENIUM
    volatile uint8_t* lpcMemMap = (volatile uint8_t *)LPC_MEMORY_BASE;
    memcpy(data, (void*)&lpcMemMap[address], len);
#else
	memcpy(data, (void*)&xeniumMem[bankOffset + address], len);
#endif
}

uint8_t xenium::flashBusy()
{
    return flashReadByte(0) != flashReadByte(0);
}

void xenium::sectorErase(uint32_t address)
{
#ifdef ENABLE_XENIUM
    lpcSendByte(0xAAAA, 0xAA);
    lpcSendByte(0x5555, 0x55);
    lpcSendByte(0xAAAA, 0x80);
    lpcSendByte(0xAAAA, 0xAA);
    lpcSendByte(0x5555, 0x55);
    lpcSendByte(address, 0x30);
	while(flashBusy());
#endif
}

void xenium::flashWriteByte(uint32_t address, uint8_t data)
{
#ifdef ENABLE_XENIUM
    lpcSendByte(0xAAAA, 0xAA);
    lpcSendByte(0x5555, 0x55);
    lpcSendByte(0xAAAA, 0xA0);
#endif
    lpcSendByte(address, data);
	while (flashReadByte(address) != data);
}

uint8_t xenium::isDetected()
{
#ifdef ENABLE_XENIUM
    lpcSendByte(0xAAAA, 0xAA);
    lpcSendByte(0x5555, 0x55);
    lpcSendByte(0xAAAA, 0x90);
    uint8_t manufacturerId = flashReadByte(0x00);
    flashReset();

    lpcSendByte(0xAAAA, 0xAA);
    lpcSendByte(0x5555, 0x55);
    lpcSendByte(0xAAAA, 0x90);
	uint8_t deviceIda = flashReadByte(0x01);
    uint8_t deviceId = flashReadByte(0x02);
    flashReset();

    if ((manufacturerId == XENIUM_MANUF_ID || manufacturerId == XENIUM_MANUF_ID1) && deviceId == XENIUM_DEVICE_ID)
    {
        return 1;
    }
#endif
    return 0;
}

uint32_t xenium::sectorToAddress(uint8_t sector)
{
    if (sector < 31)  
	{
		return sector * XENIUM_FLASH_SECTOR_SIZE1;
	}
    if (sector == 31) 
	{
		return 0x1f0000;
	}
    if (sector == 32) 
	{
		return 0x1f8000;
	}
    if (sector == 33) 
	{
		return 0x1fa000; 
	}
    if (sector == 34) 
	{
		return 0x1fc000; 
	}
    return 0;
}

uint32_t xenium::sectorToSize(uint8_t sector)
{
    if (sector < 31) 
	{
		return 64 * 1024; 
	}
    if (sector == 31)
	{
		return 32 * 1024;
	}
    if (sector == 32) 
	{
		return 8 * 1024;
	}
    if (sector == 33)
	{
		return 8 * 1024;
	}
    if (sector == 34) 
	{
		return 16 * 1024; 
	}
    return 0;
}

xenium::bankEnum xenium::sectorToBank(uint8_t sector)
{
    uint32_t address = sectorToAddress(sector);
    if (address < 0x100000) 
	{
		return bank1_1024k;
	}
    if (address < 0x180000) 
	{ 
		return bankXeniumOS;
	}
    if (address < 0x1c0000) 
	{
		return bankBootloader;
	}
    return bankRecovery;
}

uint8_t xenium::bankToSector(bankEnum bank)
{
	if (bank == bank1_256k) 
	{
		return 0;
	}
	if (bank == bank2_256k) 
	{
		return 4;
	}
	if (bank == bank3_256k) 
	{
		return 8;
	}
	if (bank == bank4_256k) 
	{
		return 12;
	}
	if (bank == bank1_512k) 
	{
		return 0;
	}
	if (bank == bank2_512k) 
	{
		return 8;
	}
	if (bank == bank1_1024k) 
	{
		return 0;
	}
	if (bank == bankXeniumOS) 
	{
		return 16;
	}
	if (bank == bankBootloader) 
	{
		return 24;
	}
	return 28;
}

uint8_t xenium::bankToSectors(bankEnum bank)
{
	if (bank == bank1_256k) 
	{
		return 4;
	}
	if (bank == bank2_256k) 
	{
		return 4;
	}
	if (bank == bank3_256k) 
	{
		return 4;
	}
	if (bank == bank4_256k) 
	{
		return 4;
	}
	if (bank == bank1_512k) 
	{
		return 8;
	}
	if (bank == bank2_512k) 
	{
		return 8;
	}
	if (bank == bank1_1024k) 
	{
		return 16;
	}
	if (bank == bankXeniumOS) 
	{
		return 8;
	}
	if (bank == bankBootloader) 
	{
		return 4;
	}
	return 4;
}

int32_t xenium::getBankSize(bankEnum bank)
{
	if (bank == bank1_256k) 
	{
		return 256 * 1024;
	}
	if (bank == bank2_256k) 
	{
		return 256 * 1024;
	}
	if (bank == bank3_256k) 
	{
		return 256 * 1024;
	}
	if (bank == bank4_256k) 
	{
		return 256 * 1024;
	}
	if (bank == bank1_512k) 
	{
		return 512 * 1024;
	}
	if (bank == bank2_512k) 
	{
		return 512 * 1024;
	}
	if (bank == bank1_1024k) 
	{
		return 1024 * 1024;
	}
	if (bank == bankXeniumOS) 
	{
		return 512 * 1024;
	}
	if (bank == bankBootloader) 
	{
		return 256 * 1024;
	}
	return -1;
}

xenium::bankEnum xenium::getBankFromIdAndSlots(uint8_t id, uint8_t slots)
{
	if (id == 0 && slots == 4)
	{
		return bank1_1024k;
	}
	if (id == 0 && slots == 2)
	{
		return bank1_512k;
	}
	if (id == 0 && slots == 1)
	{
		return bank1_256k;
	}
	if (id == 1 && slots == 1)
	{
		return bank2_256k;
	}
	if (id == 2 && slots == 2)
	{
		return bank2_512k;
	}
	if (id == 2 && slots == 1)
	{
		return bank3_256k;
	}
	if (id == 3 && slots == 1)
	{
		return bank4_256k;
	}
	return bankInvalid;
}

utils::dataContainer* xenium::readBank(bankEnum bank, uint32_t offset, uint32_t length)
{
	bankEnum originalBank = xenium::getBank();
	xenium::setBank(bank); 
	utils::dataContainer* dataContainer = new utils::dataContainer(length);
	xenium::flashReadStream(offset, (uint8_t*)dataContainer->data, length);
	xenium::setBank(originalBank);
	return dataContainer;
}

void xenium::eraseBank(bankEnum bank, uint32_t offset, uint32_t size)
{
	bankEnum originalBank = xenium::getBank();
	xenium::setBank(bank);

	utils::debugPrint("bank erasing sectors\n");
    for (uint32_t i = 0; i < size; i++)
	{
		uint32_t address = offset + i;
		if (((address % XENIUM_FLASH_SECTOR_SIZE1) == 0) || (bank == bankRecovery && address >= (3 * XENIUM_FLASH_SECTOR_SIZE1) && (address % XENIUM_FLASH_SECTOR_SIZE2) == 0))
		{
			sectorErase(address);
			flashReset();
		}
	}
	xenium::setBank(originalBank);
}

void xenium::writeBank(bankEnum bank, uint32_t offset, utils::dataContainer* dataContainer)
{
	bankEnum originalBank = xenium::getBank();
	xenium::setBank(bank);

	utils::debugPrint("bank flashing\n");
    for (uint32_t i = 0; i < dataContainer->size; i++)
	{
		flashWriteByte(offset + i, (uint8_t)dataContainer->data[i]);
    }
	flashReset();

	xenium::setBank(originalBank);
}

bool xenium::verifyBank(bankEnum bank, uint32_t offset, utils::dataContainer* dataContainer)
{
	bankEnum originalBank = xenium::getBank();
	xenium::setBank(bank);

	utils::debugPrint("bank verifying\n");
	utils::dataContainer* writtenData = readBank(bank, offset, dataContainer->size);

	bool ok = true;
    for (uint32_t i = 0; i < dataContainer->size; i++)
	{
		if (writtenData->data[i] == dataContainer->data[i])
		{
			continue;
		}
		ok = false;
		break;
    }

	delete(writtenData);

	xenium::setBank(originalBank);

	return ok;
}

void xenium::launchBank(uint8_t id, uint8_t slots, ledColorEnum ledColor)
{
	bankEnum bank = getBankFromIdAndSlots(id, slots);
	if (bank == bankInvalid)
	{
		return;
	}

	DWORD scratch;
	HalReadSMBusByte(SMBDEV_PIC16L, PIC16L_CMD_SCRATCH_REGISTER, &scratch);
	HalWriteSMBusByte(SMBDEV_PIC16L, PIC16L_CMD_SCRATCH_REGISTER, scratch & ~SCRATCH_REGISTER_BITVALUE_NO_ANIMATION);
	setBank(bank);
	setLedColor(ledColor);
	HalReturnToFirmware(RETURN_FIRMWARE_REBOOT);
}

void xenium::launchTsop()
{
	DWORD scratch;
	HalReadSMBusByte(SMBDEV_PIC16L, PIC16L_CMD_SCRATCH_REGISTER, &scratch);
	HalWriteSMBusByte(SMBDEV_PIC16L, PIC16L_CMD_SCRATCH_REGISTER, scratch & ~SCRATCH_REGISTER_BITVALUE_NO_ANIMATION);
	setBank(bankTsop);
	setLedColor(ledColorOff);
	HalReturnToFirmware(RETURN_FIRMWARE_REBOOT);
}

void xenium::sendCharacter(uint8_t value)
{
	const int dataMode = 3;
	const int bitOrder = 7;

	const uint8_t sckBit = 1 << 6;
	const uint8_t mosiBit = 1 << 4;

	static bool spiInitialized = false;
	if (spiInitialized == false)
	{
		outputByte(0xEF, 1 << 6);
		Sleep(100);
		outputByte(0xEF, 0);
		Sleep(100);
		spiInitialized = true;
	}

	for (uint8_t x = 0; x < 8; x++) 
	{
		if ((value >> (x ^ bitOrder)) & 0x01)
		{
			mosiReg |= mosiBit;
		} 
		else
		{
			mosiReg &= ~mosiBit;
		}

		outputByte(0xEF, mosiReg | sckReg);

		if (dataMode >> 1)
		{
			sckReg &= ~sckBit;
		}
		else 
		{
			sckReg |= sckBit;	
		}
		
		outputByte(0xEF, mosiReg | sckReg);
		
		if (dataMode >> 1) 
		{ 
			sckReg |= sckBit;
		}
		else 
		{
			sckReg &= ~sckBit;
		}

		outputByte(0xEF, mosiReg | sckReg);

	}
}