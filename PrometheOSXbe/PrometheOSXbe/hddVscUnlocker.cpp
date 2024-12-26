#include "hddVscUnlocker.h"
#include "xboxinternals.h"
#include "stringUtility.h"
#include "utils.h"
#include <conio.h>
#include <stdlib.h>

#ifdef _DEBUG
	#define DBG_STR_SIZE (1 * 1024 * 1024)
#else
	#define DBG_STR_SIZE (4096)
#endif

hddVscUnlocker::hddVscUnlocker() {
	mIdePort = IDE_PORT_PRIMARY;
	mIdeDrv = IDE_DRIVE_MASTER;
	mLastError = STATUS_SUCCESS;
	mHasMasterPwd = false;
	mHasUserPwd = false;

	debugStrOffset = 0;
	debugStr = (char*)malloc(DBG_STR_SIZE);
	memset(debugStr, 0, DBG_STR_SIZE);
	debugArrOff = 0;
	debugArrSize = 4096;
	memset(mDebugArr, 0, debugArrSize);

	memset(&mDrvInfo, 0, sizeof(DRIVE_INFO));
	memset(&mIdentSect, 0, sizeof(IDENT_SECTOR));
}

hddVscUnlocker::~hddVscUnlocker() {
	free(debugStr);
	freeIdentifyData();
}

void hddVscUnlocker::setIdePort(IDE_PORT port) {
	mIdePort = port;
}

void hddVscUnlocker::setIdeDrive(IDE_DRIVE drv) {
	mIdeDrv = drv;
}

void hddVscUnlocker::debug(char* str) {
	if(debugStrOffset >= DBG_STR_SIZE) {
		debugStrOffset = 0;
		memset(debugStr, 0, DBG_STR_SIZE);
	}

	uint32_t len = strlen(str);
	memcpy(debugStr + debugStrOffset, str, len);
	debugStrOffset += len;
}

void hddVscUnlocker::debugArr(uint32_t val) {
	mDebugArr[debugArrOff++] = val;
	if(debugArrOff>debugArrSize) {
		debugArrOff = 0;
		memset(mDebugArr, 0, debugArrSize);
	}
}

// This will set/clear bit 4 of the drive select register
uint8_t hddVscUnlocker::getDrvRegBits(uint8_t drvReg) {
	if(mIdeDrv == IDE_DRIVE_MASTER) {
		return drvReg & ~0x10;
	} else {
		return drvReg | 0x10;
	}
}

void hddVscUnlocker::freeIdentifyData() {
	free(mDrvInfo.model);
	free(mDrvInfo.serial);
	free(mDrvInfo.firmware);
	memset(&mDrvInfo, 0, sizeof(DRIVE_INFO));
}

bool hddVscUnlocker::waitForDrive(bool altPort, uint32_t timeout) {
	uint8_t flags = 0x80; // 1000 0000 = Drive busy flag
	uint8_t status = 0;
	uint16_t reg = mIdePort + 7;
	uint8_t scale = 10;
	timeout = timeout * scale;
	uint32_t start = timeout;

	// Read from alt status port, doesn't distrub any flags
	if(altPort) {
		reg = 0x200 + mIdePort + 6;
	}

	status = _inp(reg);
	while (((status & flags) == flags) && (timeout > 0)) {
		status = _inp(reg);
		KeStallExecutionProcessor(100);
		timeout--;
	}

	uint32_t time = (start - timeout) / scale;
	utils::debugPrint("[waitForDrive] status: %02X, reg: %02X, time: %lums\n", status, reg, time);

	if((status & flags) == flags) {
		mLastError = STATUS_IO_TIMEOUT;
		return false;
	}

	mLastError = STATUS_SUCCESS;
	return true;
}

bool hddVscUnlocker::spinWait(bool altPort, uint32_t timeout) {
	uint16_t reg = altPort ? (0x200 + mIdePort + 6) : (mIdePort + 7);
	uint8_t status = 0;
	mLastError = STATUS_SUCCESS;

	do {
		status = _inp(reg);
		if((status & 0x80) == 0) return true;
		KeStallExecutionProcessor(100);
	} while(--timeout);

	mLastError = STATUS_IO_TIMEOUT;
	return false;
}

