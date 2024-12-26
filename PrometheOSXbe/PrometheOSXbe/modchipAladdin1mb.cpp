#include "modchipAladdin1mb.h"
#include "crc32.h"
#include "settingsManager.h"
#include "globalDefines.h"
#include "stringUtility.h"

//#define ALADDIN1MB_INSTALLER_LOGO_BANK ALADDIN1MB_BANK_PROMETHEOS2
//#define ALADDIN1MB_INSTALLER_LOGO_OFFSET (0x1f0000 - 0x1c0000)

//Bank_PrometheOS =  0x84  0x000000 - 0x07ffff  Sector Size 4096  Total  524288 (See below)
//Bank1_256k      =  0x86  0x080000 - 0x0bffff  Sector Size 4096  Total  262144
//Bank_Bootloader =  0x87  0x0C0000 - 0x0fffff  Sector Size 4096  Total  262144

//0x000000 - 0x07efff  Sector Size 4096  Total 520192 (PrometheOS)
//0x07f000 - 0x07ffff  Sector Size 4096  Total 4096   (Settings)

modchipAladdin1mb::modchipAladdin1mb()
{
	mBank = ALADDIN1MB_BANK_BOOTLOADER;
}

void modchipAladdin1mb::setLedColor(uint8_t ledColor)
{
    //outputByte(ALADDIN1MB_REGISTER_LED, ledColor);
}

uint32_t modchipAladdin1mb::getSlotCount()
{
	return 1;
}

uint32_t modchipAladdin1mb::getFlashSize(bool recovery)
{
	return recovery ? 0 : (1 * 1024 * 1024);
}

void modchipAladdin1mb::disableRecovery()
{
}

bool modchipAladdin1mb::isValidBankSize(uint32_t size)
{
	return size == (256 * 1024);
}

bool modchipAladdin1mb::isValidFlashSize(bool recovery, uint32_t size)
{
	return size == getFlashSize(recovery);
}

uint32_t modchipAladdin1mb::getBankSize(uint8_t bank)
{
	if (bank == ALADDIN1MB_BANK_BOOTLOADER) 
	{
		return 256 * 1024;
	}
	if (bank == ALADDIN1MB_BANK_PROMETHEOS) 
	{
		return 512 * 1024;
	}
	if (bank == ALADDIN1MB_BANK_SLOT1_256K) 
	{
		return 256 * 1024;
	}
	return 0;
}

uint32_t modchipAladdin1mb::getBankMemOffset(uint8_t bank)
{
	if (bank == ALADDIN1MB_BANK_SLOT1_256K)
	{
		return 0x080000;
	}
	if (bank == ALADDIN1MB_BANK_BOOTLOADER)
	{
		return 0x0c0000;
	}
	if (bank == ALADDIN1MB_BANK_PROMETHEOS)
	{
		return 0x000000;
	}
	return 0;
}

uint32_t modchipAladdin1mb::getBankStartOffset(uint8_t bank)
{
	return 0;
}

uint8_t modchipAladdin1mb::getBankFromIdAndSlots(uint8_t id, uint8_t slots)
{
	if (id == 0 && slots == 1)
	{
		return ALADDIN1MB_BANK_SLOT1_256K;
	}
	return 0;
}

utils::dataContainer* modchipAladdin1mb::readBank(uint8_t bank)
{
	setBank(bank); 
	uint32_t bankSize = getBankSize(bank);
	utils::dataContainer* dataContainer = new utils::dataContainer(bankSize);
    volatile uint8_t* lpcMemMap = (volatile uint8_t *)(LPC_MEMORY_BASE + getBankStartOffset(bank));
    memcpy(dataContainer->data, (void*)&lpcMemMap[0], bankSize);
	setBank(ALADDIN1MB_BANK_BOOTLOADER);
	return dataContainer;
}

void modchipAladdin1mb::eraseBank(uint8_t bank)
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
		offset += 4096;
	}

	setBank(ALADDIN1MB_BANK_BOOTLOADER);

#ifndef TOOLS
	setLedColor(settingsManager::getLedColor());
#else
	setLedColor(LED_COLOR_GREEN);
#endif
}

void modchipAladdin1mb::writeBank(uint8_t bank, utils::dataContainer* dataContainer)
{
	setBank(bank);

	setLedColor(LED_COLOR_BLUE);

	volatile uint8_t* lpcMemMap = (volatile uint8_t *)(LPC_MEMORY_BASE + getBankStartOffset(bank));

    for (uint32_t i = 0; i < dataContainer->size; i++)
	{
		uint8_t value = (uint8_t)dataContainer->data[i];
		lpcMemMap[0x5555] = 0xAA;
		lpcMemMap[0x2AAA] = 0x55;
		lpcMemMap[0x5555] = 0xA0;
		lpcMemMap[i] = value;
		while (lpcMemMap[i] != value);
    }
	lpcMemMap[0] = 0xF0;

	setBank(ALADDIN1MB_BANK_BOOTLOADER);

#ifndef TOOLS
	setLedColor(settingsManager::getLedColor());
#else
	setLedColor(LED_COLOR_GREEN);
#endif
}

