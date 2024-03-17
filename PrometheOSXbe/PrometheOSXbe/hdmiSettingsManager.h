#pragma once

#include "xboxinternals.h"
#include "versioning.h"

#pragma pack(push,1)
typedef struct hdmiSettingsState 
{
    semver firmwareVersion;
    semver softwareVersion; 
    uint32_t checksum;
    uint8_t outputResolution;
    uint8_t widescreen;
    uint8_t colorspace;
    uint8_t interpolation;
    uint8_t interpolationScaleX;
    uint8_t interpolationScaleY;
    uint8_t interpolationWeightX;
    uint8_t interpolationWeightY;
    uint8_t autoVideoBlank;
    uint8_t autoRegionSwitch;
    uint8_t prescaleFixDisable;
    uint8_t disable480p169;
    uint8_t aviInfoOverride;
    uint8_t pictureAspectRatio;
    uint8_t activeFormatAspectRatio;
    uint8_t activeFormatStatus;
    uint8_t barInfoPresent;
    uint16_t barInfoEtb;
    uint16_t barInfoEbb;
    uint16_t barInfoElb;
    uint16_t barInfoSrb;
    uint8_t pixelClockDelayOverride;
    uint8_t pixelClockDelay;
    uint8_t pixelClockDelayNeg;
	uint8_t padding[212];
} hdmiSettingsState;
#pragma pack(pop)

class hdmiSettingsManager
{
public:
	static semver getSoftwareVersion();
	static semver getFirmwareVersion();
	static char* getVersionSting(semver version);
	static void initSettings();
	static void loadSettings();
	static void saveSettings();
	static hdmiSettingsState* getSettings();
};
