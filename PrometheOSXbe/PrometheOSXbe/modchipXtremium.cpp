#include "modchipXtremium.h"
#include "crc32.h"
#include "settingsManager.h"
#include "globalDefines.h"
#include "stringUtility.h"

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

//0x1c0000 - 0x1dffff  Total 131072 (Protected - Installer Logo (IOpen Xtremium))
//0x1e0000 - 0x1f7fff  Total 98304  (PrometheOS Continued)
//0x1f8000 - 0x1f9fff  Total 8192   (Protected)
//0x1fa000 - 0x1fbfff  Total 8192   (Settings)
//0x1fc000 - 0x1fffff  Total 16384  (Spare)



modchipXtremium::modchipXtremium()
{
	InitializeCriticalSection(&mMutex);
	uint8_t bank = XTREMIUM_BANK_BOOTLOADER;
	displaySPI2PARXtremium::getInstance()->setBankValue(bank);
	uint8_t spi = inputByte(XTREMIUM_REGISTER_BANKING) & 0x70;
	displaySPI2PARXtremium::getInstance()->setSpiValue(spi);
}

void modchipXtremium::setLedColor(uint8_t ledColor)
{
    outputByte(XTREMIUM_REGISTER_LED, ledColor);
}

uint32_t modchipXtremium::getSlotCount()
{
	return 4;
}

uint32_t modchipXtremium::getFlashSize(bool recovery)
{
	return recovery ? 0 : (2 * 1024 * 1024);
}

void modchipXtremium::disableRecovery()
{
}

bool modchipXtremium::isValidBankSize(uint32_t size)
{
	return size == (256 * 1024) || size == (512 * 1024) || size == (1024 * 1024);
}

bool modchipXtremium::isValidFlashSize(bool recovery, uint32_t size)
{
	return size == getFlashSize(recovery);
}

uint32_t modchipXtremium::getBankSize(uint8_t bank)
{
	if (bank == XTREMIUM_BANK_BOOTLOADER) 
	{
		return 256 * 1024;
	}
	if (bank == XTREMIUM_BANK_PROMETHEOS1) 
	{
		return 512 * 1024;
	}
	if (bank == XTREMIUM_BANK_PROMETHEOS2) 
	{
		return 256 * 1024;
	}
	if (bank == XTREMIUM_BANK_SLOT1_256K) 
	{
		return 256 * 1024;
	}
	if (bank == XTREMIUM_BANK_SLOT2_256K) 
	{
		return 256 * 1024;
	}
	if (bank == XTREMIUM_BANK_SLOT3_256K) 
	{
		return 256 * 1024;
	}
	if (bank == XTREMIUM_BANK_SLOT4_256K) 
	{
		return 256 * 1024;
	}
	if (bank == XTREMIUM_BANK_SLOT1_512K) 
	{
		return 512 * 1024;
	}
	if (bank == XTREMIUM_BANK_SLOT2_512K) 
	{
		return 512 * 1024;
	}
	if (bank == XTREMIUM_BANK_SLOT1_1024K) 
	{
		return 1024 * 1024;
	}
	return 0;
}

uint32_t modchipXtremium::getBankMemOffset(uint8_t bank)
{
	if (bank == XTREMIUM_BANK_SLOT1_256K)
	{
		return 0x000000;
	}
	if (bank == XTREMIUM_BANK_SLOT2_256K)
	{
		return 0x040000;
	}
	if (bank == XTREMIUM_BANK_SLOT2_256K)
	{
		return 0x080000;
	}
	if (bank == XTREMIUM_BANK_SLOT4_256K)
	{
		return 0x0c0000;
	}
	if (bank == XTREMIUM_BANK_SLOT1_512K)
	{
		return 0x000000;
	}
	if (bank == XTREMIUM_BANK_SLOT2_512K)
	{
		return 0x080000;
	}
	if (bank == XTREMIUM_BANK_SLOT1_1024K)
	{
		return 0x000000;
	}
	if (bank == XTREMIUM_BANK_BOOTLOADER)
	{
		return 0x180000;
	}
	if (bank == XTREMIUM_BANK_PROMETHEOS1)
	{
		return 0x100000;
	}	
	if (bank == XTREMIUM_BANK_PROMETHEOS2)
	{
		return 0x1c0000;
	}
	return 0;
}