bool hddVscUnlocker::wdUnlockMod42() {
	if(!wdVscEnSvcMode()) {
		utils::debugPrint("wdVscEnSvcMode err: 0x%08X\n", getLastError());
		return false;
	}

	if(!wdVscReadSecSect()) {
		utils::debugPrint("wdVscReadSecSect err: 0x%08X\n", getLastError());
		return false;
	}

	uint8_t buf[8192];
	uint32_t firstSectSize = 512;
	uint32_t sectIdx = 11;
	memset(buf, 0, sizeof(buf));

	readDriveData(buf, firstSectSize);
	utils::hexDumpDebug(buf, firstSectSize);

	uint8_t numSects = ((uint16_t*)buf)[sectIdx];
	uint32_t numBytes = numSects * 512;

	utils::debugPrint("wdMod42 numSects: 0x%04X - %d\n", numSects, numBytes);

	if(numBytes > sizeof(buf)) {
		utils::debugPrint("Unable to allocate Mod42 buffer\n");
		return false;
	}

	if(!wdVscReadSecSect(numSects)) {
		utils::debugPrint("wdVscReadSecSect_2 err: 0x%08X\n", getLastError());
		return false;
	}

	readDriveData(buf, numBytes);
	utils::hexDumpDebug(buf, numBytes);

	if(!resetIdeBus()) {
		return false;
	}

	wdExtractAndSetPwds(buf);

	if(!doAtaSecUnlock()) {
		return false;
	}

	mLastError = STATUS_SUCCESS;
	return true;
}

bool hddVscUnlocker::wdOemUnlock_WDC_WD80EB() {
	return wdUnlockMod42();
}

bool hddVscUnlocker::wdRoylUnlock() {
	uint8_t statusReg = mIdePort + 7;

	if(!wdRoylVscEnDis()) {
		utils::debugPrint("wdRoylVscEnDis err: 0x%08X\n", getLastError());
		return false;
	}

	if(!wdRoylSeekMod02()) {
		utils::debugPrint("wdRoylSeekMod02 err: 0x%08X\n", getLastError());
		return false;
	}

	if(!wdRoylReadMod02()) {
		utils::debugPrint("wdRoylReadMod02_1 err: 0x%08X\n", getLastError());
		return false;
	}

	uint8_t buf[8192];
	uint32_t firstSectSize = 512;
	uint32_t sectIdx = 5;
	uint32_t passIdx = 0x3d;
	memset(buf, 0, sizeof(buf));

	readDriveData(buf, firstSectSize);
	utils::hexDumpDebug(buf, firstSectSize);

	if(memcmp(buf, "ROYL", 4) != 0) {
		// Check for in-between drives (2005-ish)
		if(memcmp(buf + 0x8c, "WD-", 3) == 0) {
			sectIdx = 11;
			passIdx = 0x31;
		} else {
			utils::debugPrint("Not a WD ROYL drive\n");
			return wdUnlockMod42();
		}
	}

	uint8_t numSects = ((uint16_t*)buf)[sectIdx] - 1;
	uint32_t numBytes = numSects * 512;

	utils::debugPrint("wdRoyl numSects: 0x%04X - %d\n", numSects, numBytes);

	if((numBytes + firstSectSize) > sizeof(buf)) {
		utils::debugPrint("Unable to allocate ROYL buffer\n");
		return false;
	}

	if(!wdRoylReadMod02(numSects)) {
		utils::debugPrint("wdRoylReadMod02_2 err: 0x%08X\n", getLastError());
		return false;
	}

	readDriveData(buf + firstSectSize, numBytes);
	utils::hexDumpDebug(buf + firstSectSize, numBytes);

	if(!wdRoylVscEnDis(true)) {
		utils::debugPrint("wdRoylVscEnDis err: 0x%08X\n", getLastError());
		return false;
	}

	if(!waitForDrive(false)) {
		return false;
	}

	wdRoylExtractAndSetPwds(buf, passIdx);

	if(!doAtaSecUnlock()) {
		return false;
	}

	mLastError = STATUS_SUCCESS;
	return true;
}

