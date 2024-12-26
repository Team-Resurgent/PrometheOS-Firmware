#include "hddInfo.h"

#include "..\xboxInternals.h"
#include "..\stringUtility.h"
#include "..\XKUtils\XKEEPROM.h"
#include "..\XKUtils\XKHDD.h"

namespace
{
	HANDLE mThread;
	bool mHarddrive;
	hddInfo::hddInfoData mData;
}

bool hddInfo::startThread(bool harddrive)
{
	memset(&mData, 0, sizeof(hddInfoData));
	  
	InitializeCriticalSection(&mData.mutex);
	mHarddrive = harddrive;

	mThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)process, (void*)&mData, 0, NULL);
	if (mThread == NULL) {
        return false;
    }
	SetThreadPriority(mThread, THREAD_PRIORITY_HIGHEST);
	return true;
}

bool hddInfo::completed()
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

hddInfo::hddInfoResponse hddInfo::getResponse()
{
	hddInfoResponse response;
	EnterCriticalSection(&mData.mutex);
	response = mData.response;
	LeaveCriticalSection(&mData.mutex);
	return response;
}

char* hddInfo::getModel()
{
	char* model;
	EnterCriticalSection(&mData.mutex);
	model = mData.model == NULL ? strdup("") : strdup(mData.model);
	LeaveCriticalSection(&mData.mutex);
	return model;
}

char* hddInfo::getSerial()
{
	char* serial;
	EnterCriticalSection(&mData.mutex);
	serial =  mData.serial == NULL ? strdup("") : strdup(mData.serial);
	LeaveCriticalSection(&mData.mutex);
	return serial;
}

void hddInfo::closeThread()
{
	CloseHandle(mThread);
    DeleteCriticalSection(&mData.mutex);
	free(mData.model);
	free(mData.serial);
}

uint64_t WINAPI hddInfo::process(void* param) 
{
	Sleep(100);
	hddInfoData* data = (hddInfoData*)param;

	hddInfoResponse response = hddInfoResponseNone;
	char* model = NULL;
	char* serial = NULL;

	XKEEPROM* eeprom = new XKEEPROM();
	eeprom->ReadFromXBOX();
	if (eeprom->Decrypt())
	{
		XKHDD::ATA_COMMAND_OBJ hddcommand;
		ZeroMemory(&hddcommand, sizeof(XKHDD::ATA_COMMAND_OBJ));
		hddcommand.DATA_BUFFSIZE = 512;
		hddcommand.IPReg.bDriveHeadReg = mHarddrive == 0 ? IDE_DEVICE_MASTER : IDE_DEVICE_SLAVE;
		hddcommand.IPReg.bCommandReg = IDE_ATA_IDENTIFY;
		bool resp = !!XKHDD::SendATACommand(IDE_PRIMARY_PORT, &hddcommand, IDE_COMMAND_READ);

		if(resp) {
			char* ideModel = XKHDD::GetIDEModel(hddcommand.DATA_BUFFER);
			model = stringUtility::trim(ideModel, ' ');
			free(ideModel);
			char* ideSerial = XKHDD::GetIDESerial(hddcommand.DATA_BUFFER);
			serial = stringUtility::trim(ideSerial, ' ');
			free(ideSerial);
			DWORD SecStatus = XKHDD::GetIDESecurityStatus(hddcommand.DATA_BUFFER);
			response = (SecStatus & IDE_SECURITY_SUPPORTED) && (SecStatus & IDE_SECURITY_ENABLED) ? hddInfoResponseLocked : hddInfoResponseUnlocked;
		}
		else
		{
			response = hddInfoResponseTimeout;
		}
	}
	else 
	{
		response = hddInfoResponseFailureEeprom;
		model = strdup("Unknown");
		serial = strdup("Unknown");
	}
	delete(eeprom);

	EnterCriticalSection(&data->mutex);
	data->response = response;
	data->model = model;
	data->serial = serial;
	LeaveCriticalSection(&data->mutex);

	return 0;
}