uint32_t modchipXtremium::getBankStartOffset(uint8_t bank)
{
	return 0;
}

uint8_t modchipXtremium::getBankFromIdAndSlots(uint8_t id, uint8_t slots)
{
	if (id == 0 && slots == 1)
	{
		return XTREMIUM_BANK_SLOT1_256K;
	}
	if (id == 1 && slots == 1)
	{
		return XTREMIUM_BANK_SLOT2_256K;
	}
	if (id == 2 && slots == 1)
	{
		return XTREMIUM_BANK_SLOT3_256K;
	}
	if (id == 3 && slots == 1)
	{
		return XTREMIUM_BANK_SLOT4_256K;
	}
	if (id == 0 && slots == 2)
	{
		return XTREMIUM_BANK_SLOT1_512K;
	}
	if (id == 2 && slots == 2)
	{
		return XTREMIUM_BANK_SLOT2_512K;
	}
	if (id == 0 && slots == 4)
	{
		return XTREMIUM_BANK_SLOT1_1024K;
	}
	return 0;
}

utils::dataContainer* modchipXtremium::readBank(uint8_t bank)
{
	setBank(bank); 
	uint32_t bankSize = getBankSize(bank);
	utils::dataContainer* dataContainer = new utils::dataContainer(bankSize);
    volatile uint8_t* lpcMemMap = (volatile uint8_t *)(LPC_MEMORY_BASE + getBankStartOffset(bank));
    memcpy(dataContainer->data, (void*)&lpcMemMap[0], bankSize);
	setBank(XTREMIUM_BANK_BOOTLOADER);
	return dataContainer;
}

void modchipXtremium::eraseBank(uint8_t bank)
{
	setBank(bank);

	setLedColor(LED_COLOR_AMBER);

	volatile uint8_t* lpcMemMap = (volatile uint8_t *)LPC_MEMORY_BASE;

	uint32_t memOffset = getBankMemOffset(bank);
	uint32_t startOffset = getBankStartOffset(bank);
	uint32_t bankSize = getBankSize(bank);

	uint32_t offset = 0;
    while (offset < bankSize)
	{
		if (isEraseMemOffset(memOffset + startOffset + offset))
		{
			sectorErase(offset);
		}
		offset += 8192;
	}

	setBank(XTREMIUM_BANK_BOOTLOADER);

#ifndef TOOLS
	setLedColor(settingsManager::getLedColor());
#else
	setLedColor(LED_COLOR_GREEN);
#endif
}

void modchipXtremium::writeBank(uint8_t bank, utils::dataContainer* dataContainer)
{
	setBank(bank);

	setLedColor(LED_COLOR_BLUE);

	volatile uint8_t* lpcMemMap = (volatile uint8_t *)(LPC_MEMORY_BASE + getBankStartOffset(bank));

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

	setBank(XTREMIUM_BANK_BOOTLOADER);

#ifndef TOOLS
	setLedColor(settingsManager::getLedColor());
#else
	setLedColor(LED_COLOR_GREEN);
#endif
}

bool modchipXtremium::verifyBank(uint8_t bank, utils::dataContainer* dataContainer)
{
	setBank(bank);

	utils::dataContainer* writtenData = readBank(bank);

	setLedColor(LED_COLOR_PURPLE);

	volatile uint8_t* lpcMemMap = (volatile uint8_t *)LPC_MEMORY_BASE;

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

	setBank(XTREMIUM_BANK_BOOTLOADER);

#ifndef TOOLS
	setLedColor(settingsManager::getLedColor());
#else
	setLedColor(LED_COLOR_GREEN);
#endif
	return ok;
}

uint8_t modchipXtremium::getFlashBankCount(bool recovery)
{
	return recovery ? 0 : 7;
}

