#include "modchipXecuter.h"
#include "crc32.h"

#ifndef TOOLS
#include "settingsManager.h"
#endif

#define XECUTER_LCD_DELAY 2

#define XECUTER_REGISTER_HARDWARE_BANKING 0xF501
#define XECUTER_REGISTER_SOFTWARE_BANKING 0xF502
#define XECUTER_REGISTER_LED 0x00EE
#define XECUTER_SPI_MOSI_BIT 0x10
#define XECUTER_SPI_CS_BIT 0x20
#define XECUTER_SPI_CLK_BIT 0x40

#define XECUTER_BANK_TSOP 0x6f
#define XECUTER_BANK_BACKUP_256K 0x8f 
#define XECUTER_DISABLE_BACKUP 0xf 

#define XECUTER_BANK_BOOTLOADER 0x00
#define XECUTER_BANK_PROMETHEOS1 0x01
#define XECUTER_BANK_PROMETHEOS2 0x02
#define XECUTER_BANK_PROMETHEOS3 0x03
#define XECUTER_BANK_SLOT1_256K 0x04
#define XECUTER_BANK_SLOT2_256K 0x05
#define XECUTER_BANK_SLOT3_256K 0x06
#define XECUTER_BANK_SLOT4_256K 0x07
#define XECUTER_BANK_SLOT1_512K 0x0a
#define XECUTER_BANK_SLOT2_512K 0x0b
#define XECUTER_BANK_SLOT1_1024K 0x0d

#define XECUTER_SETTINGS_BANK XECUTER_BANK_PROMETHEOS3
#define XECUTER_SETTINGS_OFFSET (0x0f0000 - 0x0c0000)
#define XECUTER_INSTALLER_LOGO_BANK XECUTER_BANK_PROMETHEOS3
#define XECUTER_INSTALLER_LOGO_OFFSET (0x0e0000 - 0x0c0000)

#define XECUTER_REGISTER_DISP_O_LIGHT 0xf503
#define XECUTER_REGISTER_DISP_O_DAT 0xf504
#define XECUTER_REGISTER_DISP_O_CMD 0xf505
#define XECUTER_REGISTER_DISP_O_DIR_DAT 0xf506
#define XECUTER_REGISTER_DISP_O_DIR_CMD 0xf507

#define XECUTER_DISP_CON_RS 0x01
#define XECUTER_DISP_CON_RW 0x02
#define XECUTER_DISP_CON_E 0x04
#define XECUTER_DISP_INI 0x01
#define XECUTER_DISP_CMD 0x00
#define XECUTER_DISP_DAT 0x02
#define XECUTER_DISP_CLEAR 0x01
#define XECUTER_DISP_HOME 0x02
#define XECUTER_DISP_ENTRY_MODE_SET	0x04
#define XECUTER_DISP_S_FLAG 0x01
#define XECUTER_DISP_ID_FLAG 0x02
#define XECUTER_DISP_CONTROL 0x08
#define XECUTER_DISP_D_FLAG 0x04
#define XECUTER_DISP_C_FLAG 0x02
#define XECUTER_DISP_B_FLAG 0x01
#define XECUTER_DISP_SHIFT 0x10
#define XECUTER_DISP_SC_FLAG 0x08
#define XECUTER_DISP_RL_FLAG 0x04
#define XECUTER_DISP_FUNCTION_SET 0x20
#define XECUTER_DISP_DL_FLAG 0x10
#define XECUTER_DISP_N_FLAG 0x08
#define XECUTER_DISP_F_FLAG 0x04
#define XECUTER_DISP_RE_FLAG 0x04
#define XECUTER_DISP_CGRAM_SET 0x40
#define XECUTER_DISP_SEGRAM_SET 0x40
#define XECUTER_DISP_DDRAM_SET 0x80



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

bool modchipXecuter::supportsLed()
{
	return false;
}

bool modchipXecuter::supportsLcd()
{
	return true;
}

bool modchipXecuter::supportsLcdInfo()
{
	return false;
}

bool modchipXecuter::supportsLcdContrast()
{
	return false;
}

bool modchipXecuter::supportsRecovery()
{
	return true;
}

void modchipXecuter::disableRecovery()
{
	outputByte(XECUTER_REGISTER_HARDWARE_BANKING, XECUTER_DISABLE_BACKUP);
}

bool modchipXecuter::isValidBankSize(uint32_t size)
{
	return size == (256 * 1024) || size == (512 * 1024) || size == (1024 * 1024);
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
    volatile uint8_t* lpcMemMap = (volatile uint8_t *)LPC_MEMORY_BASE;
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
		volatile uint8_t* lpcMemMap = (volatile uint8_t *)LPC_MEMORY_BASE;

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
	uint32_t checksum = crc32::calculate(((char*)&settings) + sizeof(uint32_t), sizeof(settings) - sizeof(uint32_t));

	setBank(XECUTER_BANK_BOOTLOADER);

	if (checksum != settings.checksum || versioning::compareVersion(settings.version, settingsManager::getVersion()) != 0)
	{
		settingsManager::initSettings();
		settingsManager::saveSettings();
	}

	setLedColor(settingsManager::getLedColor());
}

