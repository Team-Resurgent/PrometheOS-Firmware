#include "modchipAladdin2mb.h"
#include "crc32.h"
#include "settingsManager.h"
#include "globalDefines.h"
#include "stringUtility.h"



//#define READ_ARRAY_CMD      0xff
//#define RD_STATUS_CMD       0x70
//#define CLR_STATUS_CMD      0x50
//#define ERASE_SET_CMD       0x20
//#define ERASE_CNF_CMD       0xd0
//#define PRG_SETUP_CMD       0x40
//#define SECTOR_ERASE_SET_CMD        0x30
//#define WSM_BUSY                    0x80

//#define ALADDIN2MB_INSTALLER_LOGO_BANK ALADDIN2MB_BANK_PROMETHEOS2
//#define ALADDIN2MB_INSTALLER_LOGO_OFFSET (0x1f0000 - 0x1c0000)

//Bank_PrometheOS =  0x84  0x000000 - 0x07ffff  Sector Size 4096  Total  524288 (See below)
//Bank1_256k      =  0x86  0x080000 - 0x0bffff  Sector Size 4096  Total  262144
//Bank_Bootloader =  0x87  0x0C0000 - 0x0fffff  Sector Size 4096  Total  262144

//0x000000 - 0x07efff  Sector Size 4096  Total 520192 (PrometheOS)
//0x07f000 - 0x07ffff  Sector Size 4096  Total 4096   (Settings)

modchipAladdin2mb::modchipAladdin2mb()
{
	mBank = ALADDIN2MB_BANK_BOOTLOADER;
}

void modchipAladdin2mb::setLedColor(uint8_t ledColor)
{
    //outputByte(ALADDIN2MB_REGISTER_LED, ledColor);
}

uint32_t modchipAladdin2mb::getSlotCount()
{
	return 4;
}

uint32_t modchipAladdin2mb::getFlashSize(bool recovery)
{
	return recovery ? 0 : (2 * 1024 * 1024);
}

void modchipAladdin2mb::disableRecovery()
{
}

bool modchipAladdin2mb::isValidBankSize(uint32_t size)
{
	return size == (256 * 1024) || size == (512 * 1024) || size == (1024 * 1024);
}

bool modchipAladdin2mb::isValidFlashSize(bool recovery, uint32_t size)
{
	return size == getFlashSize(recovery);
}

uint32_t modchipAladdin2mb::getBankSize(uint8_t bank)
{
	if (bank == ALADDIN2MB_BANK_SLOT1_256K) 
	{
		return 256 * 1024;
	}
	if (bank == ALADDIN2MB_BANK_SLOT2_256K) 
	{
		return 256 * 1024;
	}
	if (bank == ALADDIN2MB_BANK_SLOT3_256K) 
	{
		return 256 * 1024;
	}
	if (bank == ALADDIN2MB_BANK_SLOT4_256K) 
	{
		return 256 * 1024;
	}
	if (bank == ALADDIN2MB_BANK_SLOT1_512K) 
	{
		return 512 * 1024;
	}
	if (bank == ALADDIN2MB_BANK_SLOT2_512K) 
	{
		return 512 * 1024;
	}
	if (bank == ALADDIN2MB_BANK_SLOT1_1024K) 
	{
		return 1024 * 1024;
	}
	if (bank == ALADDIN2MB_BANK_BOOTLOADER) 
	{
		return 256 * 1024;
	}
	if (bank == ALADDIN2MB_BANK_PROMETHEOS1) 
	{
		return 256 * 1024;
	}
	if (bank == ALADDIN2MB_BANK_PROMETHEOS2) 
	{
		return 256 * 1024;
	}
	if (bank == ALADDIN2MB_BANK_PROMETHEOS3) 
	{
		return 256 * 1024;
	}
	return 0;
}

uint32_t modchipAladdin2mb::getBankMemOffset(uint8_t bank)
{
	if (bank == ALADDIN2MB_BANK_SLOT1_256K)
	{
		return 0x000000;
	}
	if (bank == ALADDIN2MB_BANK_SLOT2_256K)
	{
		return 0x040000;
	}
	if (bank == ALADDIN2MB_BANK_SLOT3_256K)
	{
		return 0x080000;
	}
	if (bank == ALADDIN2MB_BANK_SLOT4_256K)
	{
		return 0x0c0000;
	}
	if (bank == ALADDIN2MB_BANK_SLOT1_512K)
	{
		return 0x000000;
	}
	if (bank == ALADDIN2MB_BANK_SLOT2_512K)
	{
		return 0x080000;
	}
	if (bank == ALADDIN2MB_BANK_SLOT1_1024K)
	{
		return 0x000000;
	}
	if (bank == ALADDIN2MB_BANK_BOOTLOADER)
	{
		return 0x100000;
	}
	if (bank == ALADDIN2MB_BANK_PROMETHEOS1)
	{
		return 0x140000;
	}
	if (bank == ALADDIN2MB_BANK_PROMETHEOS2)
	{
		return 0x180000;
	}
	if (bank == ALADDIN2MB_BANK_PROMETHEOS3)
	{
		return 0x1c0000;
	}
	return 0;
}