uint8_t modchipXtremium::getFlashBank(bool recovery, uint8_t bank)
{
	const uint8_t banks[] = { 
		XTREMIUM_BANK_SLOT1_256K, 
		XTREMIUM_BANK_SLOT2_256K, 
		XTREMIUM_BANK_SLOT3_256K, 
		XTREMIUM_BANK_SLOT4_256K,
		XTREMIUM_BANK_BOOTLOADER,
		XTREMIUM_BANK_PROMETHEOS1,
		XTREMIUM_BANK_PROMETHEOS2,
	};
	return recovery ? 0 : banks[bank];
}

bankType modchipXtremium::getFlashBankType(bool recovery, uint8_t bank)
{
	const bankType banks[] = { 
		bankTypeUser, 
		bankTypeUser, 
		bankTypeUser, 
		bankTypeUser,
		bankTypeSystem,
		bankTypeSystem,
		bankTypeSystem,
	};
	return recovery ? bankTypeUser : banks[bank];
}

utils::dataContainer* modchipXtremium::readFlash(bool recovery)
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

void modchipXtremium::launchBank(uint8_t bank, uint8_t ledColor)
{
	DWORD scratch;
	HalReadSMBusByte(SMBDEV_PIC16L, PIC16L_CMD_SCRATCH_REGISTER, &scratch);
	HalWriteSMBusByte(SMBDEV_PIC16L, PIC16L_CMD_SCRATCH_REGISTER, scratch & ~SCRATCH_REGISTER_BITVALUE_NO_ANIMATION);
	setBank(bank);
	setLedColor(ledColor);
	HalReturnToFirmware(RETURN_FIRMWARE_REBOOT);
}

void modchipXtremium::launchTsop()
{
	DWORD scratch;
	HalReadSMBusByte(SMBDEV_PIC16L, PIC16L_CMD_SCRATCH_REGISTER, &scratch);
	HalWriteSMBusByte(SMBDEV_PIC16L, PIC16L_CMD_SCRATCH_REGISTER, scratch & ~SCRATCH_REGISTER_BITVALUE_NO_ANIMATION);
	setBank(XTREMIUM_BANK_TSOP);
	setLedColor(LED_COLOR_OFF);
	HalReturnToFirmware(RETURN_FIRMWARE_REBOOT);
}

void modchipXtremium::launchRecovery()
{
}

#ifndef TOOLS

void modchipXtremium::loadSettings(settingsState& settings)
{
	setBank(XTREMIUM_SETTINGS_BANK); 

	setLedColor(LED_COLOR_WHITE);

    volatile uint8_t* lpcMemMap = (volatile uint8_t *)LPC_MEMORY_BASE;

    memcpy(&settings, (void*)&lpcMemMap[XTREMIUM_SETTINGS_OFFSET], sizeof(settings));
	uint32_t checksum = crc32::calculate(((uint8_t*)&settings) + sizeof(uint32_t), sizeof(settings) - sizeof(uint32_t));

	setBank(XTREMIUM_BANK_BOOTLOADER);

	if (checksum != settings.checksum || versioning::compareVersion(settings.version, settingsManager::getVersion()) != 0)
	{
		settingsManager::initSettings();
		settingsManager::saveSettings();
	}

	setLedColor(settingsManager::getLedColor());
}

void modchipXtremium::saveSettings(settingsState& settings) 
{
	setBank(XTREMIUM_SETTINGS_BANK); 

	settings.checksum = crc32::calculate(((uint8_t*)&settings) + sizeof(uint32_t), sizeof(settings) - sizeof(uint32_t));
	utils::dataContainer* settingsData = new utils::dataContainer((char*)&settings, sizeof(settings), sizeof(settings));

	volatile uint8_t* lpcMemMap = (volatile uint8_t *)LPC_MEMORY_BASE;

	setLedColor(LED_COLOR_AMBER);
	sectorErase(XTREMIUM_SETTINGS_OFFSET);

	setLedColor(LED_COLOR_BLUE);
    for (uint32_t i = 0; i < settingsData->size; i++)
	{
		uint8_t value = (uint8_t)settingsData->data[i];
		lpcMemMap[0xAAAA] = 0xAA;
		lpcMemMap[0x5555] = 0x55;
		lpcMemMap[0xAAAA] = 0xA0;
		lpcMemMap[XTREMIUM_SETTINGS_OFFSET + i] = value;
		while (lpcMemMap[XTREMIUM_SETTINGS_OFFSET + i] != value);
    }
	lpcMemMap[0] = 0xF0;

	setBank(XTREMIUM_BANK_BOOTLOADER);

	setLedColor(settingsManager::getLedColor());
}

