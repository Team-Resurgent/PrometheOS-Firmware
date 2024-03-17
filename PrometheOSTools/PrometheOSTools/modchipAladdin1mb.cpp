#include "modchipAladdin1mb.h"
#include "crc32.h"

#ifndef TOOLS
#include "settingsManager.h"
#endif

#define ALADDIN1MB_LCD_DELAY 2

#define ALADDIN1MB_REGISTER_IO    0xF70Du
#define ALADDIN1MB_REGISTER_CONTROL    0xF70Fu
#define ALADDIN1MB_REGISTER_DISP_O 0xF700u
#define ALADDIN1MB_REGISTER_LCD_BL 0xF701u
#define ALADDIN1MB_REGISTER_LCD_CT 0xF703u
#define ALADDIN1MB_REGISTER_BANKING 0xF70F

#define ALADDIN1MB_DISP_EXT_CONTROL 0x08
#define ALADDIN1MB_DISP_NW_FLAG 0x01

#define ALADDIN1MB_DISP_CON_RS 0x01
#define ALADDIN1MB_DISP_CON_RW 0x02
#define ALADDIN1MB_DISP_CON_E 0x04
#define ALADDIN1MB_DISP_INI 0x01
#define ALADDIN1MB_DISP_CMD 0x00
#define ALADDIN1MB_DISP_DAT 0x02
#define ALADDIN1MB_DISP_CLEAR 0x01
#define ALADDIN1MB_DISP_HOME 0x02
#define ALADDIN1MB_DISP_ENTRY_MODE_SET	0x04
#define ALADDIN1MB_DISP_S_FLAG 0x01
#define ALADDIN1MB_DISP_ID_FLAG 0x02
#define ALADDIN1MB_DISP_CONTROL 0x08
#define ALADDIN1MB_DISP_D_FLAG 0x04
#define ALADDIN1MB_DISP_C_FLAG 0x02
#define ALADDIN1MB_DISP_B_FLAG 0x01
#define ALADDIN1MB_DISP_SHIFT 0x10
#define ALADDIN1MB_DISP_SC_FLAG 0x08
#define ALADDIN1MB_DISP_RL_FLAG 0x04
#define ALADDIN1MB_DISP_FUNCTION_SET 0x20
#define ALADDIN1MB_DISP_DL_FLAG 0x10
#define ALADDIN1MB_DISP_N_FLAG 0x08
#define ALADDIN1MB_DISP_F_FLAG 0x04
#define ALADDIN1MB_DISP_RE_FLAG 0x04
#define ALADDIN1MB_DISP_CGRAM_SET 0x40
#define ALADDIN1MB_DISP_SEGRAM_SET 0x40
#define ALADDIN1MB_DISP_DDRAM_SET 0x80

#define ALADDIN1MB_BANK_TSOP 0
#define ALADDIN1MB_BANK_BOOTLOADER 0x87
#define ALADDIN1MB_BANK_PROMETHEOS 0x84
#define ALADDIN1MB_BANK_SLOT1_256K 0x86

#define ALADDIN1MB_LCD_EN5V 0x01

#define ALADDIN1MB_SETTINGS_BANK ALADDIN1MB_BANK_PROMETHEOS
#define ALADDIN1MB_SETTINGS_OFFSET (0x07f000)

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
	mSpi = 0;
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

bool modchipAladdin1mb::supportsLed()
{
	return false;
}

bool modchipAladdin1mb::supportsLcd()
{
	return true;
}

bool modchipAladdin1mb::supportsLcdInfo()
{
	return false;
}

bool modchipAladdin1mb::supportsLcdContrast()
{
	return true;
}

bool modchipAladdin1mb::supportsRecovery()
{
	return false;
}

void modchipAladdin1mb::disableRecovery()
{
}

