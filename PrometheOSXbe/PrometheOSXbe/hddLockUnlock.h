#pragma once

#include "xboxInternals.h"

class hddLockUnlock
{
public:

	enum hddLockUnlockAction
	{ 
		hddLockUnlockActionLock = 0,
		hddLockUnlockActionUnlock = 1
	}; 
	
	enum hddLockUnlockResponse
	{ 
		hddLockUnlockResponseNone = 0,
		hddLockUnlockResponseUnlocked = 1,
		hddLockUnlockResponseFailedToUnlock = 2,
		hddLockUnlockResponseAlreadyUnlocked = 3,
		hddLockUnlockResponseLocked = 4,
		hddLockUnlockResponseFailedToLock = 5,
		hddLockUnlockResponseAlreadyLocked = 6,
		hddLockUnlockResponseLockingNotSupported = 7,
		hddLockUnlockResponseFailureEeprom = 8
	}; 

	typedef struct hddLockUnlockData {
		hddLockUnlockAction action;
		hddLockUnlockResponse response;
		CRITICAL_SECTION mutex;
	} hddLockUnlockData;

	static bool startThread(hddLockUnlockAction action);
	static bool completed();
	static hddLockUnlockResponse getResponse();
	static void closeThread();

private:

	static uint64_t WINAPI process(void* param);
	static hddLockUnlockResponse lockHdd();
	static hddLockUnlockResponse unlockHdd();
};
