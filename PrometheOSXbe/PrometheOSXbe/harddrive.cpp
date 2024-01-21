/*
**********************************
**********************************
**      BROUGHT TO YOU BY:		**
**********************************
**********************************
**								**
**		  [TEAM ASSEMBLY]		**
**								**
**		www.team-assembly.com	**
**								**
******************************************************************************************************
* This is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
* 
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
******************************************************************************************************

********************************************************************************************************
**	     XKHDD.CPP - Hard Disk Class Implementation      
********************************************************************************************************
**
**  This Class contains functionality to access a IDE HDD and query it by sending ATA Commands.
**  Member helper function included to parse ATA IDENTIFY data and get HDD Serial, Model etc.
**
**
********************************************************************************************************

********************************************************************************************************
**	CREDITS:
********************************************************************************************************
**	SPEEDBUMP:
**	----------
**		My utmost greatfullness and admiration goes towards SpeedBump for all his hard work..
**		I used most of his code and converted to C++ objects etc..
**
**	MRUEL:
**	------
**		Thanks for the pointing us to the Pascall Code that was used for Locking/Unlocking the disk..  
**		Great work on UnlockX.. keep it up !
**
**	MARTIN GERDES:
**	--------------
**		An editor of the german "c't" magazine. The article we referenced was published in 
**		c't Magazine 11/91 "Platten-Auslese - Konfiguration von AT-Bus-Platten lesen"
**		Thanks!, We ported this Pascall and ASM stuff to CPP to access the IDE Ports directly 
**
********************************************************************************************************


********************************************************************************************************
**	References:
********************************************************************************************************
**
**  ATA Specification:
**  ------------------
**  This stuff helped us a quite a bit..  so go check it out !
**  REFERENCE URL: ftp://fission.dt.wdc.com/pub/standards/ata/ata-3/ata3-r6.doc
**
********************************************************************************************************


UPDATE LOG:
--------------------------------------------------------------------------------------------------------
Date: 02/18/2003
By: UNDEAD [team-assembly]
Reason: Prepared 0.2 for Public Release
--------------------------------------------------------------------------------------------------------
Date: 01/06/2003
By: UNDEAD [team-assembly]
Reason: Prepared for Public Release
--------------------------------------------------------------------------------------------------------
*/

#include "harddrive.h"
#include "XKUtils\XKEEPROM.h"
#include "XKUtils\XKRC4.h"
#include "XKUtils\XKSHA1.h"
#include <conio.h>

//Byte swap the Data that comes back from ATA command..  and clean out blanks etc.
int XKHDD::CleanATAData(unsigned char *dst, unsigned char *src, int len)
{
	unsigned char tmp;
	int i;

	for(i=0; i < len; i+=2) 
	{
		tmp = src[i];		
		dst[i] = src[i+1];
		dst[i+1] = tmp;
	}
	
	--dst;
	for(i=len; i>0; --i) 
	{
		if (dst[i] != ' ') break;
	}

	return i;
}

void XKHDD::GenerateHDDPwd(UCHAR* HDDKey, UCHAR* IDEData, UCHAR* HDDPass)
{
	XKSHA1 SHA1Obj;

	UCHAR serial[0x14];
	ZeroMemory(serial, 0x14);

	UCHAR model[0x28];
	ZeroMemory(model, 0x28); 
	
	UCHAR s_length = 0x14;
	UCHAR m_length = 0x28;

	s_length = CleanATAData(serial, IDEData+HDD_SERIAL_OFFSET, s_length);
	m_length = CleanATAData(model, IDEData+HDD_MODEL_OFFSET, m_length);


	SHA1Obj.HMAC_SHA1 (HDDPass, HDDKey, 0x10, model, m_length, serial, s_length);
}

void XKHDD::GetIDEModel(UCHAR* IDEData, LPSTR ModelString, LPDWORD StrLen)
{
	UCHAR m_length = 0x28;
	m_length = CleanATAData((UCHAR*)ModelString, IDEData+HDD_MODEL_OFFSET, m_length);
	*StrLen = m_length;

}