uint32_t modchipAladdin2mb::getBankStartOffset(uint8_t bank)
{
	return 0;
}

uint8_t modchipAladdin2mb::getBankFromIdAndSlots(uint8_t id, uint8_t slots)
{
	if (id == 0 && slots == 1)
	{
		return ALADDIN2MB_BANK_SLOT1_256K;
	}
	if (id == 1 && slots == 1)
	{
		return ALADDIN2MB_BANK_SLOT2_256K;
	}
	if (id == 2 && slots == 1)
	{
		return ALADDIN2MB_BANK_SLOT3_256K;
	}
	if (id == 3 && slots == 1)
	{
		return ALADDIN2MB_BANK_SLOT4_256K;
	}
	if (id == 0 && slots == 2)
	{
		return ALADDIN2MB_BANK_SLOT1_512K;
	}
	if (id == 2 && slots == 2)
	{
		return ALADDIN2MB_BANK_SLOT2_512K;
	}
	if (id == 0 && slots == 4)
	{
		return ALADDIN2MB_BANK_SLOT1_1024K;
	}
	return 0;
}

utils::dataContainer* modchipAladdin2mb::readBank(uint8_t bank)
{
	setBank(bank); 
	uint32_t bankSize = getBankSize(bank);
	utils::dataContainer* dataContainer = new utils::dataContainer(bankSize);
    volatile uint8_t* lpcMemMap = (volatile uint8_t *)(LPC_MEMORY_BASE + getBankStartOffset(bank));
    memcpy(dataContainer->data, (void*)&lpcMemMap[0], bankSize);
	setBank(ALADDIN2MB_BANK_BOOTLOADER);
	return dataContainer;
}

void modchipAladdin2mb::eraseBank(uint8_t bank)
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

	setBank(ALADDIN2MB_BANK_BOOTLOADER);

#ifndef TOOLS
	setLedColor(settingsManager::getLedColor());
#else
	setLedColor(LED_COLOR_GREEN);
#endif
}

void modchipAladdin2mb::writeBank(uint8_t bank, utils::dataContainer* dataContainer)
{
	setBank(bank);

	setLedColor(LED_COLOR_BLUE);

	volatile uint8_t* lpcMemMap = (volatile uint8_t *)(LPC_MEMORY_BASE + getBankStartOffset(bank));

    for (uint32_t i = 0; i < dataContainer->size; i++)
	{
		uint8_t value = (uint8_t)dataContainer->data[i];
		lpcMemMap[0] = 0x50;
		lpcMemMap[0] = 0x40;
		lpcMemMap[i] = value;
		lpcMemMap[0] = 0x70;
		uint8_t status = lpcMemMap[0];
		while((lpcMemMap[0] & 0x80) != 0x80);
    }
	lpcMemMap[0] = 0xff;

	setBank(ALADDIN2MB_BANK_BOOTLOADER);

#ifndef TOOLS
	setLedColor(settingsManager::getLedColor());
#else
	setLedColor(LED_COLOR_GREEN);
#endif
}

bool modchipAladdin2mb::verifyBank(uint8_t bank, utils::dataContainer* dataContainer)
{
	setBank(bank);

	utils::dataContainer* writtenData = readBank(bank);

	setLedColor(LED_COLOR_PURPLE);

	volatile uint8_t* lpcMemMap = (volatile uint8_t *)LPC_MEMORY_BASE;

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

	setBank(ALADDIN2MB_BANK_BOOTLOADER);

#ifndef TOOLS
	setLedColor(settingsManager::getLedColor());
#else
	setLedColor(LED_COLOR_GREEN);
#endif
	return ok;
}

uint8_t modchipAladdin2mb::getFlashBankCount(bool recovery)
{
	return 8;
}

