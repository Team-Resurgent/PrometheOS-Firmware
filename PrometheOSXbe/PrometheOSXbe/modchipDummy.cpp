#include "modchipDummy.h"
#include "crc32.h"
#include "settingsManager.h"
#include "stringUtility.h"

#define DUMMY_BANK_TSOP 0
#define DUMMY_BANK_BOOTLOADER 1
#define DUMMY_BANK_PROMETHEOS1 2
#define DUMMY_BANK_PROMETHEOS2 10
#define DUMMY_BANK_SLOT1_256K 3
#define DUMMY_BANK_SLOT2_256K 4
#define DUMMY_BANK_SLOT3_256K 5
#define DUMMY_BANK_SLOT4_256K 6
#define DUMMY_BANK_SLOT1_512K 7
#define DUMMY_BANK_SLOT2_512K 8
#define DUMMY_BANK_SLOT1_1024K 9

#define DUMMY_SETTINGS_BANK DUMMY_BANK_PROMETHEOS2
#define DUMMY_SETTINGS_OFFSET (0x1f8000 - 0x1c0000)
#define DUMMY_INSTALLER_LOGO_BANK DUMMY_BANK_PROMETHEOS2
#define DUMMY_INSTALLER_LOGO_OFFSET (0x1f0000 - 0x1c0000)

#define DUMMY_LCD_DELAY 2
#define DUMMY_LCD_COMMAND_MODE 0x80
#define DUMMY_LCD_DATA_MODE 0x40

#define DUMMY_US2066_DISPLAY_MODE 0x08
#define DUMMY_US2066_DISPLAY_MODE_OFF_FLAG 0x00
#define DUMMY_US2066_DISPLAY_MODE_ON_FLAG 0x04
#define DUMMY_US2066_DISPLAY_MODE_CURSOR_OFF_FLAG 0x00
#define DUMMY_US2066_DISPLAY_MODE_CURSOR_ON_FLAG 0x02
#define DUMMY_US2066_DISPLAY_MODE_BLINK_OFF_FLAG 0x00
#define DUMMY_US2066_DISPLAY_MODE_BLINK_ON_FLAG 0x01

#define DUMMY_US2066_EXTENDED_MODE 0x08
#define DUMMY_US2066_EXTENDED_MODE_5DOT_FLAG 0x00
#define DUMMY_US2066_EXTENDED_MODE_6DOT_FLAG 0x04
#define DUMMY_US2066_EXTENDED_MODE_INVERT_CURSOR_OFF_FLAG 0x00
#define DUMMY_US2066_EXTENDED_MODE_INVERT_CURSOR_FLAG 0x02
#define DUMMY_US2066_EXTENDED_MODE_1TO2ROWS_FLAG 0x00
#define DUMMY_US2066_EXTENDED_MODE_3TO4ROWS_FLAG 0x01


// commands
#define DUMMY_LCD_CLEARDISPLAY 0x01
#define DUMMY_LCD_RETURNHOME 0x02
#define DUMMY_LCD_ENTRYMODESET 0x04
#define DUMMY_LCD_DISPLAYCONTROL 0x08
#define DUMMY_LCD_CURSORSHIFT 0x10
#define DUMMY_LCD_FUNCTIONSET 0x20
#define DUMMY_LCD_SETCGRAMADDR 0x40
#define DUMMY_LCD_SETDDRAMADDR 0x80

// flags for display entry mode
#define DUMMY_LCD_ENTRYRIGHT 0x00
#define DUMMY_LCD_ENTRYLEFT 0x02
#define DUMMY_LCD_ENTRYSHIFTINCREMENT 0x01
#define DUMMY_LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define DUMMY_LCD_DISPLAYON 0x04
#define DUMMY_LCD_DISPLAYOFF 0x00
#define DUMMY_LCD_CURSORON 0x02
#define DUMMY_LCD_CURSOROFF 0x00
#define DUMMY_LCD_BLINKON 0x01
#define DUMMY_LCD_BLINKOFF 0x00

// flags for display/cursor shift
#define DUMMY_LCD_DISPLAYMOVE 0x08
#define DUMMY_LCD_CURSORMOVE 0x00
#define DUMMY_LCD_MOVERIGHT 0x04
#define DUMMY_LCD_MOVELEFT 0x00

