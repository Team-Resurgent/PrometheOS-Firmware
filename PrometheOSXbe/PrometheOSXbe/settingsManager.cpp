#include "settingsManager.h"
#include "modchip.h"
#include "modchipXenium.h"
#include "context.h"
#include "versioning.h"
#include "crc32.h"
#include "stringUtility.h"
#include "utils.h"
#include "Threads\lcdRender.h"

#include <stdio.h>
#include <algorithm>

#define SETTINGS_OFFSET (0x1f8000 - 0x1c0000)
#define INSTALLER_LOGO_OFFSET (0x1f0000 - 0x1c0000)

namespace 
{
	settingsState mSettings;
}

semver settingsManager::getVersion()
{
	semver version;
	memset(&version, 0, sizeof(version));
	version.major = 1;
	version.minor = 4;
	version.patch = 0;
	return version;
}

char* settingsManager::getVersionSting(semver version)
{
	return stringUtility::formatString("%i.%i.%i", version.major, version.minor, version.patch);
}

#ifdef _DEBUG
#define TEST
#endif

void settingsManager::initSettings()
{
	memset(&mSettings, 0, sizeof(mSettings));
	mSettings.version = getVersion();
	mSettings.autoBootDelay = 5;
	mSettings.musicVolume = 75;
	mSettings.soundVolume = 100;
	mSettings.minFanSpeed = 10;
	mSettings.ledColor = 2;
	mSettings.lcdBacklight = 100;
	mSettings.lcdContrast = 60;
	mSettings.driveSetup = 1;
	mSettings.udmaModeMaster = 2;
	mSettings.udmaModeSlave = 2;
	mSettings.splashDelay = 6;
//#ifdef TEST
//	mSettings.banks[0].slots = 1;
//	stringUtility::copyString(&mSettings.banks[0].name[0], "Bank 1 Rom", 41);
//	mSettings.banks[1].slots = 1;
//	stringUtility::copyString(&mSettings.banks[1].name[0], "Bank 2 Rom", 41);
//	mSettings.banks[2].slots = 1;
//	stringUtility::copyString(&mSettings.banks[2].name[0], "Bank 3 Rom", 41);
//	mSettings.banks[3].slots = 1;
//	stringUtility::copyString(&mSettings.banks[3].name[0], "Bank 4 Rom", 41);
//
//	stringUtility::copyString(&mSettings.skinName[0], "TeamResurgent-Animated", 51);
//	stringUtility::copyString(&mSettings.soundPackName[0], "Cybernoid", 51);
//
//#endif
}

void settingsManager::loadSettings()
{
#ifndef TOOLS
	context::getModchip()->loadSettings(mSettings);
#else
	initSettings();
#endif
}

void settingsManager::saveSettings()
{
#ifndef TOOLS
	context::getModchip()->saveSettings(mSettings);
#endif
}

utils::dataContainer* settingsManager::getInstallerLogoData()
{
	return context::getModchip()->getInstallerLogo();
}

settingsState* settingsManager::getSettings()
{
	return &mSettings;
}

uint32_t settingsManager::getFreeSlots()
{
	uint32_t slotCount = context::getModchip()->getSlotCount();
	uint32_t freeSlots = slotCount;
	for (uint32_t i = 0; i < slotCount; i++)
	{
		freeSlots = freeSlots - mSettings.banks[i].slots;
	}
	return freeSlots;
}

char* settingsManager::getFreeSlotsJson()
{
	char* freeSlotsJson = stringUtility::formatString("{\"freeslots\":%lu}", getFreeSlots());
	return freeSlotsJson;
}

bankInfo settingsManager::getBankInfo(uint8_t id)
{
	return mSettings.banks[id];
}

pointerVector<bankDetails*>* settingsManager::getBankInfos()
{
	pointerVector<bankDetails*>* bankInfos = new pointerVector<bankDetails*>(true);

	uint32_t totalSlots = context::getModchip()->getSlotCount();
	for (uint32_t i = 0; i < totalSlots; i++)
	{
		if (mSettings.banks[i].slots == 0)
		{
			continue;
		}

		bankDetails* bank = new bankDetails();
		bank->id = (uint8_t)i;
		bank->ledColor = mSettings.banks[i].ledColor;
		bank->slots = mSettings.banks[i].slots;
		bank->name = strdup(&mSettings.banks[i].name[0]);
		bank->autoBoot = mSettings.banks[i].autoBoot == 1;
		bankInfos->add(bank);
	}

	return bankInfos;
}