bool modchipAladdin1mb::isValidBankSize(uint32_t size)
{
	return size == (256 * 1024);
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
    volatile uint8_t* lpcMemMap = (volatile uint8_t *)LPC_MEMORY_BASE;
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
	uint32_t bankSize = getBankSize(bank);

	uint32_t offset = 0;
    while (offset < bankSize)
	{
		if (isEraseMemOffset(memOffset + offset))
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

	volatile uint8_t* lpcMemMap = (volatile uint8_t *)LPC_MEMORY_BASE;

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
	uint32_t checksum = crc32::calculate(((char*)&settings) + sizeof(uint32_t), sizeof(settings) - sizeof(uint32_t));

	setBank(ALADDIN1MB_BANK_BOOTLOADER);

	if (checksum != settings.checksum || versioning::compareVersion(settings.version, settingsManager::getVersion()) != 0)
	{
		settingsManager::initSettings();
		settingsManager::saveSettings();
	}

	setLedColor(settingsManager::getLedColor());
}

void modchipAladdin1mb::saveSettings(settingsState settings) 
{
	setBank(ALADDIN1MB_SETTINGS_BANK); 

	settings.checksum = crc32::calculate(((const char*)&settings) + sizeof(uint32_t), sizeof(settings) - sizeof(uint32_t));
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

void modchipAladdin1mb::lcdSendCharacter(uint8_t value, uint8_t command)
{
	uint8_t oDataHigh  = (value >> 2) & 0x28;
	oDataHigh |= (value >> 0) & 0x50;
	outputByte(ALADDIN1MB_REGISTER_DISP_O, command | oDataHigh);	
	outputByte(ALADDIN1MB_REGISTER_DISP_O, command | ALADDIN1MB_DISP_CON_E | oDataHigh);	
	outputByte(ALADDIN1MB_REGISTER_DISP_O, command | oDataHigh);	
	Sleep(ALADDIN1MB_LCD_DELAY);
	
	uint8_t oDataLow  = (value << 2) & 0x28;
	oDataLow |= (value << 4) & 0x50;
	outputByte(ALADDIN1MB_REGISTER_DISP_O, command | oDataLow);	
	outputByte(ALADDIN1MB_REGISTER_DISP_O, command | ALADDIN1MB_DISP_CON_E | oDataLow);	
	outputByte(ALADDIN1MB_REGISTER_DISP_O, command | oDataLow);	
	Sleep(ALADDIN1MB_LCD_DELAY);
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

	uint8_t value = 0;
	if (row == 1)
	{
		value = 0x40;
	}
	else if (row == 2)
	{
		value = 0x14;
	}
	else if (row == 3)
	{
		value = 0x54;
	}

	lcdSendCharacter(ALADDIN1MB_DISP_DDRAM_SET | (value + col), ALADDIN1MB_DISP_CMD);
	Sleep(10);
}

void modchipAladdin1mb::lcdInit(uint8_t backlight, uint8_t contrast)
{
	outputByte(ALADDIN1MB_REGISTER_IO, ALADDIN1MB_LCD_EN5V);

	lcdSendCharacter(0x33, 0);
	Sleep(ALADDIN1MB_LCD_DELAY);

	lcdSendCharacter(0x32, 0);  
	Sleep(ALADDIN1MB_LCD_DELAY);

	lcdSendCharacter(ALADDIN1MB_DISP_FUNCTION_SET | ALADDIN1MB_DISP_N_FLAG | ALADDIN1MB_DISP_RE_FLAG, ALADDIN1MB_DISP_CMD);
	Sleep(ALADDIN1MB_LCD_DELAY);
    lcdSendCharacter(ALADDIN1MB_DISP_SEGRAM_SET, ALADDIN1MB_DISP_CMD);
	Sleep(ALADDIN1MB_LCD_DELAY);
    lcdSendCharacter(ALADDIN1MB_DISP_EXT_CONTROL | ALADDIN1MB_DISP_NW_FLAG, ALADDIN1MB_DISP_CMD);
	Sleep(ALADDIN1MB_LCD_DELAY);
    lcdSendCharacter(ALADDIN1MB_DISP_FUNCTION_SET | ALADDIN1MB_DISP_N_FLAG, ALADDIN1MB_DISP_CMD); 
	Sleep(ALADDIN1MB_LCD_DELAY);
    lcdSendCharacter(ALADDIN1MB_DISP_CONTROL | ALADDIN1MB_DISP_D_FLAG, ALADDIN1MB_DISP_CMD);
	Sleep(ALADDIN1MB_LCD_DELAY);
    lcdSendCharacter(ALADDIN1MB_DISP_CLEAR, ALADDIN1MB_DISP_CMD);
	Sleep(ALADDIN1MB_LCD_DELAY);
    lcdSendCharacter(ALADDIN1MB_DISP_ENTRY_MODE_SET | ALADDIN1MB_DISP_ID_FLAG, ALADDIN1MB_DISP_CMD);
	Sleep(ALADDIN1MB_LCD_DELAY);
    lcdSendCharacter(ALADDIN1MB_DISP_HOME, ALADDIN1MB_DISP_CMD);  
	Sleep(ALADDIN1MB_LCD_DELAY);

	lcdSetBacklight(backlight);
	lcdSetContrast(contrast);
	Sleep(ALADDIN1MB_LCD_DELAY);
}

void modchipAladdin1mb::lcdPrintMessage(const char* message)
{
	for (int i = 0; i < (int)strlen(message); ++i)
	{
		uint8_t cLCD = message[i];
		lcdSendCharacter(cLCD, ALADDIN1MB_DISP_DAT);
		Sleep(ALADDIN1MB_LCD_DELAY);
	}
}

void modchipAladdin1mb::lcdSetBacklight(uint8_t value)
{
	if (value > 100)
	{
		value = 100;
	}
	outputByte(ALADDIN1MB_REGISTER_LCD_BL, (int)(value * 1.27f));
	Sleep(ALADDIN1MB_LCD_DELAY);
}

void modchipAladdin1mb::lcdSetContrast(uint8_t value)
{
	if (value > 100)
	{
		value = 100;
	}
	outputByte(ALADDIN1MB_REGISTER_LCD_CT, (int)(value * 0.64f));
	Sleep(ALADDIN1MB_LCD_DELAY);
}

// Private

void modchipAladdin1mb::setBank(uint8_t bank)
{
	mBank = bank;
    outputByte(ALADDIN1MB_REGISTER_BANKING, mBank);
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
    lpcMemMap[0x5555]=0xF0;
}
