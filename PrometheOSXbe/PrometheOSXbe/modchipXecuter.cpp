#include "modchipXecuter.h"
#include "crc32.h"
#include "settingsManager.h"
#include "globalDefines.h"
#include "stringUtility.h"

//#define DISPLAY_E         0x04        //Mapped for LPCMod output format.
//#define DISPLAY_RS        0x02        //Mapped for LPCMod output format.
//#define DISP_CLEAR          0x01 // cmd: clear display command
//#define DISP_HOME           0x02 // cmd: return cursor to home
//#define DISP_ENTRY_MODE_SET 0x04 // cmd: enable cursor moving direction and enable the shift of entire display
//#define DISP_S_FLAG         0x01
//#define DISP_ID_FLAG        0x02
//#define DISP_CONTROL        0x08  //cmd: display ON/OFF
//#define DISP_D_FLAG         0x04  // display on
//#define DISP_C_FLAG         0x02  // cursor on
//#define DISP_B_FLAG         0x01  // blinking on
//#define DISP_EXT_CONTROL    0x08  
//
//#define DISP_FW_FLAG        0x04  
//#define DISP_BW_FLAG        0x02 
//#define DISP_NW_FLAG        0x01 
//
//#define DISP_SHIFT          0x10 
//#define DISP_SC_FLAG        0x08
//#define DISP_RL_FLAG        0x04
//#define DISP_FUNCTION_SET   0x20  
//#define DISP_DL_FLAG        0x10 
//#define DISP_N_FLAG         0x08 
//#define DISP_F_FLAG         0x04 
//#define DISP_RE_FLAG        0x04
//#define DISP_CGRAM_SET      0x40  
//#define DISP_SEGRAM_SET     0x40  
#define DISP_DDRAM_SET      0x80  


//Bank_Bootloader  =  0  0x000000 - 0x03ffff  Sector Size 65536  Total  262144
//Bank_PrometheOS1 =  1  0x040000 - 0x07ffff  Sector Size 65536  Total  262144
//Bank_PrometheOS2 =  2  0x080000 - 0x0bffff  Sector Size 65536  Total  262144
//Bank_PrometheOS3 =  3  0x0c0000 - 0x0fffff  Sector Size 65536  Total  262144
//Bank1_256k       =  4  0x100000 - 0x13ffff  Sector Size 65536  Total  262144
//Bank2_256k       =  5  0x140000 - 0x17ffff  Sector Size 65536  Total  262144
//Bank3_256k       =  6  0x180000 - 0x1bffff  Sector Size 65536  Total  262144
//Bank4_256k       =  7  0x1c0000 - 0x1fffff  Sector Size 65536  Total  262144
//Bank1_512k       =  7  0x100000 - 0x17ffff  Sector Size 65536  Total  524288
//Bank2_512k       =  8  0x180000 - 0x1fffff  Sector Size 65536  Total  524288
//Bank1_1024k      =  9  0x100000 - 0x1fffff  Sector Size 65536  Total 1048576

//0x0c0000 - 0x0dffff  Total 131072 PrometheOS Continued
//0x0e0000 - 0x0effff  Total 65536  Installer Logo
//0x0f0000 - 0x0fffff  Total 65536  Settings

modchipXecuter::modchipXecuter()
{
	mBank = XECUTER_BANK_BOOTLOADER;
}

void modchipXecuter::setLedColor(uint8_t ledColor)
{
}

uint32_t modchipXecuter::getSlotCount()
{
	return 4;
}

uint32_t modchipXecuter::getFlashSize(bool recovery)
{
	return recovery ? (256 * 1024) : (2 * 1024 * 1024);
}

void modchipXecuter::disableRecovery()
{
	outputByte(XECUTER_REGISTER_HARDWARE_BANKING, XECUTER_DISABLE_BACKUP);
}

bool modchipXecuter::isValidBankSize(uint32_t size)
{
	return size == (256 * 1024) || size == (512 * 1024) || size == (1024 * 1024);
}

bool modchipXecuter::isValidFlashSize(bool recovery, uint32_t size)
{
	return size == getFlashSize(recovery);
}