// flags for function set
#define DUMMY_LCD_8BITMODE 0x10
#define DUMMY_LCD_4BITMODE 0x00
#define DUMMY_LCD_2LINE 0x08
#define DUMMY_LCD_1LINE 0x00
#define DUMMY_LCD_5x10DOTS 0x04
#define DUMMY_LCD_5x8DOTS 0x00

// flags for backlight control
#define DUMMY_LCD_BACKLIGHT   B00001000
#define DUMMY_LCD_NOBACKLIGHT B00000000

// values for graphtype in calls to init_bargraph and character geometry
#define DUMMY_LCDI2C_VERTICAL_BAR_GRAPH    1
#define DUMMY_LCDI2C_HORIZONTAL_BAR_GRAPH  2
#define DUMMY_LCDI2C_HORIZONTAL_LINE_GRAPH 3
#define DUMMY_LCD_CHARACTER_HORIZONTAL_DOTS 5
#define DUMMY_LCD_CHARACTER_VERTICAL_DOTS   8

#define DUMMY_LCD_EN B00000100  // Enable bit
#define DUMMY_LCD_RW B00000010  // Read/Write bit
#define DUMMY_LCD_RS B00000001  // Register select bit

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

//https://github.com/mrkaleArduinoLib/LiquidCrystal_I2C/blob/master/src/LiquidCrystal_I2C.cpp

modchipDummy::modchipDummy()
{
	mFlashData = (uint8_t*)malloc(getFlashSize(false));
}

modchipDummy::~modchipDummy()
{
	free(mFlashData);
}

void modchipDummy::setLedColor(uint8_t ledColor)
{
}

uint32_t modchipDummy::getSlotCount()
{
	return 4;
}

uint32_t modchipDummy::getFlashSize(bool recovery)
{
	return recovery ? 0 : (2 * 1024 * 1024);
}

void modchipDummy::disableRecovery()
{
}

bool modchipDummy::isValidBankSize(uint32_t size)
{
	return size == (256 * 1024) || size == (512 * 1024) || size == (1024 * 1024);
}

bool modchipDummy::isValidFlashSize(bool recovery, uint32_t size)
{
	return size == getFlashSize(recovery);
}

uint32_t modchipDummy::getBankSize(uint8_t bank)
{
	if (bank == DUMMY_BANK_BOOTLOADER) 
	{
		return 256 * 1024;
	}
	if (bank == DUMMY_BANK_PROMETHEOS1) 
	{
		return 512 * 1024;
	}
	if (bank == DUMMY_BANK_PROMETHEOS2) 
	{
		return 256 * 1024;
	}
	if (bank == DUMMY_BANK_SLOT1_256K) 
	{
		return 256 * 1024;
	}
	if (bank == DUMMY_BANK_SLOT2_256K) 
	{
		return 256 * 1024;
	}
	if (bank == DUMMY_BANK_SLOT3_256K) 
	{
		return 256 * 1024;
	}
	if (bank == DUMMY_BANK_SLOT4_256K) 
	{
		return 256 * 1024;
	}
	if (bank == DUMMY_BANK_SLOT1_512K) 
	{
		return 512 * 1024;
	}
	if (bank == DUMMY_BANK_SLOT2_512K) 
	{
		return 512 * 1024;
	}
	if (bank == DUMMY_BANK_SLOT1_1024K) 
	{
		return 1024 * 1024;
	}
	return 0;
}

uint32_t modchipDummy::getBankMemOffset(uint8_t bank)
{
	if (bank == DUMMY_BANK_SLOT1_256K)
	{
		return 0x000000;
	}
	if (bank == DUMMY_BANK_SLOT2_256K)
	{
		return 0x040000;
	}
	if (bank == DUMMY_BANK_SLOT2_256K)
	{
		return 0x080000;
	}
	if (bank == DUMMY_BANK_SLOT4_256K)
	{
		return 0x0c0000;
	}
	if (bank == DUMMY_BANK_SLOT1_512K)
	{
		return 0x000000;
	}
	if (bank == DUMMY_BANK_SLOT2_512K)
	{
		return 0x080000;
	}
	if (bank == DUMMY_BANK_SLOT1_1024K)
	{
		return 0x000000;
	}
	if (bank == DUMMY_BANK_BOOTLOADER)
	{
		return 0x180000;
	}
	if (bank == DUMMY_BANK_PROMETHEOS1)
	{
		return 0x100000;
	}	
	if (bank == DUMMY_BANK_PROMETHEOS2)
	{
		return 0x1c0000;
	}
	return 0;
}

