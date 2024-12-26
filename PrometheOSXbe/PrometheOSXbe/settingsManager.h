#pragma once

#include "xboxinternals.h"
#include "versioning.h"
#include "utils.h"
#include "pointerVector.h"

#include <string>

#define MAX_SLOTS 16

#pragma pack(push,1)

typedef struct bankInfo 
{
	uint8_t ledColor;
	uint8_t slots;
	uint8_t autoBoot;
	uint8_t reserved0;
	char name[64];
} bankInfo;

enum networkModeEnum
{ 
	networkModeDynamicAutoDns = 0,
	networkModeDynamicManualDns = 1,
	networkModeStatic = 2,
}; 

typedef struct networkInfo 
{
	uint32_t networkMode;
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
	bankInfo banks[MAX_SLOTS];
	networkInfo network;
	uint32_t snakeHiScore;
	uint32_t invadersHiScore;
	char skinName[64];
	char soundPackName[64];
	uint8_t autoBootDelay;
	uint8_t musicVolume;
	uint8_t soundVolume;
	uint8_t minFanSpeed;
	uint8_t ledColor;
	uint8_t lcdMode;
	uint8_t lcdModel;
	uint8_t lcdAddress;
	uint8_t lcdBacklight;
	uint8_t lcdContrast;
	uint8_t rtcEnable;
	uint8_t driveSetup;
	uint8_t udmaModeMaster;
	uint8_t udmaModeSlave;
	uint8_t splashDelay;
	uint8_t vgaEnable;
	uint8_t reserved0;
	uint8_t reserved1;
	uint8_t reserved2;
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
	static bool shouldSave();
	static utils::dataContainer* getInstallerLogoData();
	static uint32_t getFreeSlots();
	static char* getFreeSlotsJson();
	static bankInfo getBankInfo(uint8_t id);
	static pointerVector<bankDetails*>* getBankInfos();
	static char* getBankInfosJson();
	static void toggleAutoBootBank(uint8_t id);
	static bool hasAutoBootBank();
	static void deleteBank(uint8_t id);
	static void optimizeBanks(uint8_t slotsNeeded);
	static bool tryGetLastUsedBank(uint8_t slotsNeeded, uint8_t& bankId);
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
	static uint32_t getSnakeHiScore();
	static void setSnakeHiScore(uint32_t playerScore);
	static uint32_t getInvadersHiScore();
	static void setInvadersHiScore(uint32_t playerScore);
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
	static uint8_t getLcdMode(bool current);
	static void setLcdMode(uint8_t lcdMode);
	static uint8_t getLcdModel(bool current);
	static void setLcdModel(uint8_t lcdModel);
	static uint8_t getLcdAddress(bool current);
	static void setLcdAddress(uint8_t lcdAddress);
	static uint8_t getLcdBacklight(bool current);
	static void setLcdBacklight(uint8_t backlight);
	static uint8_t getLcdContrast(bool current);
	static void setLcdContrast(uint8_t contrast);
	static bool getRtcEnable();
	static void setRtcEnable(bool enable);
	static uint8_t getDriveSetup();
	static void setDriveSetup(uint8_t driveSetup);
	static uint8_t getUdmaMode(bool master = true);
	static void setUdmaMode(uint8_t udmaMode, bool master = true);
	static uint8_t getSplashDelay();
	static void setSplashDelay(uint8_t splashDelay);
	static bool getVgaEnable();
	static void setVgaEnable(bool enable);
};