uint8_t modchipAladdin2mb::getFlashBank(bool recovery, uint8_t bank)
{
	const uint8_t banks[] = { 
		ALADDIN2MB_BANK_SLOT1_256K,
		ALADDIN2MB_BANK_SLOT2_256K,
		ALADDIN2MB_BANK_SLOT3_256K,
		ALADDIN2MB_BANK_SLOT4_256K,
		ALADDIN2MB_BANK_BOOTLOADER,
		ALADDIN2MB_BANK_PROMETHEOS1,
		ALADDIN2MB_BANK_PROMETHEOS2,
		ALADDIN2MB_BANK_PROMETHEOS3,
	};
	return recovery ? 0 : banks[bank];
}

bankType modchipAladdin2mb::getFlashBankType(bool recovery, uint8_t bank)
{
	const bankType banks[] = { 
		bankTypeUser, 
		bankTypeUser, 
		bankTypeUser, 
		bankTypeUser, 
		bankTypeSystem,
		bankTypeSystem,
		bankTypeSystem,
		bankTypeSystem,
	};
	return recovery ? bankTypeUser : banks[bank];
}

utils::dataContainer* modchipAladdin2mb::readFlash(bool recovery)
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

void modchipAladdin2mb::launchBank(uint8_t bank, uint8_t ledColor)
{
	DWORD scratch;
	HalReadSMBusByte(SMBDEV_PIC16L, PIC16L_CMD_SCRATCH_REGISTER, &scratch);
	HalWriteSMBusByte(SMBDEV_PIC16L, PIC16L_CMD_SCRATCH_REGISTER, scratch & ~SCRATCH_REGISTER_BITVALUE_NO_ANIMATION);
	setBank(bank);
	setLedColor(ledColor);
	HalReturnToFirmware(RETURN_FIRMWARE_REBOOT);
}

void modchipAladdin2mb::launchTsop()
{
	DWORD scratch;
	HalReadSMBusByte(SMBDEV_PIC16L, PIC16L_CMD_SCRATCH_REGISTER, &scratch);
	HalWriteSMBusByte(SMBDEV_PIC16L, PIC16L_CMD_SCRATCH_REGISTER, scratch & ~SCRATCH_REGISTER_BITVALUE_NO_ANIMATION);
	setBank(ALADDIN2MB_BANK_TSOP);
	setLedColor(LED_COLOR_OFF);
	HalReturnToFirmware(RETURN_FIRMWARE_REBOOT);
}

void modchipAladdin2mb::launchRecovery()
{
}

#ifndef TOOLS

void modchipAladdin2mb::loadSettings(settingsState& settings)
{
	setBank(ALADDIN2MB_SETTINGS_BANK); 

	setLedColor(LED_COLOR_WHITE);

    volatile uint8_t* lpcMemMap = (volatile uint8_t *)LPC_MEMORY_BASE;

    memcpy(&settings, (void*)&lpcMemMap[ALADDIN2MB_SETTINGS_OFFSET], sizeof(settings));
	uint32_t checksum = crc32::calculate(((uint8_t*)&settings) + sizeof(uint32_t), sizeof(settings) - sizeof(uint32_t));

	setBank(ALADDIN2MB_BANK_BOOTLOADER);

	if (checksum != settings.checksum || versioning::compareVersion(settings.version, settingsManager::getVersion()) != 0)
	{
		settingsManager::initSettings();
		settingsManager::saveSettings();
	}

	setLedColor(settingsManager::getLedColor());
}

void modchipAladdin2mb::saveSettings(settingsState& settings) 
{
	setBank(ALADDIN2MB_SETTINGS_BANK); 

	settings.checksum = crc32::calculate(((uint8_t*)&settings) + sizeof(uint32_t), sizeof(settings) - sizeof(uint32_t));
	utils::dataContainer* settingsData = new utils::dataContainer((char*)&settings, sizeof(settings), sizeof(settings));

	volatile uint8_t* lpcMemMap = (volatile uint8_t *)LPC_MEMORY_BASE;

	setLedColor(LED_COLOR_AMBER);
	sectorErase(ALADDIN2MB_SETTINGS_OFFSET);

	setLedColor(LED_COLOR_BLUE);
    for (uint32_t i = 0; i < settingsData->size; i++)
	{
		uint8_t value = (uint8_t)settingsData->data[i];
		lpcMemMap[0] = 0x50;
		lpcMemMap[0] = 0x40;
		lpcMemMap[ALADDIN2MB_SETTINGS_OFFSET + i] = value;
		lpcMemMap[0] = 0x70;
		uint8_t status = lpcMemMap[0];
		while((lpcMemMap[0] & 0x80) != 0x80);
    }
	lpcMemMap[0] = 0xff;
	
	setBank(ALADDIN2MB_BANK_BOOTLOADER);

	setLedColor(settingsManager::getLedColor());
}

