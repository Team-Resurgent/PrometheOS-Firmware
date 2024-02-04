#include "hddLockUnlock.h"

#include "..\xboxInternals.h"
#include "..\XKUtils\XKEEPROM.h"
#include "..\XKUtils\XKHDD.h"

namespace
{
	HANDLE mThread;
	hddLockUnlock::hddLockUnlockData mData;
}

bool hddLockUnlock::startThread(hddLockUnlockAction action)
{
	memset(&mData, 0, sizeof(hddLockUnlockData));
	mData.action = action;
	  
	InitializeCriticalSection(&mData.mutex);

	mThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)process, (void*)&mData, 0, NULL);
	if (mThread == NULL) {
        return false;
    }
	SetThreadPriority(mThread, THREAD_PRIORITY_HIGHEST);
	return true;
}

bool hddLockUnlock::completed()
{
	DWORD exitCode;
    if (GetExitCodeThread(mThread, &exitCode)) 
	{
        if (exitCode != STILL_ACTIVE) 
		{
            return true;
        }
    }
	return false;
}

hddLockUnlock::hddLockUnlockResponse hddLockUnlock::getResponse()
{
	hddLockUnlockResponse response;
	EnterCriticalSection(&mData.mutex);
	response = mData.response;
	LeaveCriticalSection(&mData.mutex);
	return response;
}

void hddLockUnlock::closeThread()
{
	CloseHandle(mThread);
    DeleteCriticalSection(&mData.mutex);
}

uint64_t WINAPI hddLockUnlock::process(void* param) 
{
	hddLockUnlockData* data = (hddLockUnlockData*)param;

	if (data->action == hddLockUnlockActionLock)
	{
		hddLockUnlockResponse response = lockHdd();
		EnterCriticalSection(&data->mutex);
		data->response = response;
		LeaveCriticalSection(&data->mutex);
	}
	else
	{
		hddLockUnlockResponse response = unlockHdd();
		EnterCriticalSection(&data->mutex);
		data->response = response;
		LeaveCriticalSection(&data->mutex);
	}

	return 0;
}

hddLockUnlock::hddLockUnlockResponse hddLockUnlock::lockHdd()
{
	hddLockUnlockResponse response = hddLockUnlockResponseNone;

	XKEEPROM* eeprom = new XKEEPROM();
	eeprom->ReadFromXBOX();

	XKHDD::ATA_COMMAND_OBJ hddcommand;
	UCHAR HddPass[32];
	UCHAR MasterPassword[13] = "TEAMASSEMBLY";
	
	XBOX_VERSION xver = eeprom->GetXBOXVersion();

	if (eeprom->Decrypt())
	{
		ZeroMemory(&hddcommand, sizeof(XKHDD::ATA_COMMAND_OBJ));
		hddcommand.DATA_BUFFSIZE = 0;
		hddcommand.IPReg.bDriveHeadReg = IDE_DEVICE_MASTER;
		hddcommand.IPReg.bCommandReg = IDE_ATA_IDENTIFY;
		XKHDD::SendATACommand(IDE_PRIMARY_PORT, &hddcommand, IDE_COMMAND_READ);

		XKEEPROM::EEPROMDATA tmpData;
		eeprom->GetEEPROMData(&tmpData);
		XKHDD::GenerateHDDPwd(tmpData.HDDKkey, hddcommand.DATA_BUFFER, HddPass);

		DWORD SecStatus = XKHDD::GetIDESecurityStatus(hddcommand.DATA_BUFFER);
		if ((SecStatus & IDE_SECURITY_SUPPORTED) && !(SecStatus & IDE_SECURITY_ENABLED))
		{
			ZeroMemory(&hddcommand, sizeof(XKHDD::ATA_COMMAND_OBJ));
			hddcommand.DATA_BUFFSIZE = 512;
			hddcommand.IPReg.bDriveHeadReg = IDE_DEVICE_MASTER;
			hddcommand.IPReg.bCommandReg = IDE_ATA_SECURITY_SETPASSWORD;
			LPBYTE HDDP = (LPBYTE)&hddcommand.DATA_BUFFER;
			LPDWORD pMastr = (LPDWORD) HDDP;
			*pMastr = 0x0001;
			memcpy(HDDP+2, MasterPassword, 13);
			XKHDD::SendATACommand(IDE_PRIMARY_PORT, &hddcommand, IDE_COMMAND_WRITE);

			ZeroMemory(&hddcommand, sizeof(XKHDD::ATA_COMMAND_OBJ));
			hddcommand.DATA_BUFFSIZE = 512;
			hddcommand.IPReg.bDriveHeadReg = IDE_DEVICE_MASTER;
			hddcommand.IPReg.bCommandReg = IDE_ATA_IDENTIFY;
			XKHDD::SendATACommand(IDE_PRIMARY_PORT, &hddcommand, IDE_COMMAND_READ);
			SecStatus = XKHDD::GetIDESecurityStatus(hddcommand.DATA_BUFFER);
			
			ZeroMemory(&hddcommand, sizeof(XKHDD::ATA_COMMAND_OBJ));
			hddcommand.DATA_BUFFSIZE = 512;
			hddcommand.IPReg.bDriveHeadReg = IDE_DEVICE_MASTER;
			hddcommand.IPReg.bCommandReg = IDE_ATA_SECURITY_SETPASSWORD;
			memcpy(HDDP+2, HddPass, 20);
			XKHDD::SendATACommand(IDE_PRIMARY_PORT, &hddcommand, IDE_COMMAND_WRITE);

			int retrycnt = 0;
			do
			{
				ZeroMemory(&hddcommand, sizeof(XKHDD::ATA_COMMAND_OBJ));
				hddcommand.DATA_BUFFSIZE = 512;
				hddcommand.IPReg.bDriveHeadReg = IDE_DEVICE_MASTER;
				hddcommand.IPReg.bCommandReg = IDE_ATA_IDENTIFY;
				XKHDD::SendATACommand(IDE_PRIMARY_PORT, &hddcommand, IDE_COMMAND_READ);
				SecStatus = XKHDD::GetIDESecurityStatus(hddcommand.DATA_BUFFER);
				retrycnt++;
			} while (!((SecStatus & IDE_SECURITY_SUPPORTED) && (SecStatus & IDE_SECURITY_ENABLED)) && (retrycnt < 3));

			if ((SecStatus & IDE_SECURITY_SUPPORTED) && (SecStatus & IDE_SECURITY_ENABLED))
			{
				response = hddLockUnlockResponseLocked;
			}
			else
			{
				response = hddLockUnlockResponseFailedToLock;
			}
		}
		else if (!(SecStatus & IDE_SECURITY_SUPPORTED))
		{
			response = hddLockUnlockResponseLockingNotSupported;
		}
		else
		{
			response = hddLockUnlockResponseAlreadyLocked;
		}
	}
	else
	{
		response = hddLockUnlockResponseFailureEeprom;
	}

	delete(eeprom);
	return response;
}

