#include "modchipXenium.h"
#include "crc32.h"

#ifndef TOOLS
#include "settingsManager.h"
#endif

#define XENIUM_LPC_MEMORY_BASE 0xFF000000u

#define XENIUM_LCD_DELAY 2

#define XENIUM_REGISTER_BANKING 0x00EF
#define XENIUM_REGISTER_LED 0x00EE
#define XENIUM_SPI_MOSI_BIT 0x10
#define XENIUM_SPI_CS_BIT 0x20
#define XENIUM_SPI_CLK_BIT 0x40

#define XENIUM_BANK_TSOP 0
#define XENIUM_BANK_BOOTLOADER 1
#define XENIUM_BANK_PROMETHEOS1 2
#define XENIUM_BANK_PROMETHEOS2 10
#define XENIUM_BANK_SLOT1_256K 3
#define XENIUM_BANK_SLOT2_256K 4
#define XENIUM_BANK_SLOT3_256K 5
#define XENIUM_BANK_SLOT4_256K 6
#define XENIUM_BANK_SLOT1_512K 7
#define XENIUM_BANK_SLOT2_512K 8
#define XENIUM_BANK_SLOT1_1024K 9

#define XENIUM_SETTINGS_BANK XENIUM_BANK_PROMETHEOS2
#define XENIUM_SETTINGS_OFFSET (0x1f8000 - 0x1c0000)
#define XENIUM_INSTALLER_LOGO_BANK XENIUM_BANK_PROMETHEOS2
#define XENIUM_INSTALLER_LOGO_OFFSET (0x1fa000 - 0x1c0000)

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

//0x1c0000 - 0x1effff  Sector Size 65536
//0x1f0000 - 0x1f7fff  Sector Size 32768
//0x1f8000 - 0x1fbfff  Sector Size 8192
//0x1fc000 - 0x1fffff  Sector Size 16384

//0x1c0000 - 0x1dffff  Total 131072 (Protected - Installer Logo (IOpen Xenium))
//0x1e0000 - 0x1f7fff  Total 98304  (PrometheOS Continued)
//0x1f8000 - 0x1f9fff  Total 8192   (Protected)
//0x1fa000 - 0x1fbfff  Total 8192   (Settings)
//0x1fc000 - 0x1fffff  Total 16384  (Spare)

modchipXenium::modchipXenium()
{
	InitializeCriticalSection(&mMutex);
	mBank = XENIUM_BANK_BOOTLOADER;
	mSpi = inputByte(XENIUM_REGISTER_BANKING) & 0x70;
}

void modchipXenium::setLedColor(uint8_t ledColor)
{
    outputByte(XENIUM_REGISTER_LED, ledColor);
}

uint32_t modchipXenium::getSlotCount()
{
	return 4;
}

uint32_t modchipXenium::getFlashSize(bool recovery)
{
	return recovery ? 0 : (2 * 1024 * 1024);
}

bool modchipXenium::supportsLed()
{
	return true;
}

bool modchipXenium::supportsLcd()
{
	return true;
}

bool modchipXenium::supportsLcdInfo()
{
	return false;
}

bool modchipXenium::supportsLcdContrast()
{
	return true;
}

bool modchipXenium::supportsRecovery()
{
	return false;
}

void modchipXenium::disableRecovery()
{
}

bool modchipXenium::isValidBankSize(uint32_t size)
{
	return size == (256 * 1024) || size == (512 * 1024) || size == (1024 * 1024);
}

