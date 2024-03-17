#pragma once

#include "xboxinternals.h"
#include "versioning.h"
#include "utils.h"
#include "pointerVector.h"

#include <string>

#pragma pack(push,1)

typedef struct bankInfo 
{
	uint8_t ledColor;
	uint8_t slots;
	char name[41];
	bool autoBoot;
} bankInfo;

enum networkModeEnum
{ 
	networkModeDynamicAutoDns = 0,
	networkModeDynamicManualDns = 1,
	networkModeStatic = 2,
}; 

typedef struct networkInfo 
{
	networkModeEnum networkMode;
	uint32_t address;
	uint32_t subnet;
	uint32_t gateway;
	uint32_t primaryDns;
	uint32_t secondaryDns;
} networkInfo;

typedef struct settingsState 
{
	uint32_t checksum;
    semver version;
	bankInfo banks[4];
	networkInfo network;
	char skinName[51];
	char soundPackName[51];
	uint32_t playerHiScore;
	uint8_t autoBootDelay;
	uint8_t musicVolume;
	uint8_t soundVolume;
	uint8_t minFanSpeed;
	uint8_t ledColor;
	bool lcdEnabled;
	uint8_t lcdBacklight;
	uint8_t lcdContrast;
} settingsState;

#pragma pack(pop)

typedef struct bankDetails
{
	uint8_t id;
	uint8_t ledColor;
	uint8_t slots;
	char* name;
	bool autoBoot;

	bankDetails() : id(0), ledColor(0), slots(0), name(NULL), autoBoot(false) {}

	~bankDetails()
	{
		free(this->name);
	}

} bankDetails;

class settingsManager
{
public:
	static semver getVersion();
	static char* getVersionSting(semver version);
	static void initSettings();
	static void loadSettings();
	static void saveSettings();
	static utils::dataContainer* getInstallerLogoData();
	static settingsState* getSettings();
	static uint32_t getFreeSlots();
	static char* getFreeSlotsJson();
	static bankInfo getBankInfo(uint8_t id);
	static pointerVector* getBankInfos();
	static char* getBankInfosJson();
	static void toggleAutoBootBank(uint8_t id);
	static bool hasAutoBootBank();
	static void deleteBank(uint8_t id);
	static void optimizeBanks(uint8_t slotsNeeded);
	static bool tryGetFreeBank(uint8_t slotsNeeded, uint8_t& bankId);
	static void eraseBank(uint8_t bankId, uint32_t size);
	static void writeBank(uint8_t bankId, utils::dataContainer* dataContainer, const char *name, uint8_t ledColor);
	static bool verifyBank(uint8_t bankId, utils::dataContainer* dataContainer);
	static utils::dataContainer* readBank(uint8_t bankId);
	static void launchBank(uint8_t id);
	static void launchTsop();
	static void launchRecovery();
	static void editBank(uint8_t bankId, const char *name, uint8_t ledColor);
	static networkModeEnum getNetworkMode();
	static uint32_t getNetworkAddress();
	static uint32_t getNetworkSubnet();
	static uint32_t getNetworkPrimaryDns();
	static uint32_t getNetworkSecondaryDns();
	static uint32_t getNetworkGateway();
	static void setNetwork(networkModeEnum networkMode, uint32_t address, uint32_t subnet, uint32_t gateway, uint32_t primaryDns, uint32_t secondaryDns);
	static char* getSkinName();
	static void setSkinName(const char* skinName);
	static char* getSoundPackName();
	static void setSoundPackName(const char* soundPackName);
	static uint32_t getPlayerHiScore();
	static void setPlayerHiScore(uint32_t playerScore);
	static uint8_t getAutoBootDelay();
	static void setAutoBootDelay(uint8_t autoBootDelay);
	static uint8_t getMusicVolume();
	static void setMusicVolume(uint8_t volume);
	static uint8_t getSoundVolume();
	static void setSoundVolume(uint8_t volume);
	static uint8_t getMinFanSpeed();
	static void setMinFanSpeed(uint8_t minFanSpeed);
	static uint8_t getLedColor();
	static void setLedColor(uint8_t ledColor);
	static bool getLcdEnabled();
	static void setLcdEnabled(bool enabled);
	static uint8_t getLcdBacklight();
	static void setLcdBacklight(uint8_t backlight);
	static uint8_t getLcdContrast();
	static void setLcdContrast(uint8_t contrast);
};
