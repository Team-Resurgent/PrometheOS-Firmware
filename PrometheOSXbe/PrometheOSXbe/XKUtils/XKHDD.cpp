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

#include "XKHDD.h"
#include "XKRC4.h"
#include "XKSHA1.h"
#include "..\xboxInternals.h"
#include <conio.h>

XKHDD::XKHDD()
{
}

XKHDD::~XKHDD(void)
{
}

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

char* XKHDD::GetIDEModel(UCHAR* IDEData)
{
	int length = 0x28;
	unsigned char* model = (unsigned char*)malloc(length + 1);
	length = CleanATAData(model, IDEData + HDD_MODEL_OFFSET, length);
	model[length] = 0;
	return (char*)model;

}

WORD XKHDD::GetIDESecurityStatus(UCHAR* IDEData)
{
	WORD retVal = (WORD) *(IDEData+HDD_SECURITY_STATUS_OFFSET);
	return retVal;
}

char* XKHDD::GetIDESerial(UCHAR* IDEData)
{
	int length = 0x14;
	unsigned char* serial = (unsigned char*)malloc(length + 1);
	length = CleanATAData(serial, IDEData + HDD_SERIAL_OFFSET, length);
	serial[length] = 0;
	return (char*)serial;
}

//XBOX Version of Sending ATA Commands..  
//Since XBOX runs everything in Kernel Mode, we use direct port access :)
BOOL XKHDD::SendATACommand(WORD IDEPort, LPATA_COMMAND_OBJ ATACommandObj, UCHAR ReadWrite)
{
	BOOL retVal = FALSE;
	DWORD defwaitcount = 30000;
	DWORD waitcount = defwaitcount;
	WORD inVal=0;
	WORD SuccessRet = 0x58;
	WORD flgBsy = 0x80;

	LPDWORD PIDEDATA = (LPDWORD) &ATACommandObj->DATA_BUFFER ;
		
	//Write IDE Registers to IDE Port.. and in essence Execute the ATA Command..
	_outp(IDEPort + 1, ATACommandObj->IPReg.bFeaturesReg);
	_outp(IDEPort + 2, ATACommandObj->IPReg.bSectorCountReg);
	_outp(IDEPort + 3, ATACommandObj->IPReg.bSectorNumberReg);
	_outp(IDEPort + 4, ATACommandObj->IPReg.bCylLowReg);
	_outp(IDEPort + 5, ATACommandObj->IPReg.bCylHighReg);
	_outp(IDEPort + 6, ATACommandObj->IPReg.bDriveHeadReg);
	_outp(IDEPort + 7, ATACommandObj->IPReg.bCommandReg);
	Sleep(50);

	//Command Executed, Check Status.. If not success, wait a while..
	inVal = _inp(IDEPort+7); 
	while (((inVal & SuccessRet) != SuccessRet) && (waitcount > 0))
	{
		inVal = _inp(IDEPort+7); //Check Status..
		KeStallExecutionProcessor(100);
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

		//Now read a sector (512 Bytes) from the IDE Port
		ZeroMemory(ATACommandObj->DATA_BUFFER, 512);
		for (int i = 0; i < 128; i++)
		{
			PIDEDATA[i] = _inpd(IDEPort);
		}

		waitcount = defwaitcount;
		inVal = _inp(0x200+IDEPort+6); 
		while (((inVal & flgBsy) == flgBsy) && (waitcount > 0))
		{
			inVal = _inp(0x200+IDEPort+6); //Check Status..
			KeStallExecutionProcessor(100);
			waitcount--;
		}

		retVal = waitcount > 0;
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

		//Now Write a sector (512 Bytes) To the IDE Port
		for (int i = 0; i <  128; i++)
		{
			_outpd(IDEPort, PIDEDATA[i]);
		}

		waitcount = defwaitcount;
		inVal = _inp(0x200+IDEPort+6); 
		while (((inVal & flgBsy) == flgBsy) && (waitcount > 0))
		{
			inVal = _inp(0x200+IDEPort+6); //Check Status..
			KeStallExecutionProcessor(100);
			waitcount--;
		}

		retVal = waitcount > 0;
	}

	return retVal;
}