char* settingsManager::getBankInfosJson()
{
	pointerVector<bankDetails*>* bankInfos = getBankInfos();

	char* bankInfoJson = strdup("[");
	for (uint32_t i = 0; i < bankInfos->count(); i++)
	{
		bankDetails* bank = bankInfos->get(i);
		if (bank->slots > 0)
		{
			char* temp = stringUtility::formatString("%s%s{\"id\":%i,\"name\":\"%s\",\"slots\":%i}", bankInfoJson, i == 0 ? "" : ",", bank->id, bank->name, bank->slots);
			free(bankInfoJson);
			bankInfoJson = temp;
		}
	}

	char* result = stringUtility::formatString("%s]", bankInfoJson);
	free(bankInfoJson);

	delete(bankInfos);
	return result;
}

void settingsManager::toggleAutoBootBank(uint8_t id)
{
	for (int i = 0; i < MAX_SLOTS; i++)
	{
		if (i == id)
		{
			mSettings.banks[i].autoBoot = !mSettings.banks[i].autoBoot;
		}
		else
		{
			mSettings.banks[i].autoBoot = false;
		}
	}
	saveSettings();
}

bool settingsManager::hasAutoBootBank()
{
	for (int i = 0; i < MAX_SLOTS; i++)
	{
		if (mSettings.banks[i].slots > 0 && mSettings.banks[i].autoBoot)
		{
			return true;
		}
	}
	return false;
}

void settingsManager::deleteBank(uint8_t id)
{
	memset(&mSettings.banks[id], 0, sizeof(bankInfo));
	saveSettings();
}

void settingsManager::optimizeBanks(uint8_t slotsNeeded)
{
	if (slotsNeeded == 1)
	{
		utils::debugPrint("No need to optimize for only 1 slot");
		return;
	}

	if (getFreeSlots() < slotsNeeded)
	{
		utils::debugPrint("Not enough free slots to be worth optimizing");
		return;
	}

	for (uint8_t i = 0; i < 3; i++)
	{
		uint8_t slotCheck = 1 << i;
		while (true)
		{
			// Exit if we now have space for what we need
			uint8_t bankId = 0;
			bool needsOptimize = tryGetFreeBank(slotsNeeded, bankId) == false;
			if (needsOptimize == false)
			{
				return;
			}

			// Work out first free bank and last used bank for current itteration of slots
			uint8_t freeBank = 0;
			if (tryGetFreeBank(slotCheck, freeBank) == false)
			{
				break;
			}
			uint8_t lastUsedBank = 0;
			if (tryGetLastUsedBank(slotCheck, lastUsedBank) == false)
			{
				break;
			}

			// Exit if no free slots before last used
			if (freeBank > lastUsedBank)
			{
				break;
			}

			// Move banks
			uint8_t destBank = context::getModchip()->getBankFromIdAndSlots(freeBank, slotCheck);
			uint8_t sourceBank = context::getModchip()->getBankFromIdAndSlots(lastUsedBank, slotCheck);
			utils::dataContainer* bankData = context::getModchip()->readBank(sourceBank);
			context::getModchip()->eraseBank(destBank);
			context::getModchip()->writeBank(destBank, bankData);
			delete(bankData);

			memcpy(&mSettings.banks[freeBank], &mSettings.banks[lastUsedBank], sizeof(bankInfo));
			memset(&mSettings.banks[lastUsedBank], 0, sizeof(bankInfo));
			saveSettings();

			utils::debugPrint("Moved bank %i to %i\n", lastUsedBank, freeBank);
		}
	}
}

bool settingsManager::tryGetLastUsedBank(uint8_t slotsNeeded, uint8_t& bankId)
{
	bool found = false;
	uint8_t lastBank = 0;

	uint8_t bank = 0;
	while (bank < context::getModchip()->getSlotCount())
	{
		uint8_t slotsUsed = mSettings.banks[bank].slots;
        if (slotsUsed == slotsNeeded)
        {
            found = true;
            lastBank = bank;
        }
        bank += slotsNeeded;
	}

	bankId = lastBank;
	return found;
}