bool hddVscUnlocker::wdRoylVscEnDis(bool disable) {
	IDE_CMD cmd = { disable ? 0x44 : 0x45, 0x0b, 0x00, 0x44, 0x57, 0xa0, 0x80 };
	if(!sendDriveCmd(cmd)) {
		utils::debugPrint("sendDriveCmd err: %08X\n", getLastError());
		return false;
	}

	mLastError = STATUS_SUCCESS;
	return true;
}

bool hddVscUnlocker::wdRoylReadMod02(uint8_t numSects) {
	IDE_CMD cmd = { 0xd5, numSects, 0xbf, 0x4f, 0xc2, 0xa0, 0xb0 };
	if(!sendDriveCmd(cmd)) {
		utils::debugPrint("sendDriveCmd err: %08X\n", getLastError());
		return false;
	}

	mLastError = STATUS_SUCCESS;
	return true;
}

bool hddVscUnlocker::wdRoylSeekMod02() {
	IDE_CMD cmd = { 0xd6, 0x01, 0xbe, 0x4f, 0xc2, 0xa0, 0xb0 };
	if(!sendDriveCmd(cmd)) {
		utils::debugPrint("sendDriveCmd err: %08X\n", getLastError());
		return false;
	}

	uint8_t buf[512];
	memset(buf, 0, sizeof(buf));
	buf[0] = 0x0b;
	buf[1] = 0x00;
	buf[2] = 0x04;
	buf[3] = 0x00;
	buf[4] = 0x02;

	if(!writeDriveData(buf, sizeof(buf))) {
		utils::debugPrint("writeDriveData err: %08X\n", getLastError());
		return false;
	}

	mLastError = STATUS_SUCCESS;
	return true;
}

bool hddVscUnlocker::wdRoylRstDrv() {
	IDE_CMD cmd = { 0xd6, 0x01, 0xbe, 0x4f, 0xc2, 0xa0, 0xb0 };
	if(!sendDriveCmd(cmd)) {
		utils::debugPrint("sendDriveCmd err: %08X\n", getLastError());
		return false;
	}

	uint8_t buf[512];
	memset(buf, 0, sizeof(buf));
	buf[0] = 0x1c;
	buf[1] = 0x00;
	buf[2] = 0x01;

	if(!writeDriveData(buf, sizeof(buf))) {
		utils::debugPrint("writeDriveData err: %08X\n", getLastError());
		return false;
	}

	mLastError = STATUS_SUCCESS;
	return true;
}

void hddVscUnlocker::sgConvertHexPwdStr(uint32_t offset, uint32_t size, char* buf, uint8_t* out) {
	for(uint32_t i = offset, n = 0; i < (offset + size); ) {
		uint8_t upper = buf[i];
		uint8_t lower = buf[i+1];

		if(upper == 0x20 || upper == 0x0d) {
			i++; continue;
		}

		if(upper == 0x0a) {
			i+=8; continue;
		}

		char hexStr[3] = { upper, lower, '\0' };
		out[n] = (uint8_t)strtol(hexStr, NULL, 16);
		n++;
		i+=2;
	}
}

void hddVscUnlocker::sgExtractAndSetPwds(char* buf, uint32_t mstrOffset, uint32_t userOffset) {
	mHasMasterPwd = true;
	mHasUserPwd = true;

	sgConvertHexPwdStr(mstrOffset, 0x50, buf, mMasterPwd);
	sgConvertHexPwdStr(userOffset, 0x50, buf, mUserPwd);

	utils::debugPrint("Seagate Master Pwd:\n");
	utils::hexDumpDebug(mMasterPwd, ATA_SEC_PWD_SIZE);

	utils::debugPrint("Seagate User Pwd:\n");
	utils::hexDumpDebug(mUserPwd, ATA_SEC_PWD_SIZE);
}

