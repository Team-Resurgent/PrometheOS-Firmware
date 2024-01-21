#pragma once

#include "xboxinternals.h"
#include <string>
#include <xtl.h>
#include <winsockx.h>

class drive
{
public:
	drive(const char* mountPoint, const char* systemPath, bool shouldRemount);
	bool mount();  		
	bool unmount();
	bool isMounted();
	char* getMountPoint();
	char* getSystemPath();
	uint64_t getTotalNumberOfBytes();
	uint64_t getTotalFreeNumberOfBytes();
protected:
	char* mMountPoint;
	char* mSystemPath;
	bool mMounted;
	bool mShouldRemount;
};