#endif

utils::dataContainer* modchipXtremium::getInstallerLogo()
{
	setBank(XTREMIUM_INSTALLER_LOGO_BANK); 

	setLedColor(LED_COLOR_WHITE);

	utils::dataContainer* installerLogo = new utils::dataContainer(32768);

	volatile uint8_t* lpcMemMap = (volatile uint8_t *)LPC_MEMORY_BASE;
    memcpy(installerLogo->data, (void*)&lpcMemMap[XTREMIUM_INSTALLER_LOGO_OFFSET], installerLogo->size);

	setBank(XTREMIUM_BANK_BOOTLOADER);

#ifndef TOOLS
	setLedColor(settingsManager::getLedColor());
#else
	setLedColor(LED_COLOR_GREEN);
#endif
	return installerLogo;
}

displayDriver* modchipXtremium::getDisplayDriver(bool current)
{
	uint8_t lcdMode = settingsManager::getLcdMode(current);
	uint8_t lcdModel = settingsManager::getLcdModel(current);
	if (lcdMode == 1)
	{
		if (lcdModel == 0)
		{
			return displayFactory::getDisplay(displayVariantSPI2PARXtremium);
		}
	}
	if (lcdMode == 2)
	{
		if (lcdModel == 0)
		{
			return displayFactory::getDisplay(displayVariantHD44780LPC);
		}
		if (lcdModel == 1)
		{
			return displayFactory::getDisplay(displayVariantLCDXXXXLPC);
		}
	}
	return NULL;
}

supportInfo modchipXtremium::getSupportInfo(bool current)
{
	uint8_t lcdMode = settingsManager::getLcdMode(current);
	supportInfo info;
	info.supportsLed = true;
	info.supportsLcd = true;

	bool lcdInfo = false;
	bool backlight = false;
	bool contrast = false;
	displayDriver* driver = getDisplayDriver(current);
	if (driver != NULL)
	{
		driver->getSupport(lcdInfo, backlight, contrast);
	}
	info.supportsLcdInfo = lcdInfo;
	info.supportsLcdBacklight = backlight;
	info.supportsLcdContrast = contrast;

	info.supportsRecovery = false;
	return info;
}

uint8_t modchipXtremium::getLcdModeCount()
{
	return 2;
}

char* modchipXtremium::getLcdModeString(uint8_t lcdMode)
{
	if (lcdMode == 1)
	{
		return strdup("SPI2PAR");
	}
	return strdup("Disabled");
}

uint8_t modchipXtremium::getLcdModelCount(bool current)
{
	uint8_t lcdMode = settingsManager::getLcdMode(current);
	if (lcdMode == 1)
	{
		return 1;
	}
	if (lcdMode == 2)
	{
		return 2;
	}
	return 0;
}

char* modchipXtremium::getLcdModelString(bool current, uint8_t lcdModel)
{
	displayDriver* driver = getDisplayDriver(current);
	if (driver == NULL)
	{
		return strdup("");
	}
	return driver->getModel();
}

uint8_t modchipXtremium::getLcdAddressCount(bool current)
{
	displayDriver* driver = getDisplayDriver(current);
	if (driver == NULL)
	{
		return 0;
	}
	return driver->getI2cAddressCount();
}

char* modchipXtremium::getLcdAddressString(bool current, uint8_t lcdAddress)
{
	displayDriver* driver = getDisplayDriver(current);
	if (driver == NULL)
	{
		return strdup("");
	}
	uint8_t address = driver->getI2cAddress(lcdAddress);
	char* result = stringUtility::formatString("0x%2X", address);
	return result;
}

