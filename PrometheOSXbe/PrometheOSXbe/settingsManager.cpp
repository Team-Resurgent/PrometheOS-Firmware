#include "settingsManager.h"
#include "versioning.h"
#include "xenium.h"
#include "crc32.h"
#include "stringUtility.h"
#include "utils.h"

#include <stdio.h>

//64k sector sizes
//0x1c0000-0x1dffff = recovery (128k)
//0x1e0000-0x1effff = unused (64k)

//8k sector sizes begin
//0x1f0000-0x1f1fff = settings (8k)
//0x1f2000-0x1f7fff = unused (24k)
//0x1f8000-0x1fffff = logo (32k)

#define SETTINGS_OFFSET (0x1f0000 - 0x1c0000)
#define INSTALLER_LOGO_OFFSET (0x1f0000 - 0x1f8000)

namespace 
{
	settingsState mSettings;
}

semver settingsManager::getVersion()
{
	semver version;
	memset(&version, 0, sizeof(version));
	version.major = 1;
	version.minor = 0;
	version.patch = 1;
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
	mSettings.ledColor = 2;

#ifdef TEST
	mSettings.banks[0].slots = 1;
	stringUtility::copyString(&mSettings.banks[0].name[0], "Bank 1 Rom", 26);
	mSettings.banks[1].slots = 1;
	stringUtility::copyString(&mSettings.banks[1].name[0], "Bank 2 Rom", 26);
	mSettings.banks[2].slots = 1;
	stringUtility::copyString(&mSettings.banks[2].name[0], "Bank 3 Rom", 26);
	mSettings.banks[3].slots = 1;
	stringUtility::copyString(&mSettings.banks[3].name[0], "Bank 4 Rom", 26);
#endif
}

void settingsManager::loadSettings()
{
#ifndef TEST
	utils::dataContainer* settingsData = xenium::readBank(xenium::bankRecovery, SETTINGS_OFFSET, sizeof(mSettings));
	memcpy(&mSettings, settingsData->data, sizeof(mSettings));
	uint32_t checksum = crc32::calculate(settingsData->data + 1, sizeof(mSettings) - 1);
	free(settingsData);
	if (checksum != mSettings.checksum || versioning::compareVersion(mSettings.version, getVersion()) != 0)
	{
		initSettings();
		saveSettings();
	}
	xenium::setLedColor((xenium::ledColorEnum)mSettings.ledColor);
#else
	initSettings();
#endif
}

void settingsManager::saveSettings()
{
	mSettings.checksum = crc32::calculate(((char*)&mSettings) + 1, sizeof(mSettings) - 1);
	utils::dataContainer* dataContainer = new utils::dataContainer((char*)&mSettings, sizeof(mSettings), sizeof(mSettings));
	xenium::eraseBank(xenium::bankRecovery, SETTINGS_OFFSET, dataContainer->size);
	xenium::writeBank(xenium::bankRecovery, SETTINGS_OFFSET, dataContainer);
	if (xenium::verifyBank(xenium::bankRecovery, SETTINGS_OFFSET, dataContainer) == false)
	{
		utils::debugPrint("Bank verify failed.\n");
	}
	delete(dataContainer);
	xenium::setLedColor((xenium::ledColorEnum)mSettings.ledColor);
}

utils::dataContainer* settingsManager::getInstallerLogoData()
{
	utils::dataContainer* installerLogoData = xenium::readBank(xenium::bankRecovery, INSTALLER_LOGO_OFFSET, 32768);
	return installerLogoData;
}

settingsState* settingsManager::getSettings()
{
	return &mSettings;
}

