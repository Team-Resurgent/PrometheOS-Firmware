#include "driveManager.h"
#include "pointerVector.h"
#include "fileSystem.h"
#include "stringUtility.h"
#include "Drive.h"

namespace
{
	bool mInitialized;
	bool mAllMounted;
	pointerVector* m_drives = NULL;
}

bool driveManager::getTotalNumberOfBytes(const char* mountPoint, uint64_t& totalSize)
{
	driveManager::mountDrive(mountPoint);

	for (size_t i = 0; i < m_drives->count(); i++) 
	{
		drive* currentDrive = (drive*)m_drives->get(i);
		if (strcmp(currentDrive->getMountPoint(), mountPoint) == 0) 
		{
			totalSize = (uint64_t)currentDrive->getTotalNumberOfBytes();
			return true;
		}
	}
	return false;
}

bool driveManager::getTotalFreeNumberOfBytes(const char* mountPoint, uint64_t& totalFree)
{
	driveManager::mountDrive(mountPoint);

	for (size_t i = 0; i < m_drives->count(); i++) 
	{
		drive* currentDrive = (drive*)m_drives->get(i);
		char* currentMountPoint = currentDrive->getMountPoint();
		if (strcmp(currentMountPoint, mountPoint) == 0) 
		{
			totalFree = (uint64_t)currentDrive->getTotalFreeNumberOfBytes();
			free(currentMountPoint);
			return true;
		}
		free(currentMountPoint);
	}
	return false;
}

pointerVector* driveManager::getMountedDrives()
{
	driveManager::mountAllDrives();

	pointerVector *drives = new pointerVector(false);
	for (uint32_t i = 0; i < m_drives->count(); i++)
	{
		drive* currentDrive = (drive*)m_drives->get(i);
		if (currentDrive->isMounted()) 
		{
			drives->add(currentDrive->getMountPoint());
		}
	}
	return drives;
}

char* driveManager::mapFtpPath(const char* path)
{
	char* tempPath = stringUtility::leftTrim(path, '/');
	for (size_t i = 0; i < m_drives->count(); i++) 
	{
		drive* currentDrive = (drive*)m_drives->get(i);
		char* mountPoint = currentDrive->getMountPoint();
		if (stringUtility::startsWith(tempPath, mountPoint, true)) 
		{
			currentDrive->mount();
			char* localFolder = stringUtility::substr(tempPath, strlen(mountPoint), -1);
			char* localPath = stringUtility::formatString("%s:%s", mountPoint, localFolder);
			char* result = stringUtility::replace(localPath, "/", "\\");
			free(localFolder);
			free(localPath);
			free(tempPath);
			free(mountPoint);
			return result;
		}
		free(mountPoint);
	}
	free(tempPath);
	return strdup(path);
}

bool driveManager::ftpPathMounted(const char* path)
{
	char* tempPath = stringUtility::leftTrim(path, '/');
	for (size_t i = 0; i < m_drives->count(); i++) 
	{
		drive* currentDrive = (drive*)m_drives->get(i);
		char* mountPoint = currentDrive->getMountPoint();
		if (stringUtility::startsWith(tempPath, mountPoint, true))
		{
			currentDrive->mount();
			free(tempPath);
			free(mountPoint);
			return currentDrive->isMounted() == true;
		}
		free(mountPoint);
	}
	free(tempPath);
	return false;
}

void driveManager::init()
{
	if (mInitialized == false)
	{
		m_drives = new pointerVector(true);

		m_drives->add(new drive("C", "\\Device\\Harddisk0\\Partition2\\", driveTypeHardDrive));
		m_drives->add(new drive("D", "\\Device\\Cdrom0\\", driveTypeCdRom));
		m_drives->add(new drive("E", "\\Device\\Harddisk0\\Partition1\\", driveTypeHardDrive));
		m_drives->add(new drive("F", "\\Device\\Harddisk0\\Partition6\\", driveTypeHardDrive));
		m_drives->add(new drive("G", "\\Device\\Harddisk0\\Partition7\\", driveTypeHardDrive));
		m_drives->add(new drive("X", "\\Device\\Harddisk0\\Partition3\\", driveTypeHardDrive));
		m_drives->add(new drive("Y", "\\Device\\Harddisk0\\Partition4\\", driveTypeHardDrive));
		m_drives->add(new drive("Z", "\\Device\\Harddisk0\\Partition5\\", driveTypeHardDrive));
		m_drives->add(new drive("R", "\\Device\\Harddisk0\\Partition8\\", driveTypeHardDrive));
		m_drives->add(new drive("S", "\\Device\\Harddisk0\\Partition9\\", driveTypeHardDrive));
		m_drives->add(new drive("V", "\\Device\\Harddisk0\\Partition10\\", driveTypeHardDrive));
		m_drives->add(new drive("W", "\\Device\\Harddisk0\\Partition11\\", driveTypeHardDrive));
		m_drives->add(new drive("A", "\\Device\\Harddisk0\\Partition12\\", driveTypeHardDrive));
		m_drives->add(new drive("B", "\\Device\\Harddisk0\\Partition13\\", driveTypeHardDrive));

		//https://github.com/brentdc-nz/xbmc4xbox/blob/13cf4fbab8d70b154941a6b91e101bd05cc5b111/xbmc/utils/MemoryUnitManager.cpp#L77

		m_drives->add(new drive("H", "", driveTypeMemoryUnit));
		m_drives->add(new drive("I", "", driveTypeMemoryUnit));
		m_drives->add(new drive("J", "", driveTypeMemoryUnit));
		m_drives->add(new drive("K", "", driveTypeMemoryUnit));
		m_drives->add(new drive("L", "", driveTypeMemoryUnit));
		m_drives->add(new drive("M", "", driveTypeMemoryUnit));
		m_drives->add(new drive("N", "", driveTypeMemoryUnit));
		m_drives->add(new drive("O", "", driveTypeMemoryUnit));

		mInitialized = true;
	}
}

bool driveManager::mountDrive(const char* driveLetter)
{
	init();

	for (size_t i = 0; i < m_drives->count(); i++)
	{
		drive* currentDrive = (drive*)m_drives->get(i);
		char* mountPoint = currentDrive->getMountPoint();

		if (stringUtility::equals(mountPoint, driveLetter, true) == true)
		{
			free(mountPoint);
			return currentDrive->mount();
		}
		free(mountPoint);
	}
	return false;
}

void driveManager::mountAllDrives()
{
	init();

	for (size_t i = 0; i < m_drives->count(); i++)
	{
		drive* currentDrive = (drive*)m_drives->get(i);
		currentDrive->mount();
	}
	mAllMounted = true;
}

bool driveManager::isAllMounted()
{
	return mAllMounted;
}