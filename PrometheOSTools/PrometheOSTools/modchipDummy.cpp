#include "modchipDummy.h"
#include "crc32.h"

#ifndef TOOLS
#include "settingsManager.h"
#endif

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

bool modchipDummy::supportsLed()
{
	return false;
}

bool modchipDummy::supportsLcd()
{
	return false;
}

bool modchipDummy::supportsLcdInfo()
{
	return false;
}

bool modchipDummy::supportsLcdContrast()
{
	return false;
}

bool modchipDummy::supportsRecovery()
{
	return false;
}

void modchipDummy::disableRecovery()
{
}

bool modchipDummy::isValidBankSize(uint32_t size)
{
	return size == (256 * 1024) || size == (512 * 1024) || size == (1024 * 1024);
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

	uint32_t offset = 0;
    while (offset < bankSize)
	{
		if (isEraseMemOffset(memOffset + offset))
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
	uint32_t checksum = crc32::calculate(((char*)&settings) + sizeof(uint32_t), sizeof(settings) - sizeof(uint32_t));

	if (checksum != settings.checksum || versioning::compareVersion(settings.version, settingsManager::getVersion()) != 0)
	{
		settingsManager::initSettings();
		settingsManager::saveSettings();
	}
}

void modchipDummy::saveSettings(settingsState settings) 
{
	uint32_t memOffset = getBankMemOffset(DUMMY_SETTINGS_BANK); 

	settings.checksum = crc32::calculate(((const char*)&settings) + sizeof(uint32_t), sizeof(settings) - sizeof(uint32_t));
	utils::dataContainer* settingsData = new utils::dataContainer((char*)&settings, sizeof(settings), sizeof(settings));

	sectorErase(memOffset + DUMMY_SETTINGS_OFFSET);

    for (uint32_t i = 0; i < settingsData->size; i++)
	{
		uint8_t value = (uint8_t)settingsData->data[i];
		*(mFlashData + memOffset + DUMMY_SETTINGS_OFFSET) = value;
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

void modchipDummy::lcdSendCharacter(uint8_t value, uint8_t command)
{
}

void modchipDummy::lcdSetCursorPosition(uint8_t row, uint8_t col)
{
}

void modchipDummy::lcdInit(uint8_t backlight, uint8_t contrast)
{
}

void modchipDummy::lcdPrintMessage(const char* message)
{
}

void modchipDummy::lcdSetBacklight(uint8_t value)
{
}

void modchipDummy::lcdSetContrast(uint8_t value)
{
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