bool hddVscUnlocker::sgVscEnableDiagMode() {
	IDE_CMD cmd = { 0, 0x44, 0x69, 0x61, 0x47, 0xe0, 0xfe }; // ".DiaG.."
	sendDriveRawCmd(cmd);
	KeStallExecutionProcessor(10000);
	return spinWait();
}

bool hddVscUnlocker::sgVscDiagSendByte(uint8_t cmdByte) {
	IDE_CMD cmd = { 0, 1, 0, 0, cmdByte, 0xe0, 0xfe };
	sendDriveRawCmd(cmd);
	KeStallExecutionProcessor(10000);
	return spinWait(true);
}

bool hddVscUnlocker::sgVscDiagSendCmd(char* cmd) {
	bool res = true;
	uint32_t status = 0;

	while(*cmd != 0) {
		if(!sgVscDiagSendByte(*cmd)) {
			res = false;
			break;
		}

		status = _inp(mIdePort + 7);
		if((status & 1) != 0) {
			mLastError = STATUS_INVALID_DEVICE_REQUEST;
			res = false;
			break;
		}

		KeStallExecutionProcessor(10000);
		cmd++;
	}

	KeStallExecutionProcessor(10000);
	return res;
}

uint8_t hddVscUnlocker::sgVscDiagReadWait(uint8_t* status, uint32_t timeout) {
	do {
		*status = _inp(0x200 + mIdePort + 6);

		if(((*status & 0x80) == 0) && ((*status & 0x40) != 0)) {
			return 1;
		}

		KeStallExecutionProcessor(100);
	} while(--timeout);

	return 0;
}

bool hddVscUnlocker::sgVscDiagRead(char* buf, uint32_t expLen, uint32_t timeout) {
	IDE_CMD cmd = { 0, 1, 0, 0, 0, 0xe0, 0xff };
	uint8_t status;
	uint32_t i = 0;
	uint32_t bufsize = 0;
	char* start = buf;
	bool res = true;
	bool hasExpLen = !!expLen;

	do {
		if(i >= 10) {
			res = false;
			break;
		}

		--expLen;

		if(hasExpLen && expLen == 0) {
			timeout = 1000;
		}

		sendDriveRawCmd(cmd);
		KeStallExecutionProcessor(1000);
		uint8_t waitRes = sgVscDiagReadWait(&status, timeout);

		if(!waitRes) {
			*buf = _inp(mIdePort + 4);
			buf++;
			res = false;
			break;
		}

		if((status & 1) == 0) {
			*buf = _inp(mIdePort + 4);
			buf++;
		} else {
			uint8_t flags = _inp(mIdePort + 2);
			if((flags & 1) == 0) {
				res = false;
				break;
			}
			i++;
		}
	} while(true);

	*buf = 0;
	KeStallExecutionProcessor(10000);
	return res;
}

bool hddVscUnlocker::sgVscDiagExecCmd(char* cmd, char* buffer, uint32_t expLen, uint32_t timeout) {
	sgVscDiagSendCmd(cmd);
	sgVscDiagRead(buffer, strlen(cmd), timeout);
	sgVscDiagSendCmd("\r");
	sgVscDiagRead(buffer, expLen, timeout);
	return true;
}

bool hddVscUnlocker::sgOemUnlock_ST310014A() {
	bool res = true;
	mLastError = STATUS_SUCCESS;

	char discard[512];
	char buffer[2048];
	memset(discard, 0xcc, sizeof(discard));
	memset(buffer, 0xcc, sizeof(buffer));

	uint8_t irql;
	HalGetInterruptVector(0xe, &irql); // 0xe = PNPISABus
	uint8_t oldIrql = KfRaiseIrql(irql);

	if(!sgVscEnableDiagMode()) {
		res = false;
		goto out;
	}


	sgVscDiagSendCmd("\x1a");                 // Ctrl-Z (enable terminal)
	sgVscDiagRead(discard, 20);

	sgVscDiagExecCmd("/2", discard, 4);       // change to diagnostic command level 2
	sgVscDiagExecCmd("S006b", discard, 4);    // seek to logical cylinder/head, i.e. track 6Bh
	sgVscDiagExecCmd("R20,01", discard, 4);   // read sector 20h into buffer
	sgVscDiagExecCmd("C0,570", discard, 4);   // copy one sector from buffer 0 to buffer 570h
	sgVscDiagExecCmd("B570", buffer, 0x5ea);  // print buffer 570h to terminal

	sgVscDiagSendCmd("\x03");                 // Ctrl-C (soft reset device)
	sgVscDiagRead(discard, 0x29, 30000);

	KeStallExecutionProcessor(100000);
	resetIdeBusRaw();

out:
	KfLowerIrql(oldIrql);

	sgExtractAndSetPwds(buffer, 0xa8, 0xf8);

	if(!waitForDrive()) {
		res = false;
	}

	if(!doAtaSecUnlock()) {
		res = false;
	}

	return res;
}

