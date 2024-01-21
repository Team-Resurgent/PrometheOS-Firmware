#pragma once

#include <xtl.h>

//Important ATA IDENTIFY Structure offsets..
//As per ATA Spec
#define HDD_SERIAL_OFFSET				0x014
#define HDD_MODEL_OFFSET				0x036
#define HDD_SECURITY_STATUS_OFFSET		0x100

//IDE Port Addresses
#define IDE_PRIMARY_PORT				0x01F0
#define IDE_SECONDARY_PORT				0x0170

//Important ATA Register Values
//As per ATA Spec
#define IDE_DEVICE_MASTER				0x00A0
#define IDE_DEVICE_SLAVE				0x00B0

//Important ATA/ATAPI Commands
//As per ATA Spec
#define IDE_ATAPI_IDENTIFY				0xA1  
#define IDE_ATA_IDENTIFY				0xEC  

#define	IDE_ATA_SECURITY_SETPASSWORD	0xF1
#define IDE_ATA_SECURITY_UNLOCK			0xF2
#define	IDE_ATA_SECURITY_FREEZE			0xF5
#define	IDE_ATA_SECURITY_DISABLE		0xF6 

//Important ATA IDENTIFY Data Structure values
//As per ATA Spec
#define IDE_SECURITY_SUPPORTED			0x0001
#define IDE_SECURITY_ENABLED			0x0002
#define IDE_SECURITY_PASSWORD_SET		0x0004
#define IDE_SECURITY_FROZEN				0x0008
#define IDE_SECURITY_COUNT_EXPIRED		0x0010
#define IDE_SECURITY_LEVEL_MAX			0x0100

//Important ATA Command return register values
//As per ATA Spec
#define IDE_ERROR_SUCCESS				0x0000
#define IDE_ERROR_ABORT					0x0004


//Our SendATACommand needs this to figure our if we should 
//read or write data to IDE registers..
#define	IDE_COMMAND_READ				0x00
#define	IDE_COMMAND_WRITE				0x01



class XKHDD
{
private:

public:


	//IDE ATA Input Registers Structure
	typedef struct IP_IDE_REG
	{
		BYTE bFeaturesReg;
		BYTE bSectorCountReg;
		BYTE bSectorNumberReg;
		BYTE bCylLowReg;
		BYTE bCylHighReg;
		BYTE bDriveHeadReg;
		BYTE bCommandReg;
	};
	typedef IP_IDE_REG* LPIP_IDE_REG;

	//IDE ATA Output Registers Structure
	typedef struct OP_IDE_REG
	{
		BYTE bErrorReg;
		BYTE bSectorCountReg;
		BYTE bSectorNumberReg;
		BYTE bCylLowReg;
		BYTE bCylHighReg;
		BYTE bDriveHeadReg;
		BYTE bStatusReg;
	};
	typedef OP_IDE_REG* LPOP_IDE_REG;

	//Our own object for issuing commands..
	//Includes in/ou register objects and 1 Sector of HDD Data
	typedef struct ATA_COMMAND_OBJ
	{
		IP_IDE_REG	IPReg;
		OP_IDE_REG	OPReg;
		BYTE		DATA_BUFFER[512];
		ULONG		DATA_BUFFSIZE;
	};
	typedef ATA_COMMAND_OBJ* LPATA_COMMAND_OBJ;





//This is the Function for Sending ATA Commands to the HDD..
//Right now we use Standard IOCTL stuff for Windows and direct 
//port access for XBOX..  later version will include Windows Driver 
//for direct port access in Windows so we can lock/unlock.  

	static BOOL	SendATACommand(WORD IDEPort, LPATA_COMMAND_OBJ ATACommandObj, UCHAR ReadWrite);

	//Helper Functions to Parse Data from ATA IDENTIFY Command
	static void	GetIDEModel(UCHAR* IDEData, LPSTR ModelString, LPDWORD StrLen);
	static void	GetIDESerial(UCHAR* IDEData, LPSTR SerialString, LPDWORD StrLen);
	static WORD	GetIDESecurityStatus(UCHAR* IDEData);
	static int	CleanATAData(unsigned char *dst, unsigned char *src, int len);

	//Given a XBOX HDDKey and ATA Identify data structure, this function calucates
	//Password the xbox will use when trying to unlock a drive..
	static void	GenerateHDDPwd(UCHAR* HDDKey, UCHAR* IDEData, UCHAR* HDDPass);

	static void LockHDD();
	static void UnlockHDD();
};