uint32_t modchipXenium::getBankSize(uint8_t bank)
{
	if (bank == XENIUM_BANK_BOOTLOADER) 
	{
		return 256 * 1024;
	}
	if (bank == XENIUM_BANK_PROMETHEOS1) 
	{
		return 512 * 1024;
	}
	if (bank == XENIUM_BANK_PROMETHEOS2) 
	{
		return 256 * 1024;
	}
	if (bank == XENIUM_BANK_SLOT1_256K) 
	{
		return 256 * 1024;
	}
	if (bank == XENIUM_BANK_SLOT2_256K) 
	{
		return 256 * 1024;
	}
	if (bank == XENIUM_BANK_SLOT3_256K) 
	{
		return 256 * 1024;
	}
	if (bank == XENIUM_BANK_SLOT4_256K) 
	{
		return 256 * 1024;
	}
	if (bank == XENIUM_BANK_SLOT1_512K) 
	{
		return 512 * 1024;
	}
	if (bank == XENIUM_BANK_SLOT2_512K) 
	{
		return 512 * 1024;
	}
	if (bank == XENIUM_BANK_SLOT1_1024K) 
	{
		return 1024 * 1024;
	}
	return 0;
}

uint32_t modchipXenium::getBankMemOffset(uint8_t bank)
{
	if (bank == XENIUM_BANK_SLOT1_256K)
	{
		return 0x000000;
	}
	if (bank == XENIUM_BANK_SLOT2_256K)
	{
		return 0x040000;
	}
	if (bank == XENIUM_BANK_SLOT2_256K)
	{
		return 0x080000;
	}
	if (bank == XENIUM_BANK_SLOT4_256K)
	{
		return 0x0c0000;
	}
	if (bank == XENIUM_BANK_SLOT1_512K)
	{
		return 0x000000;
	}
	if (bank == XENIUM_BANK_SLOT2_512K)
	{
		return 0x080000;
	}
	if (bank == XENIUM_BANK_SLOT1_1024K)
	{
		return 0x000000;
	}
	if (bank == XENIUM_BANK_BOOTLOADER)
	{
		return 0x180000;
	}
	if (bank == XENIUM_BANK_PROMETHEOS1)
	{
		return 0x100000;
	}	
	if (bank == XENIUM_BANK_PROMETHEOS2)
	{
		return 0x1c0000;
	}
	return 0;
}

uint8_t modchipXenium::getBankFromIdAndSlots(uint8_t id, uint8_t slots)
{
	if (id == 0 && slots == 1)
	{
		return XENIUM_BANK_SLOT1_256K;
	}
	if (id == 1 && slots == 1)
	{
		return XENIUM_BANK_SLOT2_256K;
	}
	if (id == 2 && slots == 1)
	{
		return XENIUM_BANK_SLOT3_256K;
	}
	if (id == 3 && slots == 1)
	{
		return XENIUM_BANK_SLOT4_256K;
	}
	if (id == 0 && slots == 2)
	{
		return XENIUM_BANK_SLOT1_512K;
	}
	if (id == 2 && slots == 2)
	{
		return XENIUM_BANK_SLOT2_512K;
	}
	if (id == 0 && slots == 4)
	{
		return XENIUM_BANK_SLOT1_1024K;
	}
	return 0;
}

utils::dataContainer* modchipXenium::readBank(uint8_t bank)
{
	setBank(bank); 
	uint32_t bankSize = getBankSize(bank);
	utils::dataContainer* dataContainer = new utils::dataContainer(bankSize);
    volatile uint8_t* lpcMemMap = (volatile uint8_t *)XENIUM_LPC_MEMORY_BASE;
    memcpy(dataContainer->data, (void*)&lpcMemMap[0], bankSize);
	setBank(XENIUM_BANK_BOOTLOADER);
	return dataContainer;
}

void modchipXenium::eraseBank(uint8_t bank)
{
	setBank(bank);

	setLedColor(LED_COLOR_AMBER);

	volatile uint8_t* lpcMemMap = (volatile uint8_t *)XENIUM_LPC_MEMORY_BASE;

	uint32_t memOffset = getBankMemOffset(bank);
	uint32_t bankSize = getBankSize(bank);

	uint32_t offset = 0;
    while (offset < bankSize)
	{
		if (isEraseMemOffset(memOffset + offset))
		{
			sectorErase(offset);
		}
		offset += 8192;
	}

	setBank(XENIUM_BANK_BOOTLOADER);

#ifndef TOOLS
	setLedColor(settingsManager::getLedColor());
#else
	setLedColor(LED_COLOR_GREEN);
#endif
}