bool hddVscUnlocker::sgOemUnlock_ST310211A() {
	uint8_t irql;
	HalGetInterruptVector(0xe, &irql); // 0xe = PNPISABus
	uint8_t irqlVect = KfRaiseIrql(irql);

	bool res = true;
	mLastError = STATUS_SUCCESS;

	char discard[512];
	char buffer[2048];
	memset(discard, 0, sizeof(discard));
	memset(buffer, 0, sizeof(buffer));

	if(!sgVscEnableDiagMode()) {
		res = false;
		goto out;
	}

	sgVscDiagSendCmd("\x1a");
	sgVscDiagRead(discard, 12);

	sgVscDiagExecCmd("GFFFFFFF3", discard, 4);
	sgVscDiagExecCmd("/2", discard, 4);
	sgVscDiagExecCmd("B0,0", buffer, 0x5e2);

	sgVscDiagSendCmd("\x03");
	sgVscDiagRead(discard, 0x91, 30000);

	KeStallExecutionProcessor(100000);
	resetIdeBusRaw();

out:
	KfLowerIrql(irqlVect);

	sgExtractAndSetPwds(buffer, 0xa0, 0xf0);

	if(!waitForDrive()) {
		res = false;
	}

	if(!doAtaSecUnlock()) {
		res = false;
	}

	return res;
}

void hddVscUnlocker::wdExtractAndSetPwds(uint8_t* buf) {
	memcpy(mMasterPwd, buf + WD_INFO_MASTER_PWD_OFFSET, ATA_SEC_PWD_SIZE);
	utils::debugPrint("WD Master Pwd:\n");
	utils::hexDumpDebug(mMasterPwd, ATA_SEC_PWD_SIZE);

	memcpy(mUserPwd, buf + WD_INFO_USER_PWD_OFFSET, ATA_SEC_PWD_SIZE);
	utils::debugPrint("WD User Pwd:\n");
	utils::hexDumpDebug(mUserPwd, ATA_SEC_PWD_SIZE);

	mHasMasterPwd = true;
	mHasUserPwd = true;
}

void hddVscUnlocker::wdRoylExtractAndSetPwds(uint8_t* buf, uint32_t idx) {
	uint16_t passRecOff = ((uint16_t*)buf)[idx];
	utils::debugPrint("WD ROYL Password Offset: 0x%04x\n", passRecOff);

	memcpy(mMasterPwd, buf + passRecOff + 4 + ATA_SEC_PWD_SIZE, ATA_SEC_PWD_SIZE);
	utils::debugPrint("WD ROYL Master Pwd:\n");
	utils::hexDumpDebug(mMasterPwd, ATA_SEC_PWD_SIZE);

	memcpy(mUserPwd, buf + passRecOff + 4, ATA_SEC_PWD_SIZE);
	utils::debugPrint("WD ROYL User Pwd:\n");
	utils::hexDumpDebug(mUserPwd, ATA_SEC_PWD_SIZE);

	mHasMasterPwd = true;
	mHasUserPwd = true;
}