bool modchipAladdin1mb::verifyBank(uint8_t bank, utils::dataContainer* dataContainer)
{
	setBank(bank);

	utils::dataContainer* writtenData = readBank(bank);

	setLedColor(LED_COLOR_PURPLE);

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

	setBank(ALADDIN1MB_BANK_BOOTLOADER);

#ifndef TOOLS
	setLedColor(settingsManager::getLedColor());
#else
	setLedColor(LED_COLOR_GREEN);
#endif
	return ok;
}

uint8_t modchipAladdin1mb::getFlashBankCount(bool recovery)
{
	return 3;
}

uint8_t modchipAladdin1mb::getFlashBank(bool recovery, uint8_t bank)
{
	const uint8_t banks[] = { 
		ALADDIN1MB_BANK_SLOT1_256K, 
		ALADDIN1MB_BANK_BOOTLOADER,
		ALADDIN1MB_BANK_PROMETHEOS
	};
	return recovery ? 0 : banks[bank];
}

bankType modchipAladdin1mb::getFlashBankType(bool recovery, uint8_t bank)
{
	const bankType banks[] = { 
		bankTypeUser, 
		bankTypeSystem,
		bankTypeSystem,
	};
	return recovery ? bankTypeUser : banks[bank];
}

utils::dataContainer* modchipAladdin1mb::readFlash(bool recovery)
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

void modchipAladdin1mb::launchBank(uint8_t bank, uint8_t ledColor)
{
	DWORD scratch;
	HalReadSMBusByte(SMBDEV_PIC16L, PIC16L_CMD_SCRATCH_REGISTER, &scratch);
	HalWriteSMBusByte(SMBDEV_PIC16L, PIC16L_CMD_SCRATCH_REGISTER, scratch & ~SCRATCH_REGISTER_BITVALUE_NO_ANIMATION);
	setBank(bank);
	setLedColor(ledColor);
	HalReturnToFirmware(RETURN_FIRMWARE_REBOOT);
}

void modchipAladdin1mb::launchTsop()
{
	DWORD scratch;
	HalReadSMBusByte(SMBDEV_PIC16L, PIC16L_CMD_SCRATCH_REGISTER, &scratch);
	HalWriteSMBusByte(SMBDEV_PIC16L, PIC16L_CMD_SCRATCH_REGISTER, scratch & ~SCRATCH_REGISTER_BITVALUE_NO_ANIMATION);
	setBank(ALADDIN1MB_BANK_TSOP);
	setLedColor(LED_COLOR_OFF);
	HalReturnToFirmware(RETURN_FIRMWARE_REBOOT);
}

void modchipAladdin1mb::launchRecovery()
{
}

#ifndef TOOLS

void modchipAladdin1mb::loadSettings(settingsState& settings)
{
	setBank(ALADDIN1MB_SETTINGS_BANK); 

	setLedColor(LED_COLOR_WHITE);

    volatile uint8_t* lpcMemMap = (volatile uint8_t *)LPC_MEMORY_BASE;

    memcpy(&settings, (void*)&lpcMemMap[ALADDIN1MB_SETTINGS_OFFSET], sizeof(settings));
	uint32_t checksum = crc32::calculate(((uint8_t*)&settings) + sizeof(uint32_t), sizeof(settings) - sizeof(uint32_t));

	setBank(ALADDIN1MB_BANK_BOOTLOADER);

	if (checksum != settings.checksum || versioning::compareVersion(settings.version, settingsManager::getVersion()) != 0)
	{
		settingsManager::initSettings();
		settingsManager::saveSettings();
	}

	setLedColor(settingsManager::getLedColor());
}

void modchipAladdin1mb::saveSettings(settingsState& settings) 
{
	setBank(ALADDIN1MB_SETTINGS_BANK); 

	settings.checksum = crc32::calculate(((uint8_t*)&settings) + sizeof(uint32_t), sizeof(settings) - sizeof(uint32_t));
	utils::dataContainer* settingsData = new utils::dataContainer((char*)&settings, sizeof(settings), sizeof(settings));

	volatile uint8_t* lpcMemMap = (volatile uint8_t *)LPC_MEMORY_BASE;

	setLedColor(LED_COLOR_AMBER);
	sectorErase(ALADDIN1MB_SETTINGS_OFFSET);

	setLedColor(LED_COLOR_BLUE);
    for (uint32_t i = 0; i < settingsData->size; i++)
	{
		uint8_t value = (uint8_t)settingsData->data[i];
		lpcMemMap[0x5555] = 0xAA;
		lpcMemMap[0x2AAA] = 0x55;
		lpcMemMap[0x5555] = 0xA0;
		lpcMemMap[ALADDIN1MB_SETTINGS_OFFSET + i] = value;
		while (lpcMemMap[ALADDIN1MB_SETTINGS_OFFSET + i] != value);
    }
	lpcMemMap[0] = 0xF0;

	setBank(ALADDIN1MB_BANK_BOOTLOADER);

	setLedColor(settingsManager::getLedColor());
}