uint32_t modchipDummy::getBankStartOffset(uint8_t bank)
{
	return 0;
}

uint8_t modchipDummy::getBankFromIdAndSlots(uint8_t id, uint8_t slots)
{
	if (id == 0 && slots == 1)
	{
		return DUMMY_BANK_SLOT1_256K;
	}
	if (id == 1 && slots == 1)
	{
		return DUMMY_BANK_SLOT2_256K;
	}
	if (id == 2 && slots == 1)
	{
		return DUMMY_BANK_SLOT3_256K;
	}
	if (id == 3 && slots == 1)
	{
		return DUMMY_BANK_SLOT4_256K;
	}
	if (id == 0 && slots == 2)
	{
		return DUMMY_BANK_SLOT1_512K;
	}
	if (id == 2 && slots == 2)
	{
		return DUMMY_BANK_SLOT2_512K;
	}
	if (id == 0 && slots == 4)
	{
		return DUMMY_BANK_SLOT1_1024K;
	}
	return 0;
}

utils::dataContainer* modchipDummy::readBank(uint8_t bank)
{
	uint32_t bankSize = getBankSize(bank);
	utils::dataContainer* dataContainer = new utils::dataContainer(bankSize);
    memcpy(dataContainer->data, mFlashData + getBankMemOffset(bank), bankSize);
	return dataContainer;
}

void modchipDummy::eraseBank(uint8_t bank)
{
	uint32_t bankSize = getBankSize(bank);
	uint32_t memOffset = getBankMemOffset(bank);
	uint32_t startOffset = getBankStartOffset(bank);

	uint32_t offset = 0;
    while (offset < bankSize)
	{
		if (isEraseMemOffset(memOffset + startOffset + offset))
		{
			sectorErase(memOffset + offset);
		}
		offset += 8192;
	}
}

void modchipDummy::writeBank(uint8_t bank, utils::dataContainer* dataContainer)
{
	uint32_t memOffset = getBankMemOffset(bank);
    for (uint32_t i = 0; i < dataContainer->size; i++)
	{
		uint8_t value = (uint8_t)dataContainer->data[i];
		mFlashData[memOffset + i] = value;
    }
}

bool modchipDummy::verifyBank(uint8_t bank, utils::dataContainer* dataContainer)
{
	utils::dataContainer* writtenData = readBank(bank);

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
	return ok;
}

uint8_t modchipDummy::getFlashBankCount(bool recovery)
{
	return recovery ? 0 : 7;
}

uint8_t modchipDummy::getFlashBank(bool recovery, uint8_t bank)
{
	const uint8_t banks[] = { 
		DUMMY_BANK_SLOT1_256K, 
		DUMMY_BANK_SLOT2_256K, 
		DUMMY_BANK_SLOT3_256K, 
		DUMMY_BANK_SLOT4_256K,
		DUMMY_BANK_BOOTLOADER,
		DUMMY_BANK_PROMETHEOS1,
		DUMMY_BANK_PROMETHEOS2,
	};
	return recovery ? 0 : banks[bank];
}

bankType modchipDummy::getFlashBankType(bool recovery, uint8_t bank)
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

utils::dataContainer* modchipDummy::readFlash(bool recovery)
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

void modchipDummy::launchBank(uint8_t bank, uint8_t ledColor)
{
}

void modchipDummy::launchTsop()
{
}

void modchipDummy::launchRecovery()
{
}

#ifndef TOOLS