void modchipXecuter::saveSettings(settingsState settings) 
{
	if (checkWriteProtect() == true)
	{
		return;
	}

	setBank(XECUTER_SETTINGS_BANK); 

	settings.checksum = crc32::calculate(((const char*)&settings) + sizeof(uint32_t), sizeof(settings) - sizeof(uint32_t));
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

void modchipXecuter::lcdSendCharacter(uint8_t value, uint8_t command)
{
	uint8_t cmd = 0;

	if (command & XECUTER_DISP_DAT)
	{
		cmd |= XECUTER_DISP_CON_RS;
	}

	outputByte(XECUTER_REGISTER_DISP_O_DAT, value & 0xF0);
	outputByte(XECUTER_REGISTER_DISP_O_CMD, cmd);	
	outputByte(XECUTER_REGISTER_DISP_O_CMD, XECUTER_DISP_CON_E | cmd);	
	outputByte(XECUTER_REGISTER_DISP_O_CMD, cmd);	

	if ((command & XECUTER_DISP_INI) == 0) 
	{							
		outputByte(XECUTER_REGISTER_DISP_O_DAT, (value << 4) & 0xF0);
		outputByte(XECUTER_REGISTER_DISP_O_CMD, cmd);
		outputByte(XECUTER_REGISTER_DISP_O_CMD, XECUTER_DISP_CON_E | cmd);
		outputByte(XECUTER_REGISTER_DISP_O_CMD, cmd);
	}
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

	lcdSendCharacter(XECUTER_DISP_DDRAM_SET | (value + col), XECUTER_DISP_CMD);
	Sleep(10);
}

void modchipXecuter::lcdInit(uint8_t backlight, uint8_t contrast)
{
	outputByte(XECUTER_REGISTER_DISP_O_DAT, 0);
	outputByte(XECUTER_REGISTER_DISP_O_CMD, 0);
	outputByte(XECUTER_REGISTER_DISP_O_DIR_DAT, 0xFF);
	outputByte(XECUTER_REGISTER_DISP_O_DIR_CMD, 0x07);

	lcdSendCharacter(XECUTER_DISP_FUNCTION_SET | XECUTER_DISP_DL_FLAG, XECUTER_DISP_INI);
	Sleep(XECUTER_LCD_DELAY);
	lcdSendCharacter(XECUTER_DISP_FUNCTION_SET | XECUTER_DISP_DL_FLAG, XECUTER_DISP_INI);	
	Sleep(XECUTER_LCD_DELAY);
	lcdSendCharacter(XECUTER_DISP_FUNCTION_SET | XECUTER_DISP_DL_FLAG, XECUTER_DISP_INI);	
	Sleep(XECUTER_LCD_DELAY);
	lcdSendCharacter(XECUTER_DISP_FUNCTION_SET, XECUTER_DISP_INI);								
	Sleep(XECUTER_LCD_DELAY);
	lcdSendCharacter(XECUTER_DISP_FUNCTION_SET | XECUTER_DISP_N_FLAG, XECUTER_DISP_CMD);	
	Sleep(XECUTER_LCD_DELAY);
	lcdSendCharacter(XECUTER_DISP_CONTROL | XECUTER_DISP_D_FLAG, XECUTER_DISP_CMD);		
	Sleep(XECUTER_LCD_DELAY);
	lcdSendCharacter(XECUTER_DISP_CLEAR, XECUTER_DISP_CMD);			
	Sleep(XECUTER_LCD_DELAY);
	lcdSendCharacter(XECUTER_DISP_ENTRY_MODE_SET | XECUTER_DISP_ID_FLAG,XECUTER_DISP_CMD);	
	Sleep(XECUTER_LCD_DELAY);

	lcdSetBacklight(backlight);
	lcdSetContrast(contrast);
	Sleep(XECUTER_LCD_DELAY);
}

void modchipXecuter::lcdPrintMessage(const char* message)
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
		lcdSendCharacter(cLCD, XECUTER_DISP_DAT);
		Sleep(XECUTER_LCD_DELAY);
	}
}

void modchipXecuter::lcdSetBacklight(uint8_t value)
{
	if (value > 100)
	{
		value = 100;
	}
	outputByte(XECUTER_REGISTER_DISP_O_LIGHT, (int)(value * 2.55f));
	Sleep(XECUTER_LCD_DELAY);
}

void modchipXecuter::lcdSetContrast(uint8_t value)
{
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