#endif

utils::dataContainer* modchipAladdin1mb::getInstallerLogo()
{
	utils::dataContainer* installerLogo = new utils::dataContainer(32768);
	return installerLogo;
}

displayDriver* modchipAladdin1mb::getDisplayDriver(bool current)
{
	uint8_t lcdMode = settingsManager::getLcdMode(current);
	uint8_t lcdModel = settingsManager::getLcdModel(current);
	if (lcdMode == 1)
	{
		if (lcdModel == 0)
		{
			return displayFactory::getDisplay(displayVariantHD44780Aladdin);
		}
		if (lcdModel == 1)
		{
			return displayFactory::getDisplay(displayVariantSPI2PARAladdin);
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

supportInfo modchipAladdin1mb::getSupportInfo(bool current)
{
	uint8_t lcdMode = settingsManager::getLcdMode(current);
	uint8_t lcdModel = settingsManager::getLcdModel(current);

	supportInfo info;
	info.supportsLed = false;
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

uint8_t modchipAladdin1mb::getLcdModeCount()
{
	return 3;
}

char* modchipAladdin1mb::getLcdModeString(uint8_t lcdMode)
{
	if (lcdMode == 1)
	{
		return strdup("Aladdin");
	}
	if (lcdMode == 2)
	{
		return strdup("SMBUS");
	}
	return strdup("Disabled");
}

uint8_t modchipAladdin1mb::getLcdModelCount(bool current)
{
	uint8_t lcdMode = settingsManager::getLcdMode(current);
	if (lcdMode == 1)
	{
		return 2;
	}
	if (lcdMode == 2)
	{
		return 2;
	}
	return 0;
}

char* modchipAladdin1mb::getLcdModelString(bool current, uint8_t lcdModel)
{
	displayDriver* driver = getDisplayDriver(current);
	if (driver == NULL)
	{
		return strdup("");
	}
	return driver->getModel();
}

uint8_t modchipAladdin1mb::getLcdAddressCount(bool current)
{	
	displayDriver* driver = getDisplayDriver(current);
	if (driver == NULL)
	{
		return 0;
	}
	return driver->getI2cAddressCount();
}

char* modchipAladdin1mb::getLcdAddressString(bool current, uint8_t lcdAddress)
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

void modchipAladdin1mb::lcdSetCursorPosition(uint8_t row, uint8_t col)
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
	driver->setAddress(settingsManager::getLcdAddress(true));
	driver->setCursorPosition(row, col);
}

void modchipAladdin1mb::lcdInit()
{
	displayDriver* driver = getDisplayDriver(true);
	if (driver == NULL)
	{
		return;
	}
	driver->setAddress(settingsManager::getLcdAddress(true));
	driver->init();
}

void modchipAladdin1mb::lcdPrintMessage(const char* message)
{
	displayDriver* driver = getDisplayDriver(true);
	if (driver == NULL)
	{
		return;
	}
	driver->setAddress(settingsManager::getLcdAddress(true));
	driver->printMessage(message);
}

void modchipAladdin1mb::lcdSetBacklight(uint8_t value)
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
	driver->setAddress(settingsManager::getLcdAddress(true));
	driver->setBacklight(value);
}

void modchipAladdin1mb::lcdSetContrast(uint8_t value)
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
	driver->setAddress(settingsManager::getLcdAddress(true));
	driver->setContrast(value);
}

// Private

void modchipAladdin1mb::setBank(uint8_t bank)
{
	mBank = bank;
    outputByte(ALADDIN_REGISTER_BANKING, mBank);
}

uint8_t modchipAladdin1mb::getBank()
{
	return mBank;
}

bool modchipAladdin1mb::isEraseMemOffset(uint32_t memOffset)
{
	return (memOffset % 4096) == 0;
}

bool modchipAladdin1mb::isProtectedMemOffset(uint32_t memOffset)
{
	if (isProtectedMemOffset(memOffset))
	{
		return true;
	}
	return false;
}

void modchipAladdin1mb::sectorErase(uint32_t offset)
{
	volatile uint8_t* lpcMemMap = (volatile uint8_t *)LPC_MEMORY_BASE;
	lpcMemMap[0x5555] = 0xAA;
	lpcMemMap[0x2AAA] = 0x55;
	lpcMemMap[0x5555] = 0x80;
	lpcMemMap[0x5555] = 0xAA;
	lpcMemMap[0x2AAA] = 0x55;
	lpcMemMap[offset] = 0x30;
	while(lpcMemMap[0] != lpcMemMap[0]);
    lpcMemMap[0x5555] = 0xF0;
}