bool settingsManager::tryGetFreeBank(uint8_t slotsNeeded, uint8_t& bankId)
{
	uint32_t freeSlots = getFreeSlots();

	if (freeSlots < slotsNeeded)
	{
		utils::debugPrint("Not enough free slots\n");
		return false;
	}

	uint8_t bank = 0;
	while (bank < context::getModchip()->getSlotCount())
	{
		uint8_t slotsToSkip = slotsNeeded;
		bool spaceFound = true;
		for (uint32_t i = 0; i < slotsNeeded; i++)
		{
			uint8_t slotsUsed = mSettings.banks[bank + i].slots;
			if (slotsUsed == 0)
			{
				continue;
			}
			slotsToSkip = max(slotsUsed, slotsNeeded);
			spaceFound = false;
			break;
		}
		if (spaceFound == true)
		{
			bankId = bank;
			return true;
		}
		bank += slotsToSkip;
	}

	return false;
}

void settingsManager::eraseBank(uint8_t bankId, uint32_t size)
{
	uint8_t slotsNeeded = (uint8_t)(size >> 18);
	uint8_t bank = context::getModchip()->getBankFromIdAndSlots(bankId, slotsNeeded);
	context::getModchip()->eraseBank(bank);
}

void settingsManager::writeBank(uint8_t bankId, utils::dataContainer* dataContainer, const char *name, uint8_t ledColor)
{
	uint8_t slotsNeeded = (uint8_t)(dataContainer->size >> 18);
	uint8_t bank = context::getModchip()->getBankFromIdAndSlots(bankId, slotsNeeded);
	context::getModchip()->writeBank(bank, dataContainer);
	mSettings.banks[bankId].ledColor = ledColor;
	mSettings.banks[bankId].slots = slotsNeeded;
	char* nameCopy = strdup(name);
	stringUtility::copyString((char*)&mSettings.banks[bankId].name[0], nameCopy, 40);
	free(nameCopy);
	saveSettings();
}

bool settingsManager::verifyBank(uint8_t bankId, utils::dataContainer* dataContainer)
{
	uint8_t slotsNeeded = (uint8_t)(dataContainer->size >> 18);
	uint8_t bank = context::getModchip()->getBankFromIdAndSlots(bankId, slotsNeeded);
	bool result = context::getModchip()->verifyBank(bank, dataContainer);
	return result;
}

utils::dataContainer* settingsManager::readBank(uint8_t bankId)
{
	uint8_t slots = mSettings.banks[bankId].slots;
	uint8_t bank = context::getModchip()->getBankFromIdAndSlots(bankId, slots);
	utils::dataContainer* result = context::getModchip()->readBank(bank);
	return result;
}

void settingsManager::launchBank(uint8_t bankId)
{
	lcdRender::waitStop();
	bankInfo bankInfo = settingsManager::getBankInfo(bankId);
	if (bankInfo.slots > 0)
	{
		uint8_t bank = context::getModchip()->getBankFromIdAndSlots(bankId, bankInfo.slots);
		context::getModchip()->launchBank(bank, bankInfo.ledColor);
	}
}

void settingsManager::launchTsop()
{
	lcdRender::waitStop();
	context::getModchip()->launchTsop();
}

void settingsManager::launchRecovery()
{
	lcdRender::waitStop();
	context::getModchip()->launchRecovery();
}

void settingsManager::editBank(uint8_t bankId, const char *name, uint8_t ledColor)
{
	mSettings.banks[bankId].ledColor = ledColor;
	char* nameCopy = strdup(name);
	stringUtility::copyString((char*)&mSettings.banks[bankId].name[0], nameCopy, 40);
	free(nameCopy);

	saveSettings();
}

networkModeEnum settingsManager::getNetworkMode()
{
	return (networkModeEnum)mSettings.network.networkMode;
}

uint32_t settingsManager::getNetworkAddress()
{
	return mSettings.network.address;
}

uint32_t settingsManager::getNetworkSubnet()
{
	return mSettings.network.subnet;
}

uint32_t settingsManager::getNetworkGateway()
{
	return mSettings.network.gateway;
}

uint32_t settingsManager::getNetworkPrimaryDns()
{
	return mSettings.network.primaryDns;
}

uint32_t settingsManager::getNetworkSecondaryDns()
{
	return mSettings.network.secondaryDns;
}

void settingsManager::setNetwork(networkModeEnum networkMode, uint32_t address, uint32_t subnet, uint32_t gateway, uint32_t primaryDns, uint32_t secondaryDns)
{
	mSettings.network.networkMode = (uint32_t)networkMode;
	mSettings.network.address = address;
	mSettings.network.address = address;
	mSettings.network.subnet = subnet;
	mSettings.network.gateway = gateway;
	mSettings.network.primaryDns = primaryDns;
	mSettings.network.secondaryDns = secondaryDns;
	saveSettings();
}

char* settingsManager::getSkinName()
{
	return strdup(&mSettings.skinName[0]);
}

