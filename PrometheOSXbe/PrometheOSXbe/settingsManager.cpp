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
	settingsState mSettingsCurrent;
	settingsState mSettingsShadow;
}

semver settingsManager::getVersion()
{
	semver version;
	memset(&version, 0, sizeof(version));
	version.major = 1;
	version.minor = 5;
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
	memset(&mSettingsCurrent, 0, sizeof(mSettingsCurrent));
	mSettingsCurrent.version = getVersion();
	mSettingsCurrent.autoBootDelay = 5;
	mSettingsCurrent.musicVolume = 75;
	mSettingsCurrent.soundVolume = 100;
	mSettingsCurrent.minFanSpeed = 10;
	mSettingsCurrent.ledColor = 2;
	mSettingsCurrent.lcdBacklight = 100;
	mSettingsCurrent.lcdContrast = 80;
	mSettingsCurrent.driveSetup = 1;
	mSettingsCurrent.udmaModeMaster = 2;
	mSettingsCurrent.udmaModeSlave = 2;
	mSettingsCurrent.splashDelay = 6;
//#ifdef TEST
//	mSettingsCurrent.banks[0].slots = 1;
//	stringUtility::copyString(&mSettingsCurrent.banks[0].name[0], "Bank 1 Rom", 41);
//	mSettingsCurrent.banks[1].slots = 1;
//	stringUtility::copyString(&mSettingsCurrent.banks[1].name[0], "Bank 2 Rom", 41);
//	mSettingsCurrent.banks[2].slots = 1;
//	stringUtility::copyString(&mSettingsCurrent.banks[2].name[0], "Bank 3 Rom", 41);
//	mSettingsCurrent.banks[3].slots = 1;
//	stringUtility::copyString(&mSettingsCurrent.banks[3].name[0], "Bank 4 Rom", 41);
//
//	stringUtility::copyString(&mSettingsCurrent.skinName[0], "TeamResurgent-Animated", 51);
//	stringUtility::copyString(&mSettingsCurrent.soundPackName[0], "Cybernoid", 51);
//
//#endif
	memcpy(&mSettingsShadow, &mSettingsCurrent, sizeof(mSettingsCurrent));
}

void settingsManager::loadSettings()
{
#ifndef TOOLS
	context::getModchip()->loadSettings(mSettingsCurrent);
	memcpy(&mSettingsShadow, &mSettingsCurrent, sizeof(mSettingsCurrent));
#else
	initSettings();
#endif
}

void settingsManager::saveSettings()
{
#ifndef TOOLS
	context::getModchip()->saveSettings(mSettingsShadow);
	memcpy(&mSettingsCurrent, &mSettingsShadow, sizeof(mSettingsCurrent));
#endif
}

bool settingsManager::shouldSave()
{
	uint8_t* first = ((uint8_t*)&mSettingsCurrent) + sizeof(uint32_t);
	uint8_t* second = ((uint8_t*)&mSettingsShadow) + sizeof(uint32_t);
	for (int i = 0; i < sizeof(mSettingsCurrent) - sizeof(uint32_t); i++)
	{
		if (first[i] != second[i])
		{
			return true;
		}
	}
	return false;
}

utils::dataContainer* settingsManager::getInstallerLogoData()
{
	return context::getModchip()->getInstallerLogo();
}

uint32_t settingsManager::getFreeSlots()
{
	uint32_t slotCount = context::getModchip()->getSlotCount();
	uint32_t freeSlots = slotCount;
	for (uint32_t i = 0; i < slotCount; i++)
	{
		freeSlots = freeSlots - mSettingsCurrent.banks[i].slots;
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
	return mSettingsCurrent.banks[id];
}

pointerVector<bankDetails*>* settingsManager::getBankInfos()
{
	pointerVector<bankDetails*>* bankInfos = new pointerVector<bankDetails*>(true);

	uint32_t totalSlots = context::getModchip()->getSlotCount();
	for (uint32_t i = 0; i < totalSlots; i++)
	{
		if (mSettingsCurrent.banks[i].slots == 0)
		{
			continue;
		}

		bankDetails* bank = new bankDetails();
		bank->id = (uint8_t)i;
		bank->ledColor = mSettingsCurrent.banks[i].ledColor;
		bank->slots = mSettingsCurrent.banks[i].slots;
		bank->name = strdup(&mSettingsCurrent.banks[i].name[0]);
		bank->autoBoot = mSettingsCurrent.banks[i].autoBoot == 1;
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
			mSettingsShadow.banks[i].autoBoot = !mSettingsShadow.banks[i].autoBoot;
		}
		else
		{
			mSettingsShadow.banks[i].autoBoot = false;
		}
	}
	saveSettings();
}