WORD XKHDD::GetIDESecurityStatus(UCHAR* IDEData)
{
	WORD retVal = (WORD) *(IDEData+HDD_SECURITY_STATUS_OFFSET);
	return retVal;
}

void XKHDD::GetIDESerial(UCHAR* IDEData, LPSTR SerialString, LPDWORD StrLen)
{
	UCHAR s_length = 0x14;
	s_length = CleanATAData((UCHAR*)SerialString, IDEData+HDD_SERIAL_OFFSET, s_length);
	*StrLen = s_length;
}

//XBOX Version of Sending ATA Commands..  
//Since XBOX runs everything in Kernel Mode, we use direct port access :)
BOOL XKHDD::SendATACommand(WORD IDEPort, LPATA_COMMAND_OBJ ATACommandObj, UCHAR ReadWrite)
{
	BOOL retVal = FALSE;
	UCHAR waitcount = 10;
	WORD inVal=0;
	WORD SuccessRet = 0x58;

	LPDWORD PIDEDATA = (LPDWORD) &ATACommandObj->DATA_BUFFER ;
		
	//Write IDE Registers to IDE Port.. and in essence Execute the ATA Command..
	_outp(IDEPort + 1, ATACommandObj->IPReg.bFeaturesReg);
	Sleep(10);
	_outp(IDEPort + 2, ATACommandObj->IPReg.bSectorCountReg);
	Sleep(10);
	_outp(IDEPort + 3, ATACommandObj->IPReg.bSectorNumberReg);
	Sleep(10);
	_outp(IDEPort + 4, ATACommandObj->IPReg.bCylLowReg);
	Sleep(10);
	_outp(IDEPort + 5, ATACommandObj->IPReg.bCylHighReg);
	Sleep(10);
	_outp(IDEPort + 6, ATACommandObj->IPReg.bDriveHeadReg);
	Sleep(10);
	_outp(IDEPort + 7, ATACommandObj->IPReg.bCommandReg);
	Sleep(300);

	//Command Executed, Check Status.. If not success, wait a while..
	inVal = _inp(IDEPort+7); 
	while (((inVal & SuccessRet) != SuccessRet) && (waitcount > 0))
	{
		inVal = _inp(IDEPort+7); //Check Status..
		Sleep(300);
		waitcount--;
	}


	//IF Waitcount reaches 0 it means a TimeOut occured while waiting for command to complete
	//This will return FALSE...


	//Is this a IDE command that Requests Data, if so, Read the from IDE port ...
	if ((waitcount > 0) && (ReadWrite == IDE_COMMAND_READ))
	{
		//Read the command return output Registers
		ATACommandObj->OPReg.bErrorReg =		_inp(IDEPort + 1);
		ATACommandObj->OPReg.bSectorCountReg =	_inp(IDEPort + 2);
		ATACommandObj->OPReg.bSectorNumberReg =	_inp(IDEPort + 3);
		ATACommandObj->OPReg.bCylLowReg =		_inp(IDEPort + 4);
		ATACommandObj->OPReg.bCylHighReg =		_inp(IDEPort + 5);
		ATACommandObj->OPReg.bDriveHeadReg =	_inp(IDEPort + 6);
		ATACommandObj->OPReg.bStatusReg =		_inp(IDEPort + 7);

		ATACommandObj->DATA_BUFFSIZE = 512;
		Sleep(10);

		//Now read a sector (512 Bytes) from the IDE Port
		ZeroMemory(ATACommandObj->DATA_BUFFER, 512);
		for (int i = 0; i < 128; i++)
		{
			PIDEDATA[i] = _inpd(IDEPort);
			Sleep(5);
		}

		retVal = TRUE;
	}

	//Is this a IDE command that Sends Data, if so, write the Data to IDE Port..
	if ((waitcount > 0) && (ATACommandObj->DATA_BUFFSIZE > 0) && (ReadWrite == IDE_COMMAND_WRITE))
	{
		//Read the command return output Registers
		ATACommandObj->OPReg.bErrorReg =		_inp(IDEPort + 1);
		ATACommandObj->OPReg.bSectorCountReg =	_inp(IDEPort + 2);
		ATACommandObj->OPReg.bSectorNumberReg =	_inp(IDEPort + 3);
		ATACommandObj->OPReg.bCylLowReg =		_inp(IDEPort + 4);
		ATACommandObj->OPReg.bCylHighReg =		_inp(IDEPort + 5);
		ATACommandObj->OPReg.bDriveHeadReg  =	_inp(IDEPort + 6);
		ATACommandObj->OPReg.bStatusReg =		_inp(IDEPort + 7);

		Sleep(10);

		//Now Write a sector (512 Bytes) To the IDE Port
		for (int i = 0; i <  128; i++)
		{
			_outpd(IDEPort, PIDEDATA[i]);
				Sleep(5);

		}
		retVal = TRUE;
	}

	return retVal;
}


