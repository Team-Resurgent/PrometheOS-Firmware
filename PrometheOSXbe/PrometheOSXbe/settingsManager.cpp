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
	version.minor = 3;
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
	context::getModchip()->loadSettings(mSettings);
}

void settingsManager::saveSettings()
{
	context::getModchip()->saveSettings(mSettings);
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

pointerVector* settingsManager::getBankInfos()
{
	pointerVector* bankInfos = new pointerVector(true);

	for (uint32_t i = 0; i < 4; i++)
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
		bank->autoBoot = mSettings.banks[i].autoBoot;
		bankInfos->add(bank);
	}

	return bankInfos;
}

char* settingsManager::getBankInfosJson()
{
	pointerVector* bankInfos = getBankInfos();

	char* bankInfoJson = strdup("[");
	for (uint32_t i = 0; i < bankInfos->count(); i++)
	{
		bankDetails* bank = (bankDetails*)bankInfos->get(i);
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
	for (int i = 0; i < 4; i++)
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
	for (int i = 0; i < 4; i++)
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
	uint32_t slotCount = context::getModchip()->getSlotCount();
	if (slotCount < 2)
	{
		utils::debugPrint("Not enough slots to be worth optimizing");
		return;
	}

	if (getFreeSlots() < slotsNeeded)
	{
		utils::debugPrint("Not enough free slots to be worth optimizing");
		return;
	}

	bool needsOptimize = false;
	if (slotCount == 4 && slotsNeeded == 2)
	{
		needsOptimize = !((mSettings.banks[0].slots == 0 && mSettings.banks[1].slots == 0) || (mSettings.banks[2].slots == 0 && mSettings.banks[3].slots == 0));
	}

	if (needsOptimize == false)
	{
		return;
	}

	int8_t destBankId = 0;
	while (destBankId < (int8_t)(slotCount - 1))
	{
		if (mSettings.banks[destBankId].slots != 0)
		{
			destBankId += mSettings.banks[destBankId].slots;
			continue;
		}
		int8_t sourceBankId = destBankId + 1;
		while (sourceBankId < (int8_t)slotCount)
		{
			if (mSettings.banks[sourceBankId].slots != 1)
			{
				sourceBankId = sourceBankId + max(mSettings.banks[sourceBankId].slots, 1);
				continue;
			}
			uint8_t destBank = context::getModchip()->getBankFromIdAndSlots(destBankId, 1);
			uint8_t sourceBank = context::getModchip()->getBankFromIdAndSlots(sourceBankId, 1);
			utils::dataContainer* bankData = context::getModchip()->readBank(sourceBank);
			context::getModchip()->eraseBank(destBank);
			context::getModchip()->writeBank(destBank, bankData);
			delete(bankData);

			memcpy(&mSettings.banks[destBankId], &mSettings.banks[sourceBankId], sizeof(bankInfo));
			memset(&mSettings.banks[sourceBankId], 0, sizeof(bankInfo));
			saveSettings();

			utils::debugPrint("Moved bank %i to %i\n", sourceBankId, destBankId);
			break;
		}
		destBankId++;
	}
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
	while (bank < 4)
	{
		bool spaceFound = true;
		for (uint32_t i = 0; i < slotsNeeded; i++)
		{
			if (mSettings.banks[i + bank].slots == 0)
			{
				continue;
			}
			spaceFound = false;
		}
		if (spaceFound == true)
		{
			bankId = bank;
			return true;
		}
		bank += slotsNeeded;
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
	return mSettings.network.networkMode;
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
	mSettings.network.networkMode = networkMode;
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

uint32_t settingsManager::getPlayerHiScore()
{
	return mSettings.playerHiScore;
}

void settingsManager::setPlayerHiScore(uint32_t playerScore)
{
	mSettings.playerHiScore = playerScore;
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

bool settingsManager::getLcdEnabled()
{
	return mSettings.lcdEnabled;
}

void settingsManager::setLcdEnabled(bool enabled)
{
	mSettings.lcdEnabled = enabled;
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