void hddVscUnlocker::parseIdentifySector() {
	freeIdentifyData();

	utils::byteSwap16((uint8_t*)mIdentSect.model, sizeof(mIdentSect.model));
	utils::byteSwap16((uint8_t*)mIdentSect.serial, sizeof(mIdentSect.serial));
	utils::byteSwap16((uint8_t*)mIdentSect.firmware, sizeof(mIdentSect.firmware));

	char model[sizeof(mIdentSect.model) + 1] = { 0 };
	char serial[sizeof(mIdentSect.serial) + 1] = { 0 };
	char firmware[sizeof(mIdentSect.firmware) + 1] = { 0 };

	memcpy(model, mIdentSect.model, sizeof(mIdentSect.model));
	memcpy(serial, mIdentSect.serial, sizeof(mIdentSect.serial));
	memcpy(firmware, mIdentSect.firmware, sizeof(mIdentSect.firmware));

	stringUtility::fTrim(model);
	stringUtility::fTrim(serial);
	stringUtility::fTrim(firmware);

	mDrvInfo.model = strdup(model);
	mDrvInfo.serial = strdup(serial);
	mDrvInfo.firmware = strdup(firmware);
}

bool hddVscUnlocker::doAtaSecUnlock() {
	if(!sendSecUnlockDisableCmd(false)) {
		utils::debugPrint("sendSecUnlockDisableCmd err: 0x%08X\n", getLastError());
		return false;
	}

	if(!identifyDrive()) {
		utils::debugPrint("identifyDrive err: 0x%08X\n", getLastError());
		return false;
	}

	if(mIdentSect.secLocked) {
		utils::debugPrint("Drive still locked, giving up.\n");
		return false;
	}

	if(!sendSecUnlockDisableCmd()) {
		utils::debugPrint("sendSecUnlockDisableCmd err: 0x%08X\n", getLastError());
		return false;
	}

	if(!identifyDrive()) {
		utils::debugPrint("identifyDrive err: 0x%08X\n", getLastError());
		return false;
	}

	if(mIdentSect.secEnabled) {
		utils::debugPrint("Drive lock still enabled, giving up.\n");
		return false;
	}

	mLastError = STATUS_SUCCESS;
	return true;
}

bool hddVscUnlocker::identifyDrive() {
	if(!sendIdentifyCmd()) {
		utils::debugPrint("sendIdentifyCmd err: 0x%08X\n", getLastError());
		return false;
	}

	readDriveData((uint8_t*)&mIdentSect, sizeof(IDENT_SECTOR));
	parseIdentifySector();

	//utils::hexDumpDebug(&mIdentSect, sizeof(mIdentSect));
	utils::debugPrint("[identifyDrive] model: \"%s\", serial: \"%s\", fw: \"%s\"\n", mDrvInfo.model, mDrvInfo.serial, mDrvInfo.firmware);

	mLastError = STATUS_SUCCESS;
	return true;
}

void hddVscUnlocker::sendDriveRawCmd(IDE_CMD cmd) {
	// Set the selected drive bit (master/slave)
	cmd.drv = getDrvRegBits(cmd.drv);

	_outp(mIdePort + 1, cmd.feat);
	_outp(mIdePort + 2, cmd.nSect);
	_outp(mIdePort + 3, cmd.sLow);
	_outp(mIdePort + 4, cmd.sMed);
	_outp(mIdePort + 5, cmd.sHigh);
	_outp(mIdePort + 6, cmd.drv);
	_outp(mIdePort + 7, cmd.cmd); // Sending cmd initiates drive action and raises BSY flag
}

bool hddVscUnlocker::sendDriveCmd(IDE_CMD cmd, bool altPort, uint32_t timeout) {
	if(!waitForDrive(altPort, timeout)) {
		return false;
	}

	utils::debugPrint("[sendDriveCmd] cmd:\n");
	utils::hexDumpDebug(&cmd, sizeof(cmd));

	sendDriveRawCmd(cmd);

	if(!waitForDrive(altPort, timeout)) {
		return false;
	}

	mLastError = STATUS_SUCCESS;
	return true;
}