BOOL	m_EnryptedRegionValid;
BOOL	m_XBOX_EEPROM_Current;
LPEEPROMDATA EEPROMData;
BOOL EEPROMData_Enc;

XBOX_VERSION xver;
XKEEPROM m_xkeeprom;

void GetEEpromData()
{
	// Read EEprom from XBOX
	m_xkeeprom.ReadFromXBOX();

	m_XBOX_EEPROM_Current = TRUE;

}


void XKHDD::LockHDD()
{

	    GetEEpromData();
		//lua_pushstring(LUA_STATE, "Locking XBOX HDD");
		//lua_setglobal( LUA_STATE, "XBOX_LOCK_HDD_CHECK1" );

		//lua_pushstring(LUA_STATE, "Creating EEPROM Backup..");
		//lua_setglobal( LUA_STATE, "XBOX_LOCK_HDD_CHECK2" );

		//ATA Command Structure..
		XKHDD::ATA_COMMAND_OBJ hddcommand;
		UCHAR HddPass[32];
		UCHAR MasterPassword[13] = "TEAMASSEMBLY";
		
		//Dont Lock if the EEPROM data was NOT read from XBOX
		if(m_XBOX_EEPROM_Current)
		{
			//Decrypting EEPROM, if it fails.. Display fail message!
			if (m_xkeeprom.Decrypt())
			{
				// Get XBOX Version
				xver = m_xkeeprom.GetXBOXVersion();

				//lua_pushstring(LUA_STATE, "Querying HDD..");
				OutputDebugString("Querying HDD..\n");
				//Get IDE_ATA_IDENTIFY Data for HDD ..
				ZeroMemory(&hddcommand, sizeof(XKHDD::ATA_COMMAND_OBJ));
				hddcommand.DATA_BUFFSIZE = 0;
				hddcommand.IPReg.bDriveHeadReg = IDE_DEVICE_MASTER;
				hddcommand.IPReg.bCommandReg = IDE_ATA_IDENTIFY;
				XKHDD::SendATACommand(IDE_PRIMARY_PORT, &hddcommand, IDE_COMMAND_READ);

				XKEEPROM::EEPROMDATA tmpData;
				m_xkeeprom.GetEEPROMData(&tmpData);
				XKHDD::GenerateHDDPwd(tmpData.HDDKkey, hddcommand.DATA_BUFFER, HddPass);

				//Get ATA Locked State
				DWORD SecStatus = XKHDD::GetIDESecurityStatus(hddcommand.DATA_BUFFER);
				//Check if Disk is already locked..
				if ((SecStatus & IDE_SECURITY_SUPPORTED) && !(SecStatus & IDE_SECURITY_ENABLED))
				{

					OutputDebugString("Setting Master Pasword: TEAMASSEMBLY\n");
					//Execute HDD Unlock..
					ZeroMemory(&hddcommand, sizeof(XKHDD::ATA_COMMAND_OBJ));
					hddcommand.DATA_BUFFSIZE = 512;
					hddcommand.IPReg.bDriveHeadReg = IDE_DEVICE_MASTER;
					hddcommand.IPReg.bCommandReg = IDE_ATA_SECURITY_SETPASSWORD;
					LPBYTE HDDP = (LPBYTE)&hddcommand.DATA_BUFFER;
					LPDWORD pMastr = (LPDWORD) HDDP;
					*pMastr = 0x0001; //Set Master Pwd..
					memcpy(HDDP+2, MasterPassword, 13);
					XKHDD::SendATACommand(IDE_PRIMARY_PORT, &hddcommand, IDE_COMMAND_WRITE);


					OutputDebugString("Querying HDD..\n");
					//Query HDD To see if we succeeded..
					ZeroMemory(&hddcommand, sizeof(XKHDD::ATA_COMMAND_OBJ));
					hddcommand.DATA_BUFFSIZE = 512;
					hddcommand.IPReg.bDriveHeadReg = IDE_DEVICE_MASTER;
					hddcommand.IPReg.bCommandReg = IDE_ATA_IDENTIFY;
					XKHDD::SendATACommand(IDE_PRIMARY_PORT, &hddcommand, IDE_COMMAND_READ);
					SecStatus = XKHDD::GetIDESecurityStatus(hddcommand.DATA_BUFFER);
					
					OutputDebugString("Locking HDD..\n");
					//Execute HDD Unlock..
					ZeroMemory(&hddcommand, sizeof(XKHDD::ATA_COMMAND_OBJ));
					hddcommand.DATA_BUFFSIZE = 512;
					hddcommand.IPReg.bDriveHeadReg = IDE_DEVICE_MASTER;
					hddcommand.IPReg.bCommandReg = IDE_ATA_SECURITY_SETPASSWORD;
					memcpy(HDDP+2, HddPass, 20);
					XKHDD::SendATACommand(IDE_PRIMARY_PORT, &hddcommand, IDE_COMMAND_WRITE);


					//We Found That sometimes that by the first query it hasnt locked yet.. so we query twice if needed..
					int retrycnt = 0;
					do
					{
					
						OutputDebugString("Querying HDD..\n");
						//Query HDD To see if we succeeded..
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
						OutputDebugString("Remember Master PASS : TEAMASSEMBLY \n LOCKED HDD!\n");
						//lua_pushstring(LUA_STATE, "XBOX HDD Now Locked..");
					}
					else
					{
						OutputDebugString("FAILED TO LOCK HDD!\n");
						//lua_pushstring(LUA_STATE, "Failed to Lock HDD..");
					}
					//lua_setglobal(LUA_STATE, "XBOX_HDD_LOCK_STATUS");

					
				}
				else
				{
					if (!(SecStatus & IDE_SECURITY_SUPPORTED))
					{
						OutputDebugString("HDD DOES NOT SUPPORT LOCKING !\n");
						//lua_pushstring(LUA_STATE, "HDD DOES NOT SUPPORT LOCKING !");
					}
					else
					{
						OutputDebugString("XBOX HDD IS ALREADY LOCKED ???\n");
						//lua_pushstring(LUA_STATE, "XBOX HDD IS ALREADY LOCKED ???");
					}
					//lua_setglobal(LUA_STATE, "XBOX_HDD_LOCK_STATUS");
				}

				// Re-Encrypt EEPROM with current XBOX Version..
				m_xkeeprom.EncryptAndCalculateCRC(xver);

			}
			else
			{
				//Encrypted Region is Invalid.. DONT Allow Locking/Unlocking..
				m_EnryptedRegionValid = FALSE;

				//lua_pushstring(LUA_STATE, "EEPROM ENCRYPTED REGION INVALID");
			}
		}
		else
		{
			//lua_pushstring(LUA_STATE, "LOAD XBOX EEPROM TO LOCK !!");
		}
}

