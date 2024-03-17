#pragma once

#include "Drive.h"
#include "pointerVector.h"
#include "xboxinternals.h"

#include <string>

class driveManager
{
public:
	static bool getTotalNumberOfBytes(const char* mountPoint, uint64_t& totalSize);
	static bool getTotalFreeNumberOfBytes(const char* mountPoint, uint64_t& totalFree);
	static pointerVector* getMountedDrives();
	static bool ftpPathMounted(const char* path);
	static char* mapFtpPath(const char* path);
	static void init();
	static bool mountDrive(const char* driveLetter);
	static void mountAllDrives();
	static bool isAllMounted();
};