#endif

utils::dataContainer* modchipAladdin2mb::getInstallerLogo()
{
	setBank(ALADDIN2MB_INSTALLER_LOGO_BANK); 

	setLedColor(LED_COLOR_WHITE);

	utils::dataContainer* installerLogo = new utils::dataContainer(32768);

	volatile uint8_t* lpcMemMap = (volatile uint8_t *)LPC_MEMORY_BASE;
    memcpy(installerLogo->data, (void*)&lpcMemMap[ALADDIN2MB_INSTALLER_LOGO_OFFSET], installerLogo->size);

	setBank(ALADDIN2MB_BANK_BOOTLOADER);

#ifndef TOOLS
	setLedColor(settingsManager::getLedColor());
#else
	setLedColor(LED_COLOR_GREEN);
#endif
	return installerLogo;
}

displayDriver* modchipAladdin2mb::getDisplayDriver(bool current)
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

supportInfo modchipAladdin2mb::getSupportInfo(bool current)
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

uint8_t modchipAladdin2mb::getLcdModeCount()
{
	return 3;
}

char* modchipAladdin2mb::getLcdModeString(uint8_t lcdMode)
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

uint8_t modchipAladdin2mb::getLcdModelCount(bool current)
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

char* modchipAladdin2mb::getLcdModelString(bool current, uint8_t lcdModel)
{
	displayDriver* driver = getDisplayDriver(current);
	if (driver == NULL)
	{
		return strdup("");
	}
	return driver->getModel();
}

uint8_t modchipAladdin2mb::getLcdAddressCount(bool current)
{	
	displayDriver* driver = getDisplayDriver(current);
	if (driver == NULL)
	{
		return 0;
	}
	return driver->getI2cAddressCount();
}

char* modchipAladdin2mb::getLcdAddressString(bool current, uint8_t lcdAddress)
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

void modchipAladdin2mb::lcdSetCursorPosition(uint8_t row, uint8_t col)
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

void modchipAladdin2mb::lcdInit()
{
	displayDriver* driver = getDisplayDriver(true);
	if (driver == NULL)
	{
		return;
	}
	driver->setAddress(settingsManager::getLcdAddress(true));
	driver->init();
}

void modchipAladdin2mb::lcdPrintMessage(const char* message)
{
	displayDriver* driver = getDisplayDriver(true);
	if (driver == NULL)
	{
		return;
	}
	driver->setAddress(settingsManager::getLcdAddress(true));
	driver->printMessage(message);
}

void modchipAladdin2mb::lcdSetBacklight(uint8_t value)
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

void modchipAladdin2mb::lcdSetContrast(uint8_t value)
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

void modchipAladdin2mb::setBank(uint8_t bank)
{
	mBank = bank;
    outputByte(ALADDIN_REGISTER_BANKING, mBank);
}

uint8_t modchipAladdin2mb::getBank()
{
	return mBank;
}

bool modchipAladdin2mb::isEraseMemOffset(uint32_t memOffset)
{
	return (memOffset % 4096) == 0;
}

bool modchipAladdin2mb::isProtectedMemOffset(uint32_t memOffset)
{
	if (isProtectedMemOffset(memOffset))
	{
		return true;
	}
	return false;
}

void modchipAladdin2mb::sectorErase(uint32_t offset)
{
	volatile uint8_t* lpcMemMap = (volatile uint8_t *)LPC_MEMORY_BASE;

	// Unprotect mem if needed
	uint8_t originalBank = getBank();
	setBank(ALADDIN2MB_BANK_MEM_REGISTER);
	bool isProtected = lpcMemMap[0x000002] != 0;
	if (isProtected == true)
	{
		for(int i = 0x00; i < 0x23; i++) 
		{
			if (i == 0x22) 
			{
				lpcMemMap[0x1FC002] = 0x00;
			} 
			else if (i == 0x21) 
			{
				lpcMemMap[0x1FA002] = 0x00;
			}
			else if (i == 0x20) 
			{
				lpcMemMap[0x1F8002] = 0x00;
			} 
			else 
			{
				lpcMemMap[(i << 16) | 0x000002] = 0x00;
			}
		}
	}
	lpcMemMap[0x400000] = 0xff;
	setBank(originalBank);

	// Erase sector
	lpcMemMap[0] = 0x50;
	lpcMemMap[0] = 0x30;
	lpcMemMap[offset] = 0xD0;
	lpcMemMap[0] = 0x70;
	while((lpcMemMap[0] & 0x80) != 0x80);
	lpcMemMap[0] = 0xff;
}