void modchipXenium::writeBank(uint8_t bank, utils::dataContainer* dataContainer)
{
	setBank(bank);

	setLedColor(LED_COLOR_BLUE);

	volatile uint8_t* lpcMemMap = (volatile uint8_t *)XENIUM_LPC_MEMORY_BASE;

	uint32_t memOffset = getBankMemOffset(bank);

    for (uint32_t i = 0; i < dataContainer->size; i++)
	{
		if (isProtectedMemOffset(memOffset + i) == true)
		{
			continue;
		}
		uint8_t value = (uint8_t)dataContainer->data[i];
		lpcMemMap[0xAAAA] = 0xAA;
		lpcMemMap[0x5555] = 0x55;
		lpcMemMap[0xAAAA] = 0xA0;
		lpcMemMap[i] = value;
		while (lpcMemMap[i] != value);
    }
	lpcMemMap[0] = 0xf0;

	setBank(XENIUM_BANK_BOOTLOADER);

#ifndef TOOLS
	setLedColor(settingsManager::getLedColor());
#else
	setLedColor(LED_COLOR_GREEN);
#endif
}

bool modchipXenium::verifyBank(uint8_t bank, utils::dataContainer* dataContainer)
{
	setBank(bank);

	utils::dataContainer* writtenData = readBank(bank);

	setLedColor(LED_COLOR_PURPLE);

	volatile uint8_t* lpcMemMap = (volatile uint8_t *)XENIUM_LPC_MEMORY_BASE;

	uint32_t memOffset = getBankMemOffset(bank);

	bool ok = true;
    for (uint32_t i = 0; i < dataContainer->size; i++)
	{
		if (isProtectedMemOffset(memOffset + i) == true)
		{
			continue;
		}
		if (writtenData->data[i] == dataContainer->data[i])
		{
			continue;
		}
		ok = false;
		break;
    }

	delete(writtenData);

	setBank(XENIUM_BANK_BOOTLOADER);

#ifndef TOOLS
	setLedColor(settingsManager::getLedColor());
#else
	setLedColor(LED_COLOR_GREEN);
#endif
	return ok;
}

uint8_t modchipXenium::getFlashBankCount(bool recovery)
{
	return recovery ? 0 : 7;
}

uint8_t modchipXenium::getFlashBank(bool recovery, uint8_t bank)
{
	const uint8_t banks[] = { 
		XENIUM_BANK_SLOT1_256K, 
		XENIUM_BANK_SLOT2_256K, 
		XENIUM_BANK_SLOT3_256K, 
		XENIUM_BANK_SLOT4_256K,
		XENIUM_BANK_BOOTLOADER,
		XENIUM_BANK_PROMETHEOS1,
		XENIUM_BANK_PROMETHEOS2,
	};
	return recovery ? 0 : banks[bank];
}

utils::dataContainer* modchipXenium::readFlash(bool recovery)
{
	utils::dataContainer* result = new utils::dataContainer(getFlashSize(recovery));
	for (uint8_t i = 0; i < getFlashBankCount(recovery); i++)
	{
		uint8_t bank = getFlashBank(recovery, i);
		uint32_t memOffset = getBankMemOffset(bank);
		utils::dataContainer* bankData = readBank(bank);
		memcpy(result->data + memOffset, bankData->data, bankData->size);
		delete(bankData);
	}
	return result;
}

