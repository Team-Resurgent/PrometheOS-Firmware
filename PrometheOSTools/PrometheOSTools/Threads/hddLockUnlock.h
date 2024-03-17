#pragma once

#include "..\xboxInternals.h"

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
		hddLockUnlockResponseUnlockedVsc = 2,
		hddLockUnlockResponseFailedToUnlock = 3,
		hddLockUnlockResponseAlreadyUnlocked = 4,
		hddLockUnlockResponseLocked = 5,
		hddLockUnlockResponseFailedToLock = 6,
		hddLockUnlockResponseAlreadyLocked = 7,
		hddLockUnlockResponseLockingNotSupported = 8,
		hddLockUnlockResponseFailureEeprom = 9,
		hddLockUnlockResponseUnlocking = 10,
		hddLockUnlockResponseUnlockingVsc = 11,
		hddLockUnlockResponseLocking = 12
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
	static void lockHdd();
	static void unlockHdd();
	static void setResponse(hddLockUnlock::hddLockUnlockResponse response);
};