void modchipDummy::loadSettings(settingsState& settings)
{
	uint32_t memOffset = getBankMemOffset(DUMMY_SETTINGS_BANK);

    memcpy(&settings, mFlashData + memOffset + DUMMY_SETTINGS_OFFSET , sizeof(settings));
	uint32_t checksum = crc32::calculate(((uint8_t*)&settings) + sizeof(uint32_t), sizeof(settings) - sizeof(uint32_t));

	if (checksum != settings.checksum || versioning::compareVersion(settings.version, settingsManager::getVersion()) != 0)
	{
		settingsManager::initSettings();
		settingsManager::saveSettings();
	}
}

void modchipDummy::saveSettings(settingsState& settings) 
{
	uint32_t memOffset = getBankMemOffset(DUMMY_SETTINGS_BANK); 

	settings.checksum = crc32::calculate(((uint8_t*)&settings) + sizeof(uint32_t), sizeof(settings) - sizeof(uint32_t));
	utils::dataContainer* settingsData = new utils::dataContainer((char*)&settings, sizeof(settings), sizeof(settings));

	sectorErase(memOffset + DUMMY_SETTINGS_OFFSET);

    for (uint32_t i = 0; i < settingsData->size; i++)
	{
		uint8_t value = (uint8_t)settingsData->data[i];
		(mFlashData + memOffset + DUMMY_SETTINGS_OFFSET)[i] = value;
    }
}

#endif

utils::dataContainer* modchipDummy::getInstallerLogo()
{
	uint32_t memOffset = getBankMemOffset(DUMMY_INSTALLER_LOGO_BANK); 
	utils::dataContainer* installerLogo = new utils::dataContainer(32768);
    memcpy(installerLogo->data, mFlashData + memOffset + DUMMY_INSTALLER_LOGO_OFFSET, installerLogo->size);
	return installerLogo;
}

displayDriver* modchipDummy::getDisplayDriver(bool current)
{
	uint8_t lcdMode = settingsManager::getLcdMode(current);
	uint8_t lcdModel = settingsManager::getLcdModel(current);
	if (lcdMode == 1)
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

supportInfo modchipDummy::getSupportInfo(bool current)
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

uint8_t modchipDummy::getLcdModeCount()
{
	return 2;
}

char* modchipDummy::getLcdModeString(uint8_t lcdMode)
{
	if (lcdMode == 1)
	{
		return strdup("SMBUS");
	}
	return strdup("Disabled");
}

uint8_t modchipDummy::getLcdModelCount(bool current)
{
	uint8_t lcdMode = settingsManager::getLcdMode(current);
	if (lcdMode == 1)
	{
		return 2;
	}
	return 0;
}

char* modchipDummy::getLcdModelString(bool current, uint8_t lcdModel)
{
	displayDriver* driver = getDisplayDriver(current);
	if (driver == NULL)
	{
		return strdup("");
	}
	return driver->getModel();
}

uint8_t modchipDummy::getLcdAddressCount(bool current)
{
	displayDriver* driver = getDisplayDriver(current);
	if (driver == NULL)
	{
		return 0;
	}
	return driver->getI2cAddressCount();
}

char* modchipDummy::getLcdAddressString(bool current, uint8_t lcdAddress)
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

void modchipDummy::lcdSetCursorPosition(uint8_t row, uint8_t col)
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

void modchipDummy::lcdInit()
{
	displayDriver* driver = getDisplayDriver(true);
	if (driver == NULL)
	{
		return;
	}
	driver->setAddress(settingsManager::getLcdAddress(true));
	driver->init();
}

void modchipDummy::lcdPrintMessage(const char* message)
{
	displayDriver* driver = getDisplayDriver(true);
	if (driver == NULL)
	{
		return;
	}
	driver->setAddress(settingsManager::getLcdAddress(true));
	driver->printMessage(message);
}

void modchipDummy::lcdSetBacklight(uint8_t value)
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

void modchipDummy::lcdSetContrast(uint8_t value)
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

void modchipDummy::setBank(uint8_t bank)
{
}

uint8_t modchipDummy::getBank()
{
	return 0;
}

bool modchipDummy::isEraseMemOffset(uint32_t memOffset)
{
	return (memOffset % 8192) == 0;
}

bool modchipDummy::isProtectedMemOffset(uint32_t memOffset)
{
	return false;
}

void modchipDummy::sectorErase(uint32_t offset)
{
	memset(mFlashData + offset, 0, 8192);
}