void modchipXenium::launchBank(uint8_t bank, uint8_t ledColor)
{
	DWORD scratch;
	HalReadSMBusByte(SMBDEV_PIC16L, PIC16L_CMD_SCRATCH_REGISTER, &scratch);
	HalWriteSMBusByte(SMBDEV_PIC16L, PIC16L_CMD_SCRATCH_REGISTER, scratch & ~SCRATCH_REGISTER_BITVALUE_NO_ANIMATION);
	setBank(bank);
	setLedColor(ledColor);
	HalReturnToFirmware(RETURN_FIRMWARE_REBOOT);
}

void modchipXenium::launchTsop()
{
	DWORD scratch;
	HalReadSMBusByte(SMBDEV_PIC16L, PIC16L_CMD_SCRATCH_REGISTER, &scratch);
	HalWriteSMBusByte(SMBDEV_PIC16L, PIC16L_CMD_SCRATCH_REGISTER, scratch & ~SCRATCH_REGISTER_BITVALUE_NO_ANIMATION);
	setBank(XENIUM_BANK_TSOP);
	setLedColor(LED_COLOR_OFF);
	HalReturnToFirmware(RETURN_FIRMWARE_REBOOT);
}

void modchipXenium::launchRecovery()
{
}

#ifndef TOOLS

void modchipXenium::loadSettings(settingsState& settings)
{
	setBank(XENIUM_SETTINGS_BANK); 

	setLedColor(LED_COLOR_WHITE);

    volatile uint8_t* lpcMemMap = (volatile uint8_t *)XENIUM_LPC_MEMORY_BASE;

    memcpy(&settings, (void*)&lpcMemMap[XENIUM_SETTINGS_OFFSET], sizeof(settings));
	uint32_t checksum = crc32::calculate(((char*)&settings) + sizeof(uint32_t), sizeof(settings) - sizeof(uint32_t));

	setBank(XENIUM_BANK_BOOTLOADER);

	if (checksum != settings.checksum || versioning::compareVersion(settings.version, settingsManager::getVersion()) != 0)
	{
		settingsManager::initSettings();
		settingsManager::saveSettings();
	}

	setLedColor(settingsManager::getLedColor());
}

void modchipXenium::saveSettings(settingsState settings) 
{
	setBank(XENIUM_SETTINGS_BANK); 

	settings.checksum = crc32::calculate(((const char*)&settings) + sizeof(uint32_t), sizeof(settings) - sizeof(uint32_t));
	utils::dataContainer* settingsData = new utils::dataContainer((char*)&settings, sizeof(settings), sizeof(settings));

	volatile uint8_t* lpcMemMap = (volatile uint8_t *)XENIUM_LPC_MEMORY_BASE;

	setLedColor(LED_COLOR_AMBER);
	sectorErase(XENIUM_SETTINGS_OFFSET);

	setLedColor(LED_COLOR_BLUE);
    for (uint32_t i = 0; i < settingsData->size; i++)
	{
		uint8_t value = (uint8_t)settingsData->data[i];
		lpcMemMap[0xAAAA] = 0xAA;
		lpcMemMap[0x5555] = 0x55;
		lpcMemMap[0xAAAA] = 0xA0;
		lpcMemMap[XENIUM_SETTINGS_OFFSET + i] = value;
		while (lpcMemMap[XENIUM_SETTINGS_OFFSET + i] != value);
    }
	lpcMemMap[0] = 0xF0;

	setBank(XENIUM_BANK_BOOTLOADER);

	setLedColor(settingsManager::getLedColor());
}

#endif

utils::dataContainer* modchipXenium::getInstallerLogo()
{
	setBank(XENIUM_INSTALLER_LOGO_BANK); 

	setLedColor(LED_COLOR_WHITE);

	utils::dataContainer* installerLogo = new utils::dataContainer(32768);

	volatile uint8_t* lpcMemMap = (volatile uint8_t *)XENIUM_LPC_MEMORY_BASE;
    memcpy(installerLogo->data, (void*)&lpcMemMap[XENIUM_INSTALLER_LOGO_OFFSET], installerLogo->size);

	setBank(XENIUM_BANK_BOOTLOADER);

#ifndef TOOLS
	setLedColor(settingsManager::getLedColor());
#else
	setLedColor(LED_COLOR_GREEN);
#endif
	return installerLogo;
}