bool settingsManager::hasAutoBootBank()
{
	for (int i = 0; i < MAX_SLOTS; i++)
	{
		if (mSettingsCurrent.banks[i].slots > 0 && mSettingsCurrent.banks[i].autoBoot)
		{
			return true;
		}
	}
	return false;
}

void settingsManager::deleteBank(uint8_t id)
{
	memset(&mSettingsShadow.banks[id], 0, sizeof(bankInfo));
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

			memcpy(&mSettingsShadow.banks[freeBank], &mSettingsShadow.banks[lastUsedBank], sizeof(bankInfo));
			memset(&mSettingsShadow.banks[lastUsedBank], 0, sizeof(bankInfo));

			utils::debugPrint("Moved bank %i to %i\n", lastUsedBank, freeBank);
		}
	}
	saveSettings();
}

bool settingsManager::tryGetLastUsedBank(uint8_t slotsNeeded, uint8_t& bankId)
{
	bool found = false;
	uint8_t lastBank = 0;

	uint8_t bank = 0;
	while (bank < context::getModchip()->getSlotCount())
	{
		uint8_t slotsUsed = mSettingsCurrent.banks[bank].slots;
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
			uint8_t slotsUsed = mSettingsCurrent.banks[bank + i].slots;
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
	mSettingsShadow.banks[bankId].ledColor = ledColor;
	mSettingsShadow.banks[bankId].slots = slotsNeeded;
	char* nameCopy = strdup(name);
	stringUtility::copyString((char*)&mSettingsShadow.banks[bankId].name[0], nameCopy, 40);
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
	uint8_t slots = mSettingsCurrent.banks[bankId].slots;
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
	mSettingsShadow.banks[bankId].ledColor = ledColor;
	char* nameCopy = strdup(name);
	stringUtility::copyString((char*)&mSettingsShadow.banks[bankId].name[0], nameCopy, 40);
	free(nameCopy);

	saveSettings();
}

networkModeEnum settingsManager::getNetworkMode()
{
	return (networkModeEnum)mSettingsCurrent.network.networkMode;
}

uint32_t settingsManager::getNetworkAddress()
{
	return mSettingsCurrent.network.address;
}

uint32_t settingsManager::getNetworkSubnet()
{
	return mSettingsCurrent.network.subnet;
}

uint32_t settingsManager::getNetworkGateway()
{
	return mSettingsCurrent.network.gateway;
}

uint32_t settingsManager::getNetworkPrimaryDns()
{
	return mSettingsCurrent.network.primaryDns;
}

uint32_t settingsManager::getNetworkSecondaryDns()
{
	return mSettingsCurrent.network.secondaryDns;
}

void settingsManager::setNetwork(networkModeEnum networkMode, uint32_t address, uint32_t subnet, uint32_t gateway, uint32_t primaryDns, uint32_t secondaryDns)
{
	mSettingsShadow.network.networkMode = (uint32_t)networkMode;
	mSettingsShadow.network.address = address;
	mSettingsShadow.network.address = address;
	mSettingsShadow.network.subnet = subnet;
	mSettingsShadow.network.gateway = gateway;
	mSettingsShadow.network.primaryDns = primaryDns;
	mSettingsShadow.network.secondaryDns = secondaryDns;
	saveSettings();
}

char* settingsManager::getSkinName()
{
	return strdup(&mSettingsCurrent.skinName[0]);
}

void settingsManager::setSkinName(const char* skinName)
{
	memset(&mSettingsShadow.skinName[0], 0, sizeof(mSettingsShadow.skinName));
	strncpy(&mSettingsShadow.skinName[0], skinName, 50);
	saveSettings();
}

char* settingsManager::getSoundPackName()
{
	return strdup(&mSettingsCurrent.soundPackName[0]);
}

void settingsManager::setSoundPackName(const char* soundPackName)
{
	memset(&mSettingsShadow.soundPackName[0], 0, sizeof(mSettingsShadow.soundPackName));
	strncpy(&mSettingsShadow.soundPackName[0], soundPackName, 50);
	saveSettings();
}

uint32_t settingsManager::getSnakeHiScore()
{
	return mSettingsCurrent.snakeHiScore;
}

void settingsManager::setSnakeHiScore(uint32_t playerScore)
{
	mSettingsShadow.snakeHiScore = playerScore;
	saveSettings();
}

uint32_t settingsManager::getInvadersHiScore()
{
	return mSettingsCurrent.invadersHiScore;
}

void settingsManager::setInvadersHiScore(uint32_t playerScore)
{
	mSettingsShadow.invadersHiScore = playerScore;
	saveSettings();
}

uint8_t settingsManager::getAutoBootDelay()
{
	return mSettingsCurrent.autoBootDelay;
}

void settingsManager::setAutoBootDelay(uint8_t autoBootDelay)
{
	mSettingsShadow.autoBootDelay = autoBootDelay;
	saveSettings();
}

uint8_t settingsManager::getMusicVolume()
{
	return mSettingsCurrent.musicVolume;
}

void settingsManager::setMusicVolume(uint8_t volume)
{
	mSettingsShadow.musicVolume = volume;
	saveSettings();
}

uint8_t settingsManager::getSoundVolume()
{
	return mSettingsCurrent.soundVolume;
}

void settingsManager::setSoundVolume(uint8_t volume)
{
	mSettingsShadow.soundVolume = volume;
	saveSettings();
}

uint8_t settingsManager::getMinFanSpeed()
{
	return mSettingsCurrent.minFanSpeed;
}

void settingsManager::setMinFanSpeed(uint8_t minFanSpeed)
{
	mSettingsShadow.minFanSpeed = minFanSpeed;
	saveSettings();
}

uint8_t settingsManager::getLedColor()
{
	return mSettingsCurrent.ledColor;
}

void settingsManager::setLedColor(uint8_t ledColor)
{
	mSettingsShadow.ledColor = ledColor;
	saveSettings();
}

uint8_t settingsManager::getLcdMode(bool current)
{
	return current ? mSettingsCurrent.lcdMode : mSettingsShadow.lcdMode;
}

void settingsManager::setLcdMode(uint8_t lcdMode)
{
	mSettingsShadow.lcdMode = lcdMode;
}

uint8_t settingsManager::getLcdModel(bool current)
{
	return current ? mSettingsCurrent.lcdModel : mSettingsShadow.lcdModel;
}

void settingsManager::setLcdModel(uint8_t lcdModel)
{
	mSettingsShadow.lcdModel = lcdModel;
}

uint8_t settingsManager::getLcdAddress(bool current)
{
	return current ? mSettingsCurrent.lcdAddress : mSettingsShadow.lcdAddress;
}

void settingsManager::setLcdAddress(uint8_t lcdAddress)
{
	mSettingsShadow.lcdAddress = lcdAddress;
}

uint8_t settingsManager::getLcdBacklight(bool current)
{
	return current ? mSettingsCurrent.lcdBacklight : mSettingsShadow.lcdBacklight;
}

void settingsManager::setLcdBacklight(uint8_t backlight)
{
	mSettingsShadow.lcdBacklight = backlight;
}

uint8_t settingsManager::getLcdContrast(bool current)
{
	return current ? mSettingsCurrent.lcdContrast : mSettingsShadow.lcdContrast;
}

void settingsManager::setLcdContrast(uint8_t contrast)
{
	mSettingsShadow.lcdContrast = contrast;
}

bool settingsManager::getRtcEnable()
{
	return mSettingsCurrent.rtcEnable == 1;
}

void settingsManager::setRtcEnable(bool enable)
{
	mSettingsShadow.rtcEnable = enable ? 1 : 0;
	saveSettings();
}

uint8_t settingsManager::getDriveSetup()
{
	return mSettingsCurrent.driveSetup;
}

void settingsManager::setDriveSetup(uint8_t driveSetup)
{
	mSettingsShadow.driveSetup = driveSetup;
	saveSettings();
}

uint8_t settingsManager::getUdmaMode(bool master)
{
	return master ? mSettingsCurrent.udmaModeMaster : mSettingsCurrent.udmaModeSlave;
}

void settingsManager::setUdmaMode(uint8_t udmaMode, bool master)
{
	if(master) {
		mSettingsShadow.udmaModeMaster = udmaMode;
	} else {
		mSettingsShadow.udmaModeSlave = udmaMode;
	}
	saveSettings();
}

uint8_t settingsManager::getSplashDelay()
{
	return mSettingsCurrent.splashDelay;
}

void settingsManager::setSplashDelay(uint8_t splashDelay)
{
	mSettingsShadow.splashDelay = splashDelay;
	saveSettings();
}

bool settingsManager::getVgaEnable()
{
	return mSettingsCurrent.vgaEnable == 1;
}

void settingsManager::setVgaEnable(bool enable)
{
	mSettingsShadow.vgaEnable = enable ? 1 : 0;
	saveSettings();
}