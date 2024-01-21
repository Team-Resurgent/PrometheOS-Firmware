#include "drive.h"
#include <string>
#include "stringUtility.h"
#include "XboxInternals.h"
#include "fileSystem.h"

drive::drive(const char* mountPoint, const char* systemPath, bool shouldRemount)
{
	mMountPoint = strdup(mountPoint);
	mSystemPath = strdup(systemPath);
	mMounted = false;
	mShouldRemount = shouldRemount;
}

bool drive::mount()
{
	if (mShouldRemount == true && mMounted == true) {
		mMounted = false;
		unmount();
	}

	if (mMounted == true) 
	{
		return true;
	}

	char* mountPoint = stringUtility::formatString("\\??\\%s:", mMountPoint);
	char* systemPath = stringUtility::formatString("%s", mSystemPath);
	STRING sMountPoint = {(USHORT)strlen(mountPoint), (USHORT)strlen(mountPoint) + 1, mountPoint};
	STRING sSystemPath = {(USHORT)strlen(systemPath), (USHORT)strlen(systemPath) + 1, systemPath};
	int result = IoCreateSymbolicLink(&sMountPoint, &sSystemPath);
	free(mountPoint);
	free(systemPath);

	if (stringUtility::equals(mMountPoint, "D", true) == true)
	{
		uint32_t trayState;
		if (HalReadSMCTrayState(&trayState, NULL) >= 0 && trayState == SMC_TRAY_STATE_MEDIA_DETECT)
		{
			mMounted = true;
		}
	}
	else
	{
		mMounted = getTotalNumberOfBytes() != 0;
	}

	return mMounted;
}

bool drive::unmount()
{
	char* mountPoint = stringUtility::formatString("\\??\\%s:", mMountPoint);
	STRING sMountPoint = {(USHORT)strlen(mountPoint), (USHORT)strlen(mountPoint) + 1, mountPoint};
	int result = IoDeleteSymbolicLink(&sMountPoint);
	free(mountPoint);

	char* systemPath = stringUtility::formatString("%s", mSystemPath);
	char* systemPath2 = stringUtility::rightTrim(systemPath, '\\');
	if (stringUtility::equals(mMountPoint, "D", true) == true)
	{
		STRING sSystemPath = {(USHORT)strlen(systemPath2), (USHORT)strlen(systemPath2) + 1, systemPath2};
		result |= IoDismountVolumeByName(&sSystemPath);
	}
	free(systemPath2);
	free(systemPath);

	mMounted = false;
	return result == 0;
}

bool drive::isMounted()
{	
	return mMounted;
}

char* drive::getMountPoint()
{
	return strdup(mMountPoint);
}

char* drive::getSystemPath()
{
	return strdup(mSystemPath);
}

uint64_t drive::getTotalNumberOfBytes()
{	
	char* rootPath = stringUtility::formatString("%s:\\", mMountPoint);
	ULARGE_INTEGER totalNumberOfBytes;
	BOOL status = GetDiskFreeSpaceExA(rootPath, NULL, &totalNumberOfBytes, NULL);
	free(rootPath);
	if (status == 0) 
	{
		return 0;
	}
	return (uint64_t)totalNumberOfBytes.QuadPart;
}

uint64_t drive::getTotalFreeNumberOfBytes()
{	
	char* rootPath = stringUtility::formatString("%s:\\", mMountPoint);
	ULARGE_INTEGER totalNumberOfFreeBytes = { 0 };
	BOOL status = GetDiskFreeSpaceExA(rootPath, &totalNumberOfFreeBytes, NULL, NULL);
	free(rootPath);
	if (status == 0) 
	{
		return 0;
	}
	return (uint64_t)totalNumberOfFreeBytes.QuadPart;
}

