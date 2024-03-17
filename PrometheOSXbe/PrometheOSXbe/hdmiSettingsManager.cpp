#include "hdmiSettingsManager.h"
#include "versioning.h"
#include "fileSystem.h"
#include "stringUtility.h"

namespace 
{
	hdmiSettingsState mSettings;
}

semver hdmiSettingsManager::getSoftwareVersion()
{
	semver appVersion;
	memset(&appVersion, 0, sizeof(appVersion));
	appVersion.major = 2;
	appVersion.minor = 2;
	appVersion.patch = 2;
	return appVersion;
}

semver hdmiSettingsManager::getFirmwareVersion()
{
	semver appVersion;
	memset(&appVersion, 0, sizeof(appVersion));
	appVersion.major = 2;
	appVersion.minor = 2;
	appVersion.patch = 2;
	return appVersion;
}

char* hdmiSettingsManager::getVersionSting(semver version)
{
	return stringUtility::formatString("%i.%i.%i", version.major, version.minor, version.patch);
}

void hdmiSettingsManager::initSettings()
{
	memset(&mSettings, 0, sizeof(mSettings));
	mSettings.firmwareVersion = getSoftwareVersion();
    mSettings.softwareVersion = getFirmwareVersion();
	mSettings.autoVideoBlank = 1;
    mSettings.autoRegionSwitch = 1;
}

void hdmiSettingsManager::loadSettings()
{
	initSettings();

	uint32_t fileHandle;
	if (fileSystem::fileOpen("C:\\ElpisHD\\settings.bin", fileSystem::FileModeRead, fileHandle))
	{
		uint32_t fileSize;
		if (fileSystem::fileSize(fileHandle, fileSize))
		{
			char* buffer = (char*)malloc(fileSize);
			uint32_t bytesRead;
			if (fileSystem::fileRead(fileHandle, buffer, fileSize, bytesRead) && bytesRead == fileSize)
			{
				if (versioning::compareVersion(mSettings.softwareVersion, getSoftwareVersion()) != 0)
				{
					initSettings();
				}
				if (versioning::compareVersion(mSettings.firmwareVersion, getFirmwareVersion()) != 0)
				{
					initSettings();
				}
			}
			else
			{
				initSettings();
			}
			free(buffer);
		}
		fileSystem::fileClose(fileHandle);
	}
}

void hdmiSettingsManager::saveSettings()
{
	if (fileSystem::directoryCreate("C:\\ElpisHD") == true)
	{
		uint32_t bytesWritten;
		fileSystem::fileWrite("C:\\ElpisHD\\settings.bin", (char*)&mSettings, sizeof(mSettings), bytesWritten);
	}
}

hdmiSettingsState* hdmiSettingsManager::getSettings()
{
	return &mSettings;
}