void modchipXenium::lcdSendCharacter(uint8_t value, uint8_t command)
{
	EnterCriticalSection(&mMutex);

	if ((mSpi & XENIUM_SPI_CLK_BIT) == 0 )
	{
		mSpi |= XENIUM_SPI_CLK_BIT;
		outputByte(XENIUM_REGISTER_BANKING, (mSpi | mBank));
		utils::uSleep(0x1c);
	}
	
	uint8_t i = 8;
	do
	{
		utils::uSleep(0x1c);

		if ((value & 0x80) > 0)
		{
			mSpi |= XENIUM_SPI_MOSI_BIT;
		} 
		else
		{
			mSpi &= ~XENIUM_SPI_MOSI_BIT;
		}
		outputByte(XENIUM_REGISTER_BANKING, (mSpi | mBank));

		mSpi &= ~XENIUM_SPI_CLK_BIT;
		outputByte(XENIUM_REGISTER_BANKING, (mSpi | mBank));
		utils::uSleep(0x1c);

		mSpi |= XENIUM_SPI_CLK_BIT;
		outputByte(XENIUM_REGISTER_BANKING, (mSpi | mBank));

		value = value << 1;
		i--;
	}
	while (i > 0);

	LeaveCriticalSection(&mMutex);
}

void modchipXenium::lcdSetCursorPosition(uint8_t row, uint8_t col)
{
	if (row > 3) 
	{
		row = 3;
	}
	if (col > 19)
	{
		col = 19; 
	}
	lcdSendCharacter(17, 0);
	lcdSendCharacter(col, 0);
	lcdSendCharacter(row, 0);
	Sleep(XENIUM_LCD_DELAY);
}

void modchipXenium::lcdInit(uint8_t backlight, uint8_t contrast)
{
	// show display
	lcdSendCharacter(3, 0); 
	Sleep(XENIUM_LCD_DELAY);

	// hide cursor
	lcdSendCharacter(4, 0);
	Sleep(XENIUM_LCD_DELAY);

	// scroll off
	lcdSendCharacter(20, 0); 
	Sleep(XENIUM_LCD_DELAY);

	// wrap off
	lcdSendCharacter(24, 0);
	Sleep(XENIUM_LCD_DELAY);

	lcdSetBacklight(backlight);
	lcdSetContrast(contrast);
	Sleep(XENIUM_LCD_DELAY);
}

void modchipXenium::lcdPrintMessage(const char* message)
{
	//const uint8_t LCD[256] = 
 //   { //HD44780 charset ROM code A00
 //       0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
 //       0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
 //       0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
 //       0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
 //       0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,
 //       0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x5b, 0xa4, 0x5d, 0x5e, 0x5f,
 //       0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
 //       0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0xb0, 0x20,
 //       0x20, 0x20, 0x2c, 0x20, 0x22, 0x20, 0x20, 0x20, 0x5e, 0x20, 0x53, 0x3c, 0x20, 0x20, 0x5a, 0x20,
 //       0x20, 0x27, 0x27, 0x22, 0x22, 0xa5, 0xb0, 0xb0, 0xb0, 0x20, 0x73, 0x3e, 0x20, 0x20, 0x7a, 0x59,
 //       0xff, 0x21, 0x20, 0x20, 0x20, 0x5c, 0x7c, 0x20, 0x22, 0x20, 0x20, 0xff, 0x0E, 0x0A, 0x09, 0x08, // Custom characters
 //       0xdf, 0x20, 0x20, 0x20, 0x27, 0xe4, 0x20, 0xa5, 0x20, 0x20, 0xdf, 0x3e, 0x20, 0x20, 0x20, 0x3f,
 //       0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x20, 0x43, 0x45, 0x45, 0x45, 0x45, 0x49, 0x49, 0x49, 0x49,
 //       0x44, 0x4e, 0x4f, 0x4f, 0x4f, 0x4f, 0x4f, 0x78, 0x30, 0x55, 0x55, 0x55, 0x55, 0x59, 0x20, 0xe2,
 //       0x61, 0x61, 0x61, 0x61, 0xe1, 0x61, 0x20, 0x63, 0x65, 0x65, 0x65, 0x65, 0x69, 0x69, 0x69, 0x69,
 //       0x6f, 0x6e, 0x6f, 0x6f, 0x6f, 0x6f, 0x6f, 0xfd, 0x6f, 0x75, 0x75, 0xfb, 0xf5, 0x79, 0x20, 0x79
 //   };

	for (int i = 0; i < (int)strlen(message); ++i)
	{
		uint8_t cLCD = message[i];
		//cLCD = LCD[cLCD];
		lcdSendCharacter(cLCD, 0);
		Sleep(XENIUM_LCD_DELAY);
	}
}