uint32_t settingsManager::getFreeSlots()
{
	uint32_t freeSlots = 4;
	for (uint32_t i = 0; i < 4; i++)
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
	bool needsOptimize = false;
	if (slotsNeeded == 2)
	{
		needsOptimize = !((mSettings.banks[0].slots == 0 && mSettings.banks[1].slots == 0) || (mSettings.banks[2].slots == 0 && mSettings.banks[3].slots == 0));
	}

	if (needsOptimize == false)
	{
		return;
	}

	int8_t destBankId = 0;
	while (destBankId < 3)
	{
		if (mSettings.banks[destBankId].slots != 0)
		{
			destBankId += mSettings.banks[destBankId].slots;
			continue;
		}
		int8_t sourceBankId = destBankId + 1;
		while (sourceBankId < 4)
		{
			if (mSettings.banks[sourceBankId].slots != 1)
			{
				sourceBankId = sourceBankId + max(mSettings.banks[sourceBankId].slots, 1);
				continue;
			}
			xenium::bankEnum destBank = xenium::getBankFromIdAndSlots(destBankId, 1);
			xenium::bankEnum sourceBank = xenium::getBankFromIdAndSlots(sourceBankId, 1);
			if (destBank == xenium::bankInvalid || sourceBank == xenium::bankInvalid)
			{
				continue;
			}
			utils::dataContainer* bankData = xenium::readBank(sourceBank, 0, xenium::getBankSize(sourceBank));
			xenium::eraseBank(destBank, 0, bankData->size);
			xenium::writeBank(destBank, 0, bankData);
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
			bankId = bank;;
			return true;
		}
		bank += slotsNeeded;
	}

	return false;
}

void settingsManager::eraseBank(uint8_t bankId, uint32_t size)
{
	xenium::setLedColor(xenium::ledColorAmber);

	uint8_t slotsNeeded = (uint8_t)(size >> 18);
	xenium::bankEnum bank = xenium::getBankFromIdAndSlots(bankId, slotsNeeded);
	xenium::eraseBank(bank, 0, size);

	xenium::setLedColor((xenium::ledColorEnum)mSettings.ledColor);
}

void settingsManager::writeBank(uint8_t bankId, utils::dataContainer* dataContainer, const char *name, uint8_t ledColor)
{
	xenium::setLedColor(xenium::ledColorBlue);

	uint8_t slotsNeeded = (uint8_t)(dataContainer->size >> 18);
	xenium::bankEnum bank = xenium::getBankFromIdAndSlots(bankId, slotsNeeded);
	xenium::writeBank(bank, 0, dataContainer);

	mSettings.banks[bankId].ledColor = ledColor;
	mSettings.banks[bankId].slots = slotsNeeded;
	char* nameCopy = strdup(name);
	stringUtility::copyString((char*)&mSettings.banks[bankId].name[0], nameCopy, 40);
	free(nameCopy);

	xenium::setLedColor((xenium::ledColorEnum)mSettings.ledColor);
	saveSettings();
}

bool settingsManager::verifyBank(uint8_t bankId, utils::dataContainer* dataContainer)
{
	xenium::setLedColor(xenium::ledColorPurple);

	uint8_t slotsNeeded = (uint8_t)(dataContainer->size >> 18);
	xenium::bankEnum bank = xenium::getBankFromIdAndSlots(bankId, slotsNeeded);
	bool result = xenium::verifyBank(bank, 0, dataContainer);

	xenium::setLedColor((xenium::ledColorEnum)mSettings.ledColor);
	return result;
}

utils::dataContainer* settingsManager::readBank(uint8_t bankId)
{
	xenium::setLedColor(xenium::ledColorWhite);

	uint8_t slots = mSettings.banks[bankId].slots;
	xenium::bankEnum readBank = xenium::getBankFromIdAndSlots(bankId, slots);
	uint32_t size = xenium::getBankSize(readBank);
	utils::dataContainer* result = xenium::readBank(readBank, 0, size);

	xenium::setLedColor((xenium::ledColorEnum)mSettings.ledColor);
	return result;
}

void settingsManager::launchBank(uint8_t bankId)
{
	bankInfo bank = settingsManager::getBankInfo(bankId);
	if (bank.slots > 0)
	{
		xenium::launchBank(bankId, bank.slots, (xenium::ledColorEnum)bank.ledColor);
	}
}

void settingsManager::launchTsop()
{
	xenium::launchTsop();
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
	strncpy(&mSettings.skinName[0], skinName, 49);
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

uint8_t settingsManager::getLedColor()
{
	return mSettings.ledColor;
}

void settingsManager::setLedColor(uint8_t ledColor)
{
	mSettings.ledColor = ledColor;
	saveSettings();
}