uint32_t modchipXecuter::getBankSize(uint8_t bank)
{
	if (bank == XECUTER_BANK_BOOTLOADER) 
	{
		return 256 * 1024;
	}
	if (bank == XECUTER_BANK_PROMETHEOS1) 
	{
		return 256 * 1024;
	}
	if (bank == XECUTER_BANK_PROMETHEOS2) 
	{
		return 256 * 1024;
	}
	if (bank == XECUTER_BANK_PROMETHEOS3) 
	{
		return 256 * 1024;
	}
	if (bank == XECUTER_BANK_SLOT1_256K) 
	{
		return 256 * 1024;
	}
	if (bank == XECUTER_BANK_SLOT2_256K) 
	{
		return 256 * 1024;
	}
	if (bank == XECUTER_BANK_SLOT3_256K) 
	{
		return 256 * 1024;
	}
	if (bank == XECUTER_BANK_SLOT4_256K) 
	{
		return 256 * 1024;
	}
	if (bank == XECUTER_BANK_SLOT1_512K) 
	{
		return 512 * 1024;
	}
	if (bank == XECUTER_BANK_SLOT2_512K) 
	{
		return 512 * 1024;
	}
	if (bank == XECUTER_BANK_SLOT1_1024K) 
	{
		return 1024 * 1024;
	}
	if (bank == XECUTER_BANK_BACKUP_256K) 
	{
		return 256 * 1024;
	}
	return 0;
}

uint32_t modchipXecuter::getBankMemOffset(uint8_t bank)
{
	if (bank == XECUTER_BANK_BOOTLOADER)
	{
		return 0x000000;
	}
	if (bank == XECUTER_BANK_PROMETHEOS1)
	{
		return 0x040000;
	}
	if (bank == XECUTER_BANK_PROMETHEOS2)
	{
		return 0x080000;
	}
	if (bank == XECUTER_BANK_PROMETHEOS3)
	{
		return 0x0c0000;
	}
	if (bank == XECUTER_BANK_SLOT1_256K)
	{
		return 0x100000;
	}
	if (bank == XECUTER_BANK_SLOT2_256K)
	{
		return 0x140000;
	}
	if (bank == XECUTER_BANK_SLOT3_256K)
	{
		return 0x180000;
	}
	if (bank == XECUTER_BANK_SLOT4_256K)
	{
		return 0x1c0000;
	}
	if (bank == XECUTER_BANK_SLOT1_512K)
	{
		return 0x100000;
	}
	if (bank == XECUTER_BANK_SLOT2_512K)
	{
		return 0x180000;
	}
	if (bank == XECUTER_BANK_SLOT1_1024K)
	{
		return 0x100000;
	}
	if (bank == XECUTER_BANK_BACKUP_256K) 
	{
		return 0x000000;
	}
	return 0;
}

uint32_t modchipXecuter::getBankStartOffset(uint8_t bank)
{
	return 0;
}

uint8_t modchipXecuter::getBankFromIdAndSlots(uint8_t id, uint8_t slots)
{
	if (id == 0 && slots == 1)
	{
		return XECUTER_BANK_SLOT1_256K;
	}
	if (id == 1 && slots == 1)
	{
		return XECUTER_BANK_SLOT2_256K;
	}
	if (id == 2 && slots == 1)
	{
		return XECUTER_BANK_SLOT3_256K;
	}
	if (id == 3 && slots == 1)
	{
		return XECUTER_BANK_SLOT4_256K;
	}
	if (id == 0 && slots == 2)
	{
		return XECUTER_BANK_SLOT1_512K;
	}
	if (id == 2 && slots == 2)
	{
		return XECUTER_BANK_SLOT2_512K;
	}
	if (id == 0 && slots == 4)
	{
		return XECUTER_BANK_SLOT1_1024K;
	}
	return 0;
}

utils::dataContainer* modchipXecuter::readBank(uint8_t bank)
{
	setBank(bank); 
	uint32_t bankSize = getBankSize(bank);
	utils::dataContainer* dataContainer = new utils::dataContainer(bankSize);
    volatile uint8_t* lpcMemMap = (volatile uint8_t *)(LPC_MEMORY_BASE + getBankStartOffset(bank));
    memcpy(dataContainer->data, (void*)&lpcMemMap[0], bankSize);
	setBank(XECUTER_BANK_BOOTLOADER);
	return dataContainer;
}