hddVscUnlocker::IDE_REG hddVscUnlocker::readDriveReg(bool altPort) {
	IDE_REG reg = { 0, 0, 0, 0, 0, 0, 0 };
	uint8_t statusReg = altPort ? (0x200 + mIdePort + 6) : (mIdePort + 7);

	reg.err    = _inp(mIdePort + 1);
	reg.nSect  = _inp(mIdePort + 2);
	reg.sLow   = _inp(mIdePort + 3);
	reg.sMed   = _inp(mIdePort + 4);
	reg.sHigh  = _inp(mIdePort + 5);
	reg.drv    = _inp(mIdePort + 6);
	reg.status = _inp(statusReg);

	utils::debugPrint("[readDriveReg] reg:\n");
	utils::hexDumpDebug(&reg, sizeof(reg));

	return reg;
}

bool hddVscUnlocker::readDriveData(uint8_t* buf, uint32_t size) {
	uint32_t cnt = size / sizeof(uint32_t);
	uint32_t* data = (uint32_t*)buf;

	if(!waitForDrive()) {
		return false;
	}

	uint8_t tmp = 4;

	for (uint32_t i = 0; (i < cnt); i++) {
		data[i] = _inpd(mIdePort);
		KeStallExecutionProcessor(500);
	}

	if(!waitForDrive(false)) {
		return false;
	}

	mLastError = STATUS_SUCCESS;
	return true;
}

bool hddVscUnlocker::writeDriveData(uint8_t* buf, uint32_t size) {
	uint32_t cnt = size / sizeof(uint32_t);
	uint32_t* data = (uint32_t*)buf;

	utils::debugPrint("[writeDriveData] buf:\n");
	utils::hexDumpDebug(buf, size);

	if(!waitForDrive()) {
		return false;
	}

	for (uint32_t i = 0; (i < cnt); i++) {
		_outpd(mIdePort, data[i]);
		KeStallExecutionProcessor(500);
	}

	if(!waitForDrive(false)) {
		return false;
	}

	mLastError = STATUS_SUCCESS;
	return true;
}

void hddVscUnlocker::resetIdeBusRaw() {
	uint16_t reg = 0x200 + mIdePort + 6;

	_outp(reg, 0);
	KeStallExecutionProcessor(50);
	_outp(reg, 4);
	KeStallExecutionProcessor(100);
	_outp(reg, 0);
	KeStallExecutionProcessor(50);
}

bool hddVscUnlocker::resetIdeBus() {
	utils::debugPrint("Resetting IDE bus on port 0x%X...\n", mIdePort);

	if(!waitForDrive()) {
		return false;
	}

	resetIdeBusRaw();

	if(!waitForDrive(false)) {
		return false;
	}

	mLastError = STATUS_SUCCESS;
	return true;
}

bool hddVscUnlocker::isDriveLocked(bool doId) {
	if(doId && !identifyDrive()) {
		utils::debugPrint("identifyDrive err: 0x%08X\n", getLastError());
		return true;
	}

	utils::debugPrint("[isDriveLocked] secEn: %u, secLock: %u\n", mIdentSect.secEnabled, mIdentSect.secLocked);

	return mIdentSect.secEnabled && mIdentSect.secLocked;
}

// checkOnly = Check if drive model is supported
bool hddVscUnlocker::attemptVscUnlock(bool checkOnly) {
	bool result = false;

	if(!checkOnly && !identifyDrive()) {
		utils::debugPrint("identifyDrive err: 0x%08X\n", getLastError());
		return false;
	}

	if(!checkOnly && !isDriveLocked(false)) {
		utils::debugPrint("Drive \"%s\" is already unlocked, quitting...\n", mDrvInfo.model);
		return false;
	}

	mHasMasterPwd = false;
	mHasUserPwd = false;

	if(memcmp(mDrvInfo.model, "WDC WD80EB", 10) == 0) {
		if(checkOnly) return true;
		result = wdOemUnlock_WDC_WD80EB();
	} else if(memcmp(mDrvInfo.model, "ST310014A", 9) == 0) {
		if(checkOnly) return true;
		result = sgOemUnlock_ST310014A();
	} else if(memcmp(mDrvInfo.model, "ST310211A", 9) == 0) {
		if(checkOnly) return true;
		result = sgOemUnlock_ST310211A();
	} else if(memcmp(mDrvInfo.model, "WDC ", 4) == 0) {
		if(checkOnly) return true;
		result = wdRoylUnlock();
	} else {
		mLastError = STATUS_DEVICE_DOES_NOT_EXIST;
		result = false;
	}

	return result;
}