void modchipXtremium::lcdSetCursorPosition(uint8_t row, uint8_t col)
{
	if (row > 3) 
	{
		row = 3;
	}
	if (col > 19)
	{
		col = 19; 
	}
	displayDriver* driver = getDisplayDriver(true);
	if (driver == NULL)
	{
		return;
	}
	EnterCriticalSection(&mMutex);
	driver->setAddress(settingsManager::getLcdAddress(true));
	driver->setCursorPosition(row, col);
	LeaveCriticalSection(&mMutex);
}

void modchipXtremium::lcdInit()
{
	displayDriver* driver = getDisplayDriver(true);
	if (driver == NULL)
	{
		return;
	}
	EnterCriticalSection(&mMutex);
	driver->setAddress(settingsManager::getLcdAddress(true));
	driver->init();
	LeaveCriticalSection(&mMutex);
}

void modchipXtremium::lcdPrintMessage(const char* message)
{
	displayDriver* driver = getDisplayDriver(true);
	if (driver == NULL)
	{
		return;
	}
	EnterCriticalSection(&mMutex);
	driver->setAddress(settingsManager::getLcdAddress(true));
	driver->printMessage(message);
	LeaveCriticalSection(&mMutex);
}

void modchipXtremium::lcdSetBacklight(uint8_t value)
{
	if (value > 100)
	{
		value = 100;
	}
	displayDriver* driver = getDisplayDriver(true);
	if (driver == NULL)
	{
		return;
	}
	EnterCriticalSection(&mMutex);
	driver->setAddress(settingsManager::getLcdAddress(true));
	driver->setBacklight(value);
	LeaveCriticalSection(&mMutex);
}

void modchipXtremium::lcdSetContrast(uint8_t value)
{
	if (value > 100)
	{
		value = 100;
	}
	displayDriver* driver = getDisplayDriver(true);
	if (driver == NULL)
	{
		return;
	}
	EnterCriticalSection(&mMutex);
	driver->setAddress(settingsManager::getLcdAddress(true));
	driver->setContrast(value);
	LeaveCriticalSection(&mMutex);
}

// Private

void modchipXtremium::setBank(uint8_t bank)
{
	EnterCriticalSection(&mMutex);
	displaySPI2PARXtremium::getInstance()->setBankValue(bank);
	uint8_t spi = displaySPI2PARXtremium::getInstance()->getSpiValue();
    outputByte(XTREMIUM_REGISTER_BANKING, (spi | bank));
	LeaveCriticalSection(&mMutex);
}

uint8_t modchipXtremium::getBank()
{
	EnterCriticalSection(&mMutex);
	uint8_t bank = (inputByte(XTREMIUM_REGISTER_BANKING) & 0x0f);
	displaySPI2PARXtremium::getInstance()->setBankValue(bank);
	LeaveCriticalSection(&mMutex);
	return bank;
}

bool modchipXtremium::isEraseMemOffset(uint32_t memOffset)
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

bool modchipXtremium::isProtectedMemOffset(uint32_t memOffset)
{
	if (memOffset >= 0x1c0000 && memOffset <= 0x1dffff)
	{
		return true;
	}
	return false;
}

void modchipXtremium::sectorErase(uint32_t offset)
{
	if (isProtectedMemOffset(offset))
	{
		return;
	}
	volatile uint8_t* lpcMemMap = (volatile uint8_t *)LPC_MEMORY_BASE;
	lpcMemMap[0xAAAA] = 0xAA;
	lpcMemMap[0x5555] = 0x55;
	lpcMemMap[0xAAAA] = 0x80;
	lpcMemMap[0xAAAA] = 0xAA;
	lpcMemMap[0x5555] = 0x55;
	lpcMemMap[offset] = 0x30;
	while(lpcMemMap[0] != lpcMemMap[0]);
	lpcMemMap[0] = 0xF0;
}