void modchipXecuter::eraseBank(uint8_t bank)
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

	setBank(XECUTER_BANK_BOOTLOADER);

#ifndef TOOLS
	setLedColor(settingsManager::getLedColor());
#else
	setLedColor(LED_COLOR_GREEN);
#endif
}

void modchipXecuter::writeBank(uint8_t bank, utils::dataContainer* dataContainer)
{
	setBank(bank);

	setLedColor(LED_COLOR_BLUE);

	if (checkWriteProtect() == false)
	{
		volatile uint8_t* lpcMemMap = (volatile uint8_t *)(LPC_MEMORY_BASE + getBankStartOffset(bank));

		uint32_t memOffset = getBankMemOffset(bank);

		for (uint32_t i = 0; i < dataContainer->size; i++)
		{
			if (isProtectedMemOffset(memOffset + i) == true)
			{
				continue;
			}
			uint8_t value = (uint8_t)dataContainer->data[i];
			lpcMemMap[0x5555] = 0xAA;
			lpcMemMap[0x2AAA] = 0x55;
			lpcMemMap[0x5555] = 0xA0;
			lpcMemMap[i] = value;
			while (lpcMemMap[i] != value);
		}
		lpcMemMap[0] = 0xf0;
	}

	setBank(XECUTER_BANK_BOOTLOADER);

#ifndef TOOLS
	setLedColor(settingsManager::getLedColor());
#else
	setLedColor(LED_COLOR_GREEN);
#endif
}

bool modchipXecuter::verifyBank(uint8_t bank, utils::dataContainer* dataContainer)
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

	setBank(XECUTER_BANK_BOOTLOADER);

#ifndef TOOLS
	setLedColor(settingsManager::getLedColor());
#else
	setLedColor(LED_COLOR_GREEN);
#endif
	return ok;
}

uint8_t modchipXecuter::getFlashBankCount(bool recovery)
{
	return recovery ? 1 : 8;
}

uint8_t modchipXecuter::getFlashBank(bool recovery, uint8_t bank)
{
	const uint8_t banks[] = { 
		XECUTER_BANK_SLOT1_256K, 
		XECUTER_BANK_SLOT2_256K, 
		XECUTER_BANK_SLOT3_256K, 
		XECUTER_BANK_SLOT4_256K,
		XECUTER_BANK_BOOTLOADER, 
		XECUTER_BANK_PROMETHEOS1, 
		XECUTER_BANK_PROMETHEOS2, 
		XECUTER_BANK_PROMETHEOS3
	};
	return recovery ? XECUTER_BANK_BACKUP_256K : banks[bank];
}

bankType modchipXecuter::getFlashBankType(bool recovery, uint8_t bank)
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

utils::dataContainer* modchipXecuter::readFlash(bool recovery)
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

void modchipXecuter::launchBank(uint8_t bank, uint8_t ledColor)
{
	DWORD scratch;
	HalReadSMBusByte(SMBDEV_PIC16L, PIC16L_CMD_SCRATCH_REGISTER, &scratch);
	HalWriteSMBusByte(SMBDEV_PIC16L, PIC16L_CMD_SCRATCH_REGISTER, scratch & ~SCRATCH_REGISTER_BITVALUE_NO_ANIMATION);
	setBank(bank);
	setLedColor(ledColor);
	HalReturnToFirmware(RETURN_FIRMWARE_REBOOT);
}

void modchipXecuter::launchTsop()
{
	DWORD scratch;
	HalReadSMBusByte(SMBDEV_PIC16L, PIC16L_CMD_SCRATCH_REGISTER, &scratch);
	HalWriteSMBusByte(SMBDEV_PIC16L, PIC16L_CMD_SCRATCH_REGISTER, scratch & ~SCRATCH_REGISTER_BITVALUE_NO_ANIMATION);
	setBank(XECUTER_BANK_TSOP);
	setLedColor(LED_COLOR_OFF);
	HalReturnToFirmware(RETURN_FIRMWARE_REBOOT);
}

