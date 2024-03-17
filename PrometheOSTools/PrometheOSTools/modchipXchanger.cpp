#include "modchipXchanger.h"
#include "crc32.h"

#define XCHANGER_REGISTER_BANKING 0x1912
//#define XCHANGER_REGISTER_LED 0x00EE
#define XCHANGER_SPI_CLK_BIT 0x40
#define XCHANGER_SPI_MOSI_BIT 0x10

#define XCHANGER_BANK_TSOP 0x3
#define XCHANGER_BANK_BOOTLOADER 0x0
#define XCHANGER_BANK_SLOT1_256K 0x1
#define XCHANGER_BANK_PROMETHEOS 0x2

#define XCHANGER_SETTINGS_BANK XCHANGER_BANK_PROMETHEOS
#define XCHANGER_SETTINGS_OFFSET (0x07f000)
//#define XCHANGER_INSTALLER_LOGO_BANK XCHANGER_BANK_PROMETHEOS2
//#define XCHANGER_INSTALLER_LOGO_OFFSET (0x1f0000 - 0x1c0000)

//Bank_PrometheOS =  0x84  0x000000 - 0x07ffff  Sector Size 4096  Total  524288 (See below)
//Bank1_256k      =  0x86  0x080000 - 0x0bffff  Sector Size 4096  Total  262144
//Bank_Bootloader =  0x87  0x0C0000 - 0x0fffff  Sector Size 4096  Total  262144

//0x000000 - 0x07efff  Sector Size 4096  Total 520192 (PrometheOS)
//0x07f000 - 0x07ffff  Sector Size 4096  Total 4096   (Settings)

modchipXchanger::modchipXchanger()
{
	mBank = XCHANGER_BANK_BOOTLOADER;
	mSpi = 0;
}

void modchipXchanger::setLedColor(uint8_t ledColor)
{
    //outputByte(XCHANGER_REGISTER_LED, ledColor);
}

uint32_t modchipXchanger::getSlotCount()
{
	return 1;
}

uint32_t modchipXchanger::getFlashSize(bool recovery)
{
	return recovery ? 0 : (1 * 1024 * 1024);
}

bool modchipXchanger::supportsLed()
{
	return false;
}

bool modchipXchanger::supportsLcd()
{
	return false;
}

bool modchipXchanger::supportsLcdInfo()
{
	return false;
}

bool modchipXchanger::supportsLcdContrast()
{
	return false;
}

bool modchipXchanger::supportsRecovery()
{
	return false;
}

void modchipXchanger::disableRecovery()
{
}

bool modchipXchanger::isValidBankSize(uint32_t size)
{
	return size == (256 * 1024);
}

uint32_t modchipXchanger::getBankSize(uint8_t bank)
{
	if (bank == XCHANGER_BANK_PROMETHEOS) 
	{
		return 512 * 1024;
	}
	if (bank == XCHANGER_BANK_SLOT1_256K) 
	{
		return 256 * 1024;
	}
	if (bank == XCHANGER_BANK_BOOTLOADER) 
	{
		return 256 * 1024;
	}
	return 0;
}

uint32_t modchipXchanger::getBankMemOffset(uint8_t bank)
{
	if (bank == XCHANGER_BANK_PROMETHEOS)
	{
		return 0x000000;
	}
	if (bank == XCHANGER_BANK_SLOT1_256K)
	{
		return 0x080000;
	}
	if (bank == XCHANGER_BANK_BOOTLOADER)
	{
		return 0x0c0000;
	}
	return 0;
}

uint8_t modchipXchanger::getBankFromIdAndSlots(uint8_t id, uint8_t slots)
{
	if (id == 0 && slots == 1)
	{
		return XCHANGER_BANK_SLOT1_256K;
	}
	return 0;
}

utils::dataContainer* modchipXchanger::readBank(uint8_t bank)
{
	setBank(bank); 
	uint32_t bankSize = getBankSize(bank);
	utils::dataContainer* dataContainer = new utils::dataContainer(bankSize);
    volatile uint8_t* lpcMemMap = (volatile uint8_t *)LPC_MEMORY_BASE;
    memcpy(dataContainer->data, (void*)&lpcMemMap[0], bankSize);
	setBank(XCHANGER_BANK_BOOTLOADER);
	return dataContainer;
}

void modchipXchanger::eraseBank(uint8_t bank)
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

	setBank(XCHANGER_BANK_BOOTLOADER);

	setLedColor(LED_COLOR_GREEN);
}

void modchipXchanger::writeBank(uint8_t bank, utils::dataContainer* dataContainer)
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

	setBank(XCHANGER_BANK_BOOTLOADER);

	setLedColor(LED_COLOR_GREEN);
}

bool modchipXchanger::verifyBank(uint8_t bank, utils::dataContainer* dataContainer)
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

	setBank(XCHANGER_BANK_BOOTLOADER);

	setLedColor(LED_COLOR_GREEN);
	return ok;
}

uint8_t modchipXchanger::getFlashBankCount(bool recovery)
{
	return recovery ? 0 : 3;
}

uint8_t modchipXchanger::getFlashBank(bool recovery, uint8_t bank)
{
	const uint8_t banks[] = { 
		XCHANGER_BANK_SLOT1_256K, 
		XCHANGER_BANK_BOOTLOADER,
		XCHANGER_BANK_PROMETHEOS
	};
	return recovery ? 0 : banks[bank];
}

utils::dataContainer* modchipXchanger::readFlash(bool recovery)
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

