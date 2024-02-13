#include "xenium.h"
#include "xboxInternals.h"
#include "utils.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <xtl.h>

//IO CONTROL INTERFACE

//Bank1_256k      =  3  0x000000 - 0x03ffff  Sector Size 65536  Total  262144
//Bank2_256k      =  4  0x040000 - 0x07ffff  Sector Size 65536  Total  262144
//Bank3_256k      =  5  0x080000 - 0x0bffff  Sector Size 65536  Total  262144
//Bank4_256k      =  6  0x0c0000 - 0x0fffff  Sector Size 65536  Total  262144
//Bank1_512k      =  7  0x000000 - 0x07ffff  Sector Size 65536  Total  524288
//Bank2_512k      =  8  0x080000 - 0x0fffff  Sector Size 65536  Total  524288
//Bank1_1024k     =  9  0x000000 - 0x0fffff  Sector Size 65536  Total 1048576
//Bank_PrometheOS =  2  0x100000 - 0x17ffff  Sector Size 65536  Total  524288
//Bank_Bootloader =  1  0x180000 - 0x1bffff  Sector Size 65536  Total  262144
//Bank_Recovery   = 10  0x1c0000 - 0x1fffff  See Below          Total  262144

//0x1c0000 - 0x1effff  Sector Size 65536  Total 196608 (PrometheOS Continued)
//0x1f0000 - 0x1f7fff  Sector Size 32768  Total 32768  (Installer Logo)
//0x1f8000 - 0x1f9fff  Sector Size 8192   Total 8192   (Settings)
//0x1fa000 - 0x1fbfff  Sector Size 8192   Total 8192   (Spare)
//0x1fc000 - 0x1fffff  Sector Size 16384  Total 16384  (Spare)

//Suggested byes for PrometheOS = 720896 (524288 bytes Bank_PrometheOS + First 196608 bytes of Bank_Recovery)

#define ENABLE_XENIUM

static bool initialized = false;
static RTL_CRITICAL_SECTION mutex;
static uint8_t mBankState = 0;

#ifdef ENABLE_XENIUM
static uint8_t mRegister = 0;
#endif

#ifndef ENABLE_XENIUM
static uint32_t flashMemOffset = 0;
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
	if (initialized == false)
	{
		initialized = true;
		RtlInitializeCriticalSection(&mutex);
		uint8_t banking = inputByte(XENIUM_REGISTER_BANKING);
		mRegister = banking & 0xF0;
		mBankState = banking & 0x0F;
	}

	if (bank <= 10) 
	{
#ifdef ENABLE_XENIUM
		RtlEnterCriticalSection(&mutex);
		mBankState = (uint8_t)bank;
        outputByte(XENIUM_REGISTER_BANKING, mBankState | mRegister);
		RtlLeaveCriticalSection(&mutex);
#else
		flashMemOffset = xenium::getBankMemOffset(bank);
#endif
	}
}

xenium::bankEnum xenium::getBank()
{
	if (initialized == false)
	{
		initialized = true;
		RtlInitializeCriticalSection(&mutex);
		uint8_t banking = inputByte(XENIUM_REGISTER_BANKING);
		mRegister = banking & 0xF0;
		mBankState = banking & 0x0F;
	}

	RtlEnterCriticalSection(&mutex);
	mBankState = (inputByte(XENIUM_REGISTER_BANKING) & 0x0F);
	RtlLeaveCriticalSection(&mutex);

	return (bankEnum)mBankState;
}

void xenium::setIoState()
{
#ifdef ENABLE_XENIUM
	RtlEnterCriticalSection(&mutex);
	outputByte(XENIUM_REGISTER_BANKING, mBankState | mRegister);
	RtlLeaveCriticalSection(&mutex);
#endif
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
	xeniumMem[flashMemOffset + address] = data;
#endif
}

uint8_t xenium::flashReadByte(uint32_t address)
{
#ifdef ENABLE_XENIUM
    volatile uint8_t* lpcMemMap = (volatile uint8_t *)LPC_MEMORY_BASE;
    return lpcMemMap[address];
#else
	return xeniumMem[flashMemOffset + address];
#endif
}

void xenium::flashReset()
{
#ifdef ENABLE_XENIUM
    lpcSendByte(0x00000000, 0xF0);
#endif
}

void xenium::flashReadStream(uint32_t address, uint8_t *data, uint32_t len)
{
#ifdef ENABLE_XENIUM
    volatile uint8_t* lpcMemMap = (volatile uint8_t *)LPC_MEMORY_BASE;
    memcpy(data, (void*)&lpcMemMap[address], len);
#else
	memcpy(data, (void*)&xeniumMem[flashMemOffset + address], len);
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
	if (bank == bankRecovery) 
	{
		return 256 * 1024;
	}
	return -1;
}

uint32_t xenium::getBankMemOffset(xenium::bankEnum bank)
{
	if (bank == bank1_1024k)
	{
		return 0x000000;
	}
	if (bank == bank1_512k)
	{
		return 0x000000;
	}
	if (bank == bank1_256k)
	{
		return 0x000000;
	}
	if (bank == bank2_256k)
	{
		return 0x040000;
	}
	if (bank == bank2_512k)
	{
		return 0x080000;
	}
	if (bank == bank3_256k)
	{
		return 0x080000;
	}
	if (bank == bank4_256k)
	{
		return 0x0c0000;
	}
	if (bank == bankXeniumOS)
	{
		return 0x100000;
	}	
	if (bank == bankBootloader)
	{
		return 0x180000;
	}
	if (bank == bankRecovery)
	{
		return 0x1c0000;
	}
	return 0;
}