void modchipXecuter::launchRecovery()
{
	DWORD scratch;
	HalReadSMBusByte(SMBDEV_PIC16L, PIC16L_CMD_SCRATCH_REGISTER, &scratch);
	HalWriteSMBusByte(SMBDEV_PIC16L, PIC16L_CMD_SCRATCH_REGISTER, scratch & ~SCRATCH_REGISTER_BITVALUE_NO_ANIMATION);
	setBank(XECUTER_BANK_BACKUP_256K);
	setLedColor(LED_COLOR_OFF);
	HalReturnToFirmware(RETURN_FIRMWARE_REBOOT);
}

#ifndef TOOLS

void modchipXecuter::loadSettings(settingsState& settings)
{
	setBank(XECUTER_SETTINGS_BANK); 

	setLedColor(LED_COLOR_WHITE);

    volatile uint8_t* lpcMemMap = (volatile uint8_t *)LPC_MEMORY_BASE;

    memcpy(&settings, (void*)&lpcMemMap[XECUTER_SETTINGS_OFFSET], sizeof(settings));
	uint32_t checksum = crc32::calculate(((uint8_t*)&settings) + sizeof(uint32_t), sizeof(settings) - sizeof(uint32_t));

	setBank(XECUTER_BANK_BOOTLOADER);

	if (checksum != settings.checksum || versioning::compareVersion(settings.version, settingsManager::getVersion()) != 0)
	{
		settingsManager::initSettings();
		settingsManager::saveSettings();
	}

	setLedColor(settingsManager::getLedColor());
}

void modchipXecuter::saveSettings(settingsState& settings) 
{
	if (checkWriteProtect() == true)
	{
		return;
	}

	setBank(XECUTER_SETTINGS_BANK); 

	settings.checksum = crc32::calculate(((uint8_t*)&settings) + sizeof(uint32_t), sizeof(settings) - sizeof(uint32_t));
	utils::dataContainer* settingsData = new utils::dataContainer((char*)&settings, sizeof(settings), sizeof(settings));

	volatile uint8_t* lpcMemMap = (volatile uint8_t *)LPC_MEMORY_BASE;

	setLedColor(LED_COLOR_AMBER);
	sectorErase(XECUTER_SETTINGS_OFFSET);

	setLedColor(LED_COLOR_BLUE);
    for (uint32_t i = 0; i < settingsData->size; i++)
	{
		uint8_t value = (uint8_t)settingsData->data[i];
		lpcMemMap[0x5555] = 0xAA;
		lpcMemMap[0x2AAA] = 0x55;
		lpcMemMap[0x5555] = 0xA0;
		lpcMemMap[XECUTER_SETTINGS_OFFSET + i] = value;
		while (lpcMemMap[XECUTER_SETTINGS_OFFSET + i] != value);
    }
	lpcMemMap[0] = 0xF0;

	setBank(XECUTER_BANK_BOOTLOADER);

	setLedColor(settingsManager::getLedColor());
}

#endif

utils::dataContainer* modchipXecuter::getInstallerLogo()
{
	setBank(XECUTER_INSTALLER_LOGO_BANK); 

	setLedColor(LED_COLOR_WHITE);

	utils::dataContainer* installerLogo = new utils::dataContainer(32768);

	volatile uint8_t* lpcMemMap = (volatile uint8_t *)LPC_MEMORY_BASE;
    memcpy(installerLogo->data, (void*)&lpcMemMap[XECUTER_INSTALLER_LOGO_OFFSET], installerLogo->size);

	setBank(XECUTER_BANK_BOOTLOADER); 

#ifndef TOOLS
	setLedColor(settingsManager::getLedColor());
#else
	setLedColor(LED_COLOR_GREEN);
#endif
	return installerLogo;
}