void settingsManager::setSkinName(const char* skinName)
{
	memset(&mSettings.skinName[0], 0, sizeof(mSettings.skinName));
	strncpy(&mSettings.skinName[0], skinName, 50);
	saveSettings();
}

char* settingsManager::getSoundPackName()
{
	return strdup(&mSettings.soundPackName[0]);
}

void settingsManager::setSoundPackName(const char* soundPackName)
{
	memset(&mSettings.soundPackName[0], 0, sizeof(mSettings.soundPackName));
	strncpy(&mSettings.soundPackName[0], soundPackName, 50);
	saveSettings();
}

uint32_t settingsManager::getSnakeHiScore()
{
	return mSettings.snakeHiScore;
}

void settingsManager::setSnakeHiScore(uint32_t playerScore)
{
	mSettings.snakeHiScore = playerScore;
	saveSettings();
}

uint32_t settingsManager::getInvadersHiScore()
{
	return mSettings.invadersHiScore;
}

void settingsManager::setInvadersHiScore(uint32_t playerScore)
{
	mSettings.invadersHiScore = playerScore;
	saveSettings();
}

uint8_t settingsManager::getAutoBootDelay()
{
	return mSettings.autoBootDelay;
}

void settingsManager::setAutoBootDelay(uint8_t autoBootDelay)
{
	mSettings.autoBootDelay = autoBootDelay;
	saveSettings();
}

uint8_t settingsManager::getMusicVolume()
{
	return mSettings.musicVolume;
}

void settingsManager::setMusicVolume(uint8_t volume)
{
	mSettings.musicVolume = volume;
	saveSettings();
}

uint8_t settingsManager::getSoundVolume()
{
	return mSettings.soundVolume;
}

void settingsManager::setSoundVolume(uint8_t volume)
{
	mSettings.soundVolume = volume;
	saveSettings();
}

uint8_t settingsManager::getMinFanSpeed()
{
	return mSettings.minFanSpeed;
}

void settingsManager::setMinFanSpeed(uint8_t minFanSpeed)
{
	mSettings.minFanSpeed = minFanSpeed;
	saveSettings();
}

uint8_t settingsManager::getLedColor()
{
	return mSettings.ledColor;
}

void settingsManager::setLedColor(uint8_t ledColor)
{
	mSettings.ledColor = ledColor;
	saveSettings();
}

uint8_t settingsManager::getLcdEnableType()
{
	return mSettings.lcdEnableType;
}

void settingsManager::setLcdEnableType(uint8_t lcdEnableType)
{
	mSettings.lcdEnableType = lcdEnableType;
	saveSettings();
}

uint8_t settingsManager::getLcdBacklight()
{
	return mSettings.lcdBacklight;
}

void settingsManager::setLcdBacklight(uint8_t backlight)
{
	mSettings.lcdBacklight = backlight;
	saveSettings();
}

uint8_t settingsManager::getLcdContrast()
{
	return mSettings.lcdContrast;
}

void settingsManager::setLcdContrast(uint8_t contrast)
{
	mSettings.lcdContrast = contrast;
	saveSettings();
}

bool settingsManager::getRtcEnable()
{
	return mSettings.rtcEnable == 1;
}

void settingsManager::setRtcEnable(bool enable)
{
	mSettings.rtcEnable = enable ? 1 : 0;
	saveSettings();
}

uint8_t settingsManager::getDriveSetup()
{
	return mSettings.driveSetup;
}

void settingsManager::setDriveSetup(uint8_t driveSetup)
{
	mSettings.driveSetup = driveSetup;
	saveSettings();
}

uint8_t settingsManager::getUdmaMode(bool master)
{
	return master ? mSettings.udmaModeMaster : mSettings.udmaModeSlave;
}

void settingsManager::setUdmaMode(uint8_t udmaMode, bool master)
{
	if(master) {
		mSettings.udmaModeMaster = udmaMode;
	} else {
		mSettings.udmaModeSlave = udmaMode;
	}
	saveSettings();
}

uint8_t settingsManager::getSplashDelay()
{
	return mSettings.splashDelay;
}

void settingsManager::setSplashDelay(uint8_t splashDelay)
{
	mSettings.splashDelay = splashDelay;
	saveSettings();
}

bool settingsManager::getVgaEnable()
{
	return mSettings.vgaEnable == 1;
}

void settingsManager::setVgaEnable(bool enable)
{
	mSettings.vgaEnable = enable ? 1 : 0;
	saveSettings();
}