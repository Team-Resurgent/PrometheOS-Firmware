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
	if (fileSystem::fileOpen("HDD0-C:\\ElpisHD\\settings.bin", fileSystem::FileModeRead, fileHandle))
	{
		uint32_t fileSize;
		if (fileSystem::fileSize(fileHandle, fileSize))
		{
			if (fileSize != sizeof(mSettings))
			{
				initSettings();
			}
			else
			{
				uint32_t bytesRead;
				if (fileSystem::fileRead(fileHandle, (char*)&mSettings, fileSize, bytesRead) && bytesRead == fileSize)
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
			}
		}
		fileSystem::fileClose(fileHandle);
	}
}

void hdmiSettingsManager::saveSettings()
{
	if (fileSystem::directoryCreate("HDD0-C:\\ElpisHD") == true)
	{
		uint32_t bytesWritten;
		fileSystem::fileWrite("HDD0-C:\\ElpisHD\\settings.bin", (char*)&mSettings, sizeof(mSettings), bytesWritten);
	}
}

hdmiSettingsState* hdmiSettingsManager::getSettings()
{
	return &mSettings;
}