displayDriver* modchipXecuter::getDisplayDriver(bool current)
{
	uint8_t lcdMode = settingsManager::getLcdMode(current);
	uint8_t lcdModel = settingsManager::getLcdModel(current);
	if (lcdMode == 1)
	{
		if (lcdModel == 0)
		{
			return displayFactory::getDisplay(displayVariantHD44780Xecuter);
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

supportInfo modchipXecuter::getSupportInfo(bool current)
{
	uint8_t lcdMode = settingsManager::getLcdMode(current);
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

	info.supportsRecovery = true;
	return info;
}

uint8_t modchipXecuter::getLcdModeCount()
{
	return 3;
}

char* modchipXecuter::getLcdModeString(uint8_t lcdMode)
{
	if (lcdMode == 1)
	{
		return strdup("Xecuter");
	}
	if (lcdMode == 2)
	{
		return strdup("SMBUS");
	}
	return strdup("Disabled");
}

uint8_t modchipXecuter::getLcdModelCount(bool current)
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

char* modchipXecuter::getLcdModelString(bool current, uint8_t lcdModel)
{
	displayDriver* driver = getDisplayDriver(current);
	if (driver == NULL)
	{
		return strdup("");
	}
	return driver->getModel();
}

uint8_t modchipXecuter::getLcdAddressCount(bool current)
{
	displayDriver* driver = getDisplayDriver(current);
	if (driver == NULL)
	{
		return 0;
	}
	return driver->getI2cAddressCount();
}

char* modchipXecuter::getLcdAddressString(bool current, uint8_t lcdAddress)
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

void modchipXecuter::lcdSetCursorPosition(uint8_t row, uint8_t col)
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

void modchipXecuter::lcdInit()
{
	displayDriver* driver = getDisplayDriver(true);
	if (driver == NULL)
	{
		return;
	}
	driver->setAddress(settingsManager::getLcdAddress(true));
	driver->init();
}

void modchipXecuter::lcdPrintMessage(const char* message)
{
	displayDriver* driver = getDisplayDriver(true);
	if (driver == NULL)
	{
		return;
	}
	driver->setAddress(settingsManager::getLcdAddress(true));
	driver->printMessage(message);
}

void modchipXecuter::lcdSetBacklight(uint8_t value)
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

void modchipXecuter::lcdSetContrast(uint8_t value)
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

void modchipXecuter::setBank(uint8_t bank)
{
	mBank = bank;

	if (mBank > 0xf)
	{
		outputByte(XECUTER_REGISTER_HARDWARE_BANKING, mBank);
	}
	else
	{
		outputByte(XECUTER_REGISTER_SOFTWARE_BANKING, mBank | 0x80);
	}
}

uint8_t modchipXecuter::getBank()
{
	return mBank;
}

bool modchipXecuter::isEraseMemOffset(uint32_t memOffset)
{
	if (mBank == XECUTER_BANK_BACKUP_256K)
	{
		if (memOffset >= 0x000000 && memOffset <= 0x02ffff)
		{
			return (memOffset % 65536) == 0;
		}
		if (memOffset >= 0x030000 && memOffset <= 0x037fff)
		{
			return (memOffset % 32768) == 0;
		}
		if (memOffset >= 0x038000 && memOffset <= 0x03bfff)
		{
			return (memOffset % 8192) == 0;
		}
		if (memOffset >= 0x03c000 && memOffset <= 0x03ffff)
		{
			return (memOffset % 16384) == 0;
		}
	}
	return (memOffset % 65536) == 0;
}

bool modchipXecuter::isProtectedMemOffset(uint32_t memOffset)
{
	return false;
}

void modchipXecuter::sectorErase(uint32_t offset)
{
	if (mBank != XECUTER_BANK_BACKUP_256K && (checkWriteProtect() || isProtectedMemOffset(offset)))
	{
		return;
	}
	volatile uint8_t* lpcMemMap = (volatile uint8_t *)LPC_MEMORY_BASE;
	lpcMemMap[0x5555] = 0xAA;
	lpcMemMap[0x2AAA] = 0x55;
	lpcMemMap[0x5555] = 0x80;
	lpcMemMap[0x5555] = 0xAA;
	lpcMemMap[0x2AAA] = 0x55;
	lpcMemMap[offset] = 0x30;
	while(lpcMemMap[0] != lpcMemMap[0]);
	lpcMemMap[0] = 0xF0;
}

bool modchipXecuter::checkWriteProtect()
{
	if (mBank == XECUTER_BANK_BACKUP_256K)
	{
		return false;
	}

	volatile uint8_t* lpcMemMap = (volatile uint8_t *)LPC_MEMORY_BASE;

    lpcMemMap[0x5555] = 0xAA;
    lpcMemMap[0x2AAA] = 0x55;
    lpcMemMap[0x5555] = 0x90;
    uint8_t manuf = lpcMemMap[0];
    uint8_t devid = lpcMemMap[1];
    lpcMemMap[0] = 0xF0;

	bool isProtectOn = (manuf != 0x01 || devid != 0xad);
    return isProtectOn;
}