void XKHDD::UnlockHDD()
{
	    //GetEEpromData();
		OutputDebugString("Unlocking HDD \n Please Wait...\n");
		//lua_pushstring(LUA_STATE, "Unlocking HDD Please Wait...");
		//lua_setglobal( LUA_STATE, "XBOX_UNLOCK_HDD_CHECK1" );

		//ATA Command Structure..
		XKHDD::ATA_COMMAND_OBJ hddcommand;
		UCHAR HddPass[32];

		//Dont Lock if the EEPROM data was NOT read from XBOX
		if(m_XBOX_EEPROM_Current)
		{
			//Decrypting EEPROM, if it fails.. Display fail message!
			if (m_xkeeprom.Decrypt())
			{
				OutputDebugString("Querying HDD..\n");

				//Get IDE_ATA_IDENTIFY Data for HDD ..
				ZeroMemory(&hddcommand, sizeof(XKHDD::ATA_COMMAND_OBJ));
				hddcommand.DATA_BUFFSIZE = 0;
				hddcommand.IPReg.bDriveHeadReg = IDE_DEVICE_MASTER;
				hddcommand.IPReg.bCommandReg = IDE_ATA_IDENTIFY;
				XKHDD::SendATACommand(IDE_PRIMARY_PORT, &hddcommand, IDE_COMMAND_READ);

				XKEEPROM::EEPROMDATA tmpData;
				m_xkeeprom.GetEEPROMData(&tmpData);
				XKHDD::GenerateHDDPwd(tmpData.HDDKkey, hddcommand.DATA_BUFFER, HddPass);
				
				//Get ATA Locked State
				DWORD SecStatus = XKHDD::GetIDESecurityStatus(hddcommand.DATA_BUFFER);
				//Check if Disk is Reall locked..
				if ((SecStatus & IDE_SECURITY_SUPPORTED) && (SecStatus & IDE_SECURITY_ENABLED))
				{
					OutputDebugString("Unlocking HDD..\n");

					//Execute HDD Unlock..
					ZeroMemory(&hddcommand, sizeof(XKHDD::ATA_COMMAND_OBJ));
					hddcommand.DATA_BUFFSIZE = 512;
					hddcommand.IPReg.bDriveHeadReg = IDE_DEVICE_MASTER;
					hddcommand.IPReg.bCommandReg = IDE_ATA_SECURITY_UNLOCK;
					LPBYTE HDDP = (LPBYTE)&hddcommand.DATA_BUFFER;
					memcpy(HDDP+2, HddPass, 20);
					XKHDD::SendATACommand(IDE_PRIMARY_PORT, &hddcommand, IDE_COMMAND_WRITE);

					OutputDebugString("Querying HDD..\n");

					//Query HDD To see if we succeeded..
					ZeroMemory(&hddcommand, sizeof(XKHDD::ATA_COMMAND_OBJ));
					hddcommand.DATA_BUFFSIZE = 512;
					hddcommand.IPReg.bDriveHeadReg = IDE_DEVICE_MASTER;
					hddcommand.IPReg.bCommandReg = IDE_ATA_IDENTIFY;
					XKHDD::SendATACommand(IDE_PRIMARY_PORT, &hddcommand, IDE_COMMAND_READ);

					OutputDebugString("Disable HDD Password..\n");

					//Execute HDD Lock Disable
					ZeroMemory(&hddcommand, sizeof(XKHDD::ATA_COMMAND_OBJ));
					hddcommand.DATA_BUFFSIZE = 512;
					hddcommand.IPReg.bDriveHeadReg = IDE_DEVICE_MASTER;
					hddcommand.IPReg.bCommandReg = IDE_ATA_SECURITY_DISABLE;
					memcpy(HDDP+2, HddPass, 20);
					XKHDD::SendATACommand(IDE_PRIMARY_PORT, &hddcommand, IDE_COMMAND_WRITE);

					OutputDebugString("Querying HDD..\n");

					//We Found That sometimes that by the first query it hasnt unlocked yet.. so we query three times if needed..
					int retrycnt = 0;
					do
					{
					
						//Query HDD To see if we succeeded..
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
						OutputDebugString("Remember MASTER PASS: TEAMASSEMBLY \n HDD NOW UNLOCKED !\n");
					}
					else
					{
						OutputDebugString("FAILED TO UNLOCK HDD !\n");
					}

					
				}
				else
				{
					OutputDebugString("XBOX HDD NOT LOCKED ??\n");
				}

				//Re Encrypt EEPROM with Current XBOX Version
				m_xkeeprom.EncryptAndCalculateCRC(xver);

			}
			else
			{
				//Encrypted Region is Invalid.. DONT Allow Locking/Unlocking..
				m_EnryptedRegionValid = FALSE;

				OutputDebugString("EEPROM ENCRYPTED REGION INVALID !\n");
			}
		}
		else
		{
			OutputDebugString("LOAD XBOX EEPROM TO UNLOCK !\n");
		}
}