void modchipXchanger::launchBank(uint8_t bank, uint8_t ledColor)
{
	DWORD scratch;
	HalReadSMBusByte(SMBDEV_PIC16L, PIC16L_CMD_SCRATCH_REGISTER, &scratch);
	HalWriteSMBusByte(SMBDEV_PIC16L, PIC16L_CMD_SCRATCH_REGISTER, scratch & ~SCRATCH_REGISTER_BITVALUE_NO_ANIMATION);
	setBank(bank);
	setLedColor(ledColor);
	HalReturnToFirmware(RETURN_FIRMWARE_REBOOT);
}

void modchipXchanger::launchTsop()
{
	DWORD scratch;
	HalReadSMBusByte(SMBDEV_PIC16L, PIC16L_CMD_SCRATCH_REGISTER, &scratch);
	HalWriteSMBusByte(SMBDEV_PIC16L, PIC16L_CMD_SCRATCH_REGISTER, scratch & ~SCRATCH_REGISTER_BITVALUE_NO_ANIMATION);
	setBank(XCHANGER_BANK_TSOP);
	setLedColor(LED_COLOR_OFF);
	HalReturnToFirmware(RETURN_FIRMWARE_REBOOT);
}

void modchipXchanger::launchRecovery()
{
}

#ifndef TOOLS

void modchipXchanger::loadSettings(settingsState& settings)
{
	setBank(XCHANGER_SETTINGS_BANK); 

	setLedColor(LED_COLOR_WHITE);

    volatile uint8_t* lpcMemMap = (volatile uint8_t *)LPC_MEMORY_BASE;

    memcpy(&settings, (void*)&lpcMemMap[XCHANGER_SETTINGS_OFFSET], sizeof(settings));
	uint32_t checksum = crc32::calculate(((char*)&settings) + sizeof(uint32_t), sizeof(settings) - sizeof(uint32_t));

	setBank(XCHANGER_BANK_BOOTLOADER);

	if (checksum != settings.checksum || versioning::compareVersion(settings.version, settingsManager::getVersion()) != 0)
	{
		settingsManager::initSettings();
		settingsManager::saveSettings();
	}

	setLedColor(settingsManager::getLedColor());
}

void modchipXchanger::saveSettings(settingsState settings) 
{
	setBank(XCHANGER_SETTINGS_BANK); 

	settings.checksum = crc32::calculate(((const char*)&settings) + sizeof(uint32_t), sizeof(settings) - sizeof(uint32_t));
	utils::dataContainer* settingsData = new utils::dataContainer((char*)&settings, sizeof(settings), sizeof(settings));

	volatile uint8_t* lpcMemMap = (volatile uint8_t *)LPC_MEMORY_BASE;

	setLedColor(LED_COLOR_AMBER);
	sectorErase(XCHANGER_SETTINGS_OFFSET);

	setLedColor(LED_COLOR_BLUE);
    for (uint32_t i = 0; i < settingsData->size; i++)
	{
		uint8_t value = (uint8_t)settingsData->data[i];
		lpcMemMap[0x5555] = 0xAA;
		lpcMemMap[0x2AAA] = 0x55;
		lpcMemMap[0x5555] = 0xA0;
		lpcMemMap[XCHANGER_SETTINGS_OFFSET + i] = value;
		while (lpcMemMap[XCHANGER_SETTINGS_OFFSET + i] != value);
    }
	lpcMemMap[0] = 0xF0;

	setBank(XCHANGER_BANK_BOOTLOADER);

	setLedColor(settingsManager::getLedColor());
}

#endif

utils::dataContainer* modchipXchanger::getInstallerLogo()
{
	utils::dataContainer* installerLogo = new utils::dataContainer(32768);
	return installerLogo;
}

void modchipXchanger::lcdSendCharacter(uint8_t value, uint8_t command)
{
}

void modchipXchanger::lcdSetCursorPosition(uint8_t row, uint8_t col)
{
}

void modchipXchanger::lcdInit(uint8_t backlight, uint8_t contrast)
{
}

void modchipXchanger::lcdPrintMessage(const char* message)
{
}

void modchipXchanger::lcdSetBacklight(uint8_t value)
{
}

void modchipXchanger::lcdSetContrast(uint8_t value)
{
}

// Private

void modchipXchanger::setBank(uint8_t bank)
{
	mBank = bank;
	outputByte(XCHANGER_REGISTER_BANKING, bank);
}

uint8_t modchipXchanger::getBank()
{
	//mBank = inputByte(XCHANGER_REGISTER_BANKING);
	return mBank;
}

bool modchipXchanger::isEraseMemOffset(uint32_t memOffset)
{
	return (memOffset % 4096) == 0;
}

bool modchipXchanger::isProtectedMemOffset(uint32_t memOffset)
{
	if (isProtectedMemOffset(memOffset))
	{
		return true;
	}
	return false;
}

void modchipXchanger::sectorErase(uint32_t offset)
{
	volatile uint8_t* lpcMemMap = (volatile uint8_t *)LPC_MEMORY_BASE;
	lpcMemMap[0x5555] = 0xAA;
	lpcMemMap[0x2AAA] = 0x55;
	lpcMemMap[0x5555] = 0x80;
	lpcMemMap[0x5555] = 0xAA;
	lpcMemMap[0x2AAA] = 0x55;
	lpcMemMap[offset] = 0x30;
	while(lpcMemMap[0] != lpcMemMap[0]);
	//lpcMemMap[0x5555]=0xaa;
 //   lpcMemMap[0x2aaa]=0x55;
    lpcMemMap[0x5555]=0xF0;
}