void modchipXenium::lcdSetBacklight(uint8_t value)
{
	if (value > 100)
	{
		value = 100;
	}
	lcdSendCharacter(14, 0);
	lcdSendCharacter(value, 0);
	Sleep(XENIUM_LCD_DELAY);
}

void modchipXenium::lcdSetContrast(uint8_t value)
{
	if (value > 100)
	{
		value = 100;
	}
	lcdSendCharacter(15, 0);
	lcdSendCharacter(value, 0);
	Sleep(XENIUM_LCD_DELAY);
}

//reboot lcd
//context::getModchip()->sendLcdCharacter(26); 
//context::getModchip()->sendLcdCharacter(26);
//Sleep(SPI_DELAY);

// Private

void modchipXenium::setBank(uint8_t bank)
{
	EnterCriticalSection(&mMutex);
	mBank = bank;
    outputByte(XENIUM_REGISTER_BANKING, (mSpi | mBank));
	LeaveCriticalSection(&mMutex);
}

uint8_t modchipXenium::getBank()
{
	EnterCriticalSection(&mMutex);
	mBank = (inputByte(XENIUM_REGISTER_BANKING) & 0x0f);
	LeaveCriticalSection(&mMutex);
	return mBank;
}

bool modchipXenium::isEraseMemOffset(uint32_t memOffset)
{
	if (memOffset >= 0x000000 && memOffset <= 0x1effff)
	{
		return (memOffset % 65536) == 0;
	}
	if (memOffset >= 0x1f0000 && memOffset <= 0x1f7fff)
	{
		return (memOffset % 32768) == 0;
	}
	if (memOffset >= 0x1f8000 && memOffset <= 0x1fbfff)
	{
		return (memOffset % 8192) == 0;
	}
	if (memOffset >= 0x1fc000 && memOffset <= 0x1fffff)
	{
		return (memOffset % 16384) == 0;
	}
	return false;
}

bool modchipXenium::isProtectedMemOffset(uint32_t memOffset)
{
	if (memOffset >= 0x1c0000 && memOffset <= 0x1dffff)
	{
		return true;
	}
	return false;
}

void modchipXenium::sectorErase(uint32_t offset)
{
	if (isProtectedMemOffset(offset))
	{
		return;
	}
	volatile uint8_t* lpcMemMap = (volatile uint8_t *)XENIUM_LPC_MEMORY_BASE;
	lpcMemMap[0xAAAA] = 0xAA;
	lpcMemMap[0x5555] = 0x55;
	lpcMemMap[0xAAAA] = 0x80;
	lpcMemMap[0xAAAA] = 0xAA;
	lpcMemMap[0x5555] = 0x55;
	lpcMemMap[offset] = 0x30;
	while(lpcMemMap[0] != lpcMemMap[0]);
	lpcMemMap[0] = 0xF0;
}
