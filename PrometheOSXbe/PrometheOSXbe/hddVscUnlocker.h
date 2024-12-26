#pragma once

#include "xboxinternals.h"

class hddVscUnlocker {
public:

	enum IDE_PORT {
		IDE_PORT_PRIMARY   = 0x1f0,
		IDE_PORT_SECONDARY = 0x170
	};

	enum IDE_DRIVE {
		IDE_DRIVE_MASTER = 0,
		IDE_DRIVE_SLAVE  = 1
	};

	enum ATA_SEC {
		ATA_SEC_PWD_SIZE = 32
	};

	enum WD_INFO {
		WD_INFO_MASTER_PWD_OFFSET = 0x398,
		WD_INFO_USER_PWD_OFFSET   = 0x3b8
	};

	struct IDE_CMD {
		uint8_t feat;
		uint8_t nSect;
		uint8_t sLow;
		uint8_t sMed;
		uint8_t sHigh;
		uint8_t drv;
		uint8_t cmd;
	};

	struct IDE_REG {
		uint8_t err;
		uint8_t nSect;
		uint8_t sLow;
		uint8_t sMed;
		uint8_t sHigh;
		uint8_t drv;
		uint8_t status;
	};

	struct DRIVE_INFO {
		char* model;
		char* serial;
		char* firmware;
	};

	struct IDENT_SECTOR {           // Words
		uint16_t res0[10];            // 0-9
		char serial[20];              // 10-19
		uint16_t res10[3];            // 20-22
		char firmware[8];             // 23-26
		char model[40];               // 27-46
		uint16_t res20[13];           // 47-59
		uint32_t nSect28;             // 60-61
		uint16_t res30[21];           // 62-82
		uint16_t pad1:10;             // 83:0-9
		uint16_t isLba48:1;           // 83:10
		uint16_t pad10:5;             // 83:11-15
		uint16_t res40[16];           // 84-99
		uint64_t nSect48;             // 100-103
		uint16_t res50[24];           // 104-127
		uint16_t secSupported:1;      // 128:0
		uint16_t secEnabled:1;        // 128:1
		uint16_t secLocked:1;         // 128:2
		uint16_t secFrozen:1;         // 128:3
		uint16_t pad20:12;            // 83:4-15
		uint16_t res60[127];          // 129-255
	};

	hddVscUnlocker();
	~hddVscUnlocker();

	NTSTATUS getLastError();         // Returns the last NTSTATUS if an error occured from call to attemptVscUnlock()
	void setIdePort(IDE_PORT port);  // Primary/Secondary port select
	void setIdeDrive(IDE_DRIVE drv); // Master/Slave drive select
	bool attemptVscUnlock(bool checkOnly = false); // Primary unlock func
	bool checkVscUnlock();           // Same as attemptVscUnlock() but just returns if model is supported
	bool getMasterPwd(uint8_t* buf); // OUT buf, sets master password, returns true if available
	bool getUserPwd(uint8_t* buf);   // OUT buf, sets user password, returns true if available
	DRIVE_INFO getDriveInfo();       // Returns model/serial/fw info

protected:
	IDE_REG readDriveReg(bool altPort = false);
	bool sendDriveCmd(IDE_CMD cmd, bool altPort = false, uint32_t timeout = 2000);
	void sendDriveRawCmd(IDE_CMD cmd);
	bool readDriveData(uint8_t* buf, uint32_t size);
	bool writeDriveData(uint8_t* buf, uint32_t size);
	bool waitForDrive(bool altPort = true, uint32_t timeout = 2000);
	uint8_t getDrvRegBits(uint8_t drvReg);
	bool identifyDrive();
	void freeIdentifyData();
	void parseIdentifySector();
	bool isDriveLocked(bool doId = true);
	bool sendIdentifyCmd();
	bool resetIdeBus();
	void resetIdeBusRaw();
	bool doAtaSecUnlock();
	bool spinWait(bool altPort = false, uint32_t timeout = 10000);
	void debug(char* str);
	void debugArr(uint32_t val);
	bool sendSecUnlockDisableCmd(bool disable = true, bool userPwd = true);

	// WD specific functions
	bool wdUnlockMod42();
	bool wdOemUnlock_WDC_WD80EB();
	bool wdVscEnSvcMode();
	bool wdVscReadSecSect(uint8_t numSects = 1);
	void wdExtractAndSetPwds(uint8_t* buf);

	// Seagate specific functions
	bool sgOemUnlock_ST310014A();
	bool sgOemUnlock_ST310211A();
	bool sgVscEnableDiagMode();
	bool sgVscDiagSendCmd(char* cmd);
	bool sgVscDiagRead(char* buf, uint32_t expLen = 0, uint32_t timeout = 10000);
	uint8_t sgVscDiagReadWait(uint8_t* status, uint32_t timeout = 10000);
	bool sgVscDiagSendByte(uint8_t cmdByte);
	bool sgVscDiagReadByte();
	bool sgVscDiagExecCmd(char* cmd, char* buffer, uint32_t expLen = 0, uint32_t timeout = 10000);
	void sgConvertHexPwdStr(uint32_t offset, uint32_t size, char* buf, uint8_t* out);
	void sgExtractAndSetPwds(char* buf, uint32_t mstrOffset, uint32_t userOffset);

	// WD ROYL specific functions
	bool wdRoylUnlock();
	bool wdRoylVscEnDis(bool disable = false);
	bool wdRoylSeekMod02();
	bool wdRoylRstDrv();
	bool wdRoylReadMod02(uint8_t numSects = 1);
	void wdRoylExtractAndSetPwds(uint8_t* buf, uint32_t idx);

	IDE_PORT mIdePort;
	IDE_DRIVE mIdeDrv;
	NTSTATUS mLastError;
	uint8_t mMasterPwd[ATA_SEC_PWD_SIZE];
	uint8_t mUserPwd[ATA_SEC_PWD_SIZE];
	bool mHasMasterPwd;
	bool mHasUserPwd;
	DRIVE_INFO mDrvInfo;
	IDENT_SECTOR mIdentSect;
	char* debugStr;
	uint32_t debugStrOffset;
	uint32_t mDebugArr[4096];
	uint32_t debugArrSize;
	uint32_t debugArrOff;

private:

};