hddLockUnlock::hddLockUnlockResponse hddLockUnlock::unlockHdd()
{
	hddLockUnlockResponse response = hddLockUnlockResponseNone;

	XKEEPROM* eeprom = new XKEEPROM();
	eeprom->ReadFromXBOX();

	XBOX_VERSION xver = eeprom->GetXBOXVersion();

	XKHDD::ATA_COMMAND_OBJ hddcommand;
	UCHAR HddPass[32];

	if (eeprom->Decrypt())
	{
		ZeroMemory(&hddcommand, sizeof(XKHDD::ATA_COMMAND_OBJ));
		hddcommand.DATA_BUFFSIZE = 0;
		hddcommand.IPReg.bDriveHeadReg = IDE_DEVICE_MASTER;
		hddcommand.IPReg.bCommandReg = IDE_ATA_IDENTIFY;
		XKHDD::SendATACommand(IDE_PRIMARY_PORT, &hddcommand, IDE_COMMAND_READ);

		XKEEPROM::EEPROMDATA tmpData;
		eeprom->GetEEPROMData(&tmpData);
		XKHDD::GenerateHDDPwd(tmpData.HDDKkey, hddcommand.DATA_BUFFER, HddPass);
		
		DWORD SecStatus = XKHDD::GetIDESecurityStatus(hddcommand.DATA_BUFFER);
		if ((SecStatus & IDE_SECURITY_SUPPORTED) && (SecStatus & IDE_SECURITY_ENABLED))
		{
			ZeroMemory(&hddcommand, sizeof(XKHDD::ATA_COMMAND_OBJ));
			hddcommand.DATA_BUFFSIZE = 512;
			hddcommand.IPReg.bDriveHeadReg = IDE_DEVICE_MASTER;
			hddcommand.IPReg.bCommandReg = IDE_ATA_SECURITY_UNLOCK;
			LPBYTE HDDP = (LPBYTE)&hddcommand.DATA_BUFFER;
			memcpy(HDDP+2, HddPass, 20);
			XKHDD::SendATACommand(IDE_PRIMARY_PORT, &hddcommand, IDE_COMMAND_WRITE);

			ZeroMemory(&hddcommand, sizeof(XKHDD::ATA_COMMAND_OBJ));
			hddcommand.DATA_BUFFSIZE = 512;
			hddcommand.IPReg.bDriveHeadReg = IDE_DEVICE_MASTER;
			hddcommand.IPReg.bCommandReg = IDE_ATA_IDENTIFY;
			XKHDD::SendATACommand(IDE_PRIMARY_PORT, &hddcommand, IDE_COMMAND_READ);

			ZeroMemory(&hddcommand, sizeof(XKHDD::ATA_COMMAND_OBJ));
			hddcommand.DATA_BUFFSIZE = 512;
			hddcommand.IPReg.bDriveHeadReg = IDE_DEVICE_MASTER;
			hddcommand.IPReg.bCommandReg = IDE_ATA_SECURITY_DISABLE;
			memcpy(HDDP+2, HddPass, 20);
			XKHDD::SendATACommand(IDE_PRIMARY_PORT, &hddcommand, IDE_COMMAND_WRITE);

			int retrycnt = 0;
			do
			{
				ZeroMemory(&hddcommand, sizeof(XKHDD::ATA_COMMAND_OBJ));
				hddcommand.DATA_BUFFSIZE = 512;
				hddcommand.IPReg.bDriveHeadReg = IDE_DEVICE_MASTER;
				hddcommand.IPReg.bCommandReg = IDE_ATA_IDENTIFY;
				XKHDD::SendATACommand(IDE_PRIMARY_PORT, &hddcommand, IDE_COMMAND_READ);
				SecStatus = XKHDD::GetIDESecurityStatus(hddcommand.DATA_BUFFER);
				retrycnt++;
			} while (!((SecStatus & IDE_SECURITY_SUPPORTED) && !(SecStatus & IDE_SECURITY_ENABLED)) && (retrycnt < 3));

			if ((SecStatus & IDE_SECURITY_SUPPORTED) && !(SecStatus & IDE_SECURITY_ENABLED))
			{
				response = hddLockUnlockResponseUnlocked;
			}
			else
			{
				response = hddLockUnlockResponseFailedToUnlock;
			}
		}
		else
		{
			response = hddLockUnlockResponseAlreadyUnlocked;
		}
	}
	else
	{
		response = hddLockUnlockResponseFailureEeprom;
	}

	delete(eeprom);
	return response;
}