bool hddVscUnlocker::checkVscUnlock() {
	return attemptVscUnlock(true);
}

bool hddVscUnlocker::getMasterPwd(uint8_t* buf) {
	if(mHasMasterPwd) {
		memcpy(buf, mMasterPwd, ATA_SEC_PWD_SIZE);

		mLastError = STATUS_SUCCESS;
		return true;
	}

	mLastError = STATUS_NOT_FOUND;
	return false;
}

bool hddVscUnlocker::getUserPwd(uint8_t* buf) {
	if(mHasUserPwd) {
		memcpy(buf, mUserPwd, ATA_SEC_PWD_SIZE);

		mLastError = STATUS_SUCCESS;
		return true;
	}

	mLastError = STATUS_NOT_FOUND;
	return false;
}

hddVscUnlocker::DRIVE_INFO hddVscUnlocker::getDriveInfo() {
	DRIVE_INFO ret;
	if(!mDrvInfo.model) identifyDrive();
	if(mDrvInfo.model) ret.model = strdup(mDrvInfo.model);
	if(mDrvInfo.serial) ret.serial = strdup(mDrvInfo.serial);
	if(mDrvInfo.firmware) ret.firmware = strdup(mDrvInfo.firmware);
	return ret;
}

bool hddVscUnlocker::sendSecUnlockDisableCmd(bool disable, bool userPwd) {
	uint8_t* pwd = userPwd ? mUserPwd : mMasterPwd;

	if(!pwd) {
		return false;
	}

	uint8_t op = disable ? 0xf6 : 0xf2;

	IDE_CMD cmd = { 0, 0, 0, 0, 0, 0xa0, op };
	if(!sendDriveCmd(cmd)) {
		utils::debugPrint("sendDriveCmd err: %08X\n", getLastError());
		return false;
	}

	uint8_t buf[512];
	memset(buf, 0, sizeof(buf));
	buf[0] = userPwd ? 0 : 1;
	memcpy(buf+2, pwd, ATA_SEC_PWD_SIZE);

	if(!writeDriveData(buf, sizeof(buf))) {
		utils::debugPrint("writeDriveData err: %08X\n", getLastError());
		return false;
	}

	mLastError = STATUS_SUCCESS;
	return true;
}

bool hddVscUnlocker::sendIdentifyCmd() {
	IDE_CMD cmd = { 0, 0, 0, 0, 0, 0xa0, 0xec };
	if(!sendDriveCmd(cmd)) {
		utils::debugPrint("sendDriveCmd err: %08X\n", getLastError());
		return false;
	}

	mLastError = STATUS_SUCCESS;
	return true;
}

bool hddVscUnlocker::wdVscEnSvcMode() {
	IDE_CMD cmd = { 0x57, 0x44, 0x43, 0, 0, 0xa0, 0x8a };
	if(!sendDriveCmd(cmd)) {
		utils::debugPrint("sendDriveCmd err: %08X\n", getLastError());
		return false;
	}

	mLastError = STATUS_SUCCESS;
	return true;
}

bool hddVscUnlocker::wdVscReadSecSect(uint8_t numSects) {
	IDE_CMD cmd = { 0, numSects, 0, 0, 0x0f, 0xe0, 0x21 };
	if(!sendDriveCmd(cmd)) {
		utils::debugPrint("sendDriveCmd err: %08X\n", getLastError());
		return false;
	}

	mLastError = STATUS_SUCCESS;
	return true;
}

bool hddVscUnlocker::sgVscDiagReadByte() {
	IDE_CMD cmd = { 0, 1, 0, 0, 0, 0xe0, 0xff };
	if(!sendDriveCmd(cmd, true)) {
		utils::debugPrint("sendDriveCmd err: %08X\n", getLastError());
		return false;
	}

	mLastError = STATUS_SUCCESS;
	return true;
}

NTSTATUS hddVscUnlocker::getLastError() {
	return mLastError;
}