bool xenium::isEraseMemOffset(uint32_t memOffset, uint32_t& sectorSize)
{
	if (memOffset >= 0x000000 && memOffset <= 0x1effff)
	{
		sectorSize = 65536;
		return (memOffset % 65536) == 0;
	}
	if (memOffset >= 0x1f0000 && memOffset <= 0x1f7fff)
	{
		sectorSize = 32768;
		return (memOffset % 32768) == 0;
	}
	if (memOffset >= 0x1f8000 && memOffset <= 0x1fbfff)
	{
		sectorSize = 8192;
		return (memOffset % 8192) == 0;
	}
	if (memOffset >= 0x1fc000 && memOffset <= 0x1fffff)
	{
		sectorSize = 16384;
		return (memOffset % 16384) == 0;
	}
	return false;
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

	utils::debugPrint("bank reading\n");

	utils::dataContainer* dataContainer = new utils::dataContainer(length);
	xenium::flashReadStream(offset, (uint8_t*)dataContainer->data, length);
	xenium::setBank(originalBank);
	return dataContainer;
}

void xenium::eraseBank(bankEnum bank, uint32_t offset, uint32_t size)
{
	bankEnum originalBank = xenium::getBank();
	setBank(bank);

	utils::debugPrint("bank erasing\n");

	uint32_t memOffset = getBankMemOffset(bank);
	uint32_t bytesProcessed = 0;
    while (bytesProcessed < size)
	{
		uint32_t sectorSize;
		if (xenium::isEraseMemOffset(memOffset + offset, sectorSize))
		{
			sectorErase(offset);
			flashReset();
			offset += sectorSize;
			bytesProcessed += sectorSize;
			continue;
		}
		offset++;
		bytesProcessed++;
	}

	xenium::setBank(originalBank);
}

void xenium::writeBank(bankEnum bank, uint32_t offset, utils::dataContainer* dataContainer, uint32_t dataContainerOffset, uint32_t dataContainerSize)
{
	bankEnum originalBank = xenium::getBank();
	xenium::setBank(bank);

	utils::debugPrint("bank flashing\n");
    for (uint32_t i = 0; i < min(dataContainer->size, dataContainerSize); i++)
	{
		flashWriteByte(offset + i, (uint8_t)dataContainer->data[i + dataContainerOffset]);
    }
	flashReset();

	xenium::setBank(originalBank);
}

bool xenium::verifyBank(bankEnum bank, uint32_t offset, utils::dataContainer* dataContainer, uint32_t dataContainerOffset, uint32_t dataContainerSize)
{
	bankEnum originalBank = xenium::getBank();
	xenium::setBank(bank);

	utils::debugPrint("bank verifying\n");
	utils::dataContainer* writtenData = readBank(bank, offset, min(dataContainer->size, dataContainerSize));

	bool ok = true;
    for (uint32_t i = 0; i < min(dataContainer->size, dataContainerSize); i++)
	{
		if (writtenData->data[i] == dataContainer->data[i + dataContainerOffset])
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

utils::dataContainer* xenium::readFlash()
{
	utils::dataContainer* result = new utils::dataContainer(2 * 1024 * 1024);
	for (uint32_t i = 0; i < 7; i++)
	{
		xenium::bankEnum currentBank = xenium::bank1_256k;
		if (i == 1)
		{
			currentBank = xenium::bank2_256k;
		}
		else if (i == 2)
		{
			currentBank = xenium::bank3_256k;
		}
		else if (i == 3)
		{
			currentBank = xenium::bank3_256k;
		}
		else if (i == 4)
		{
			currentBank = xenium::bankBootloader;
		}
		else if (i == 5)
		{
			currentBank = xenium::bankXeniumOS;
		}
		else if (i == 6)
		{
			currentBank = xenium::bankRecovery;
		}

		uint32_t bankSize = xenium::getBankSize(currentBank);
		uint32_t memOffset = xenium::getBankMemOffset(currentBank);
		utils::dataContainer* bankData = xenium::readBank(currentBank, 0, bankSize);
		memcpy(result->data + memOffset, bankData->data, bankSize);
		delete(bankData);
	}

	return result;
}

void xenium::sendCharacter(uint8_t value)
{

#ifdef ENABLE_XENIUM

	if (initialized == false)
	{
		initialized = true;
		RtlInitializeCriticalSection(&mutex);
		uint8_t banking = inputByte(XENIUM_REGISTER_BANKING);
		mRegister = banking & 0xF0;
		mBankState = banking & 0x0F;
	}

	const uint8_t sckBit = 1 << 6; //0x40
	const uint8_t mosiBit = 1 << 4; //0x10

	//static LARGE_INTEGER clockFreq;
	//static bool spiInitialized = false;
	//if (spiInitialized == false)
	//{
	//	QueryPerformanceFrequency(&clockFreq);
	//	spiInitialized = true;
	//}

	if ((mRegister & sckBit) == 0 )
	{
		mRegister |= sckBit;
		setIoState();
		utils::uSleep(0x1c);
	}
	
	uint8_t i = 8;
	do
	{
		utils::uSleep(0x1c);

		if ((value & 0x80) > 0)
		{
			mRegister |= mosiBit;
		} 
		else
		{
			mRegister &= ~mosiBit;
		}
		setIoState();

		mRegister &= ~sckBit;
		setIoState();
		
		utils::uSleep(0x1c);

		mRegister |= sckBit;
		setIoState();

		value = value << 1;
		i--;
	}
	while (i > 0);

#endif
}