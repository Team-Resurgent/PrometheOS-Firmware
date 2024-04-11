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
**	     XKEEPROM.CPP - XBOX EEPROM Class' Implementation      
********************************************************************************************************
**
**	This Class encapsulates the XBOX EEPROM strucure and many helper functions to parse,
**	Calculate CRC's and Decrypt various values in the XBOX EEPROM..  
**
********************************************************************************************************

********************************************************************************************************
**	CREDITS:
********************************************************************************************************
**	XBOX-LINUX TEAM:
**  ---------------
**		Wow, you guys are awsome !!  I bow down to your greatness !!  the "Friday 13th" Middle 
**		Message Hack really saved our butts !!
**		REFERENCE URL:  http://xbox-linux.sourceforge.net
**
********************************************************************************************************

UPDATE LOG:
--------------------------------------------------------------------------------------------------------
Date: 11/27/2004
By: Yoshihiro
Reason: Update for xbox 1.6
--------------------------------------------------------------------------------------------------------
Date: 02/18/2003
By: UNDEAD [team-assembly]
Reason: Prepared 0.2 for Public Release
--------------------------------------------------------------------------------------------------------
Date: 01/25/2003
By: UNDEAD [team-assembly]
Reason: Added XBOX Specific code to read EEPROM Data from Hardware
--------------------------------------------------------------------------------------------------------
Date: 01/06/2003
By: UNDEAD [team-assembly]
Reason: Prepared for Public Release
--------------------------------------------------------------------------------------------------------

*/

#include "xkeeprom.h"

#include "..\xboxInternals.h"
#include "..\stringUtility.h"

/* Default Constructor using a Blank eeprom image... */
XKEEPROM::XKEEPROM()
{
	m_XBOX_Version = V1_0;
	ZeroMemory(&m_EEPROMData, sizeof(EEPROMDATA));
	m_EncryptedState = FALSE;

}

/* Constructor to specify a eeprom image to use ... */
XKEEPROM::XKEEPROM(LPEEPROMDATA pEEPROMData, BOOL Encrypted)
{
	m_XBOX_Version = V_NONE;
	memcpy(&m_EEPROMData, (LPBYTE)pEEPROMData, sizeof(EEPROMDATA));
	m_EncryptedState = Encrypted;
}

/* Default Destructor */
XKEEPROM::~XKEEPROM(void)
{
}

/* Read a EEPROM image from a .BIN file.. */
/* could be a decrypted or Enrytped.. make sure you specify correct value */
bool XKEEPROM::ReadFromBINFile(const char* FileName, bool isEncrypted)
{
	//First Make sure the File exists...
	WIN32_FIND_DATA wfd;
	HANDLE hf = FindFirstFile(FileName, &wfd);
	if (hf == INVALID_HANDLE_VALUE)
		return false;

	//Now Read the EEPROM Data from File..
	BYTE Data[EEPROM_SIZE];
	DWORD dwBytesRead = 0;
	ZeroMemory(Data, EEPROM_SIZE);
	
	hf = CreateFile(FileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	bool retVal = ReadFile(hf, Data, EEPROM_SIZE, &dwBytesRead, NULL) == TRUE;
	if (retVal && (dwBytesRead >= EEPROM_SIZE))
	{
		memcpy(&m_EEPROMData, Data, EEPROM_SIZE);
		m_EncryptedState = isEncrypted;
	}
	else
		retVal = false;

	CloseHandle(hf);

	return retVal;
}

/* Write the Current EEPROM image to a .BIN file.. */
bool XKEEPROM::WriteToBINFile(const char* fileName)
{
	bool retVal = false;
	DWORD dwBytesWrote = 0;

	//Only write bin file if encrypted..
	if (m_EncryptedState)
	{
		HANDLE hf = CreateFile(fileName, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hf !=  INVALID_HANDLE_VALUE)
		{
			//Write EEPROM File
			retVal = WriteFile(hf , &m_EEPROMData, EEPROM_SIZE, &dwBytesWrote, NULL) == TRUE;
		}
		CloseHandle(hf); 
	}
	return retVal;
}

void XKEEPROM::ReadFromXBOX()
{
	ULONG type;
	ULONG size;
	ExQueryNonVolatileSetting(0xFFFF, &type, &m_EEPROMData, 256, &size);
	m_EncryptedState = TRUE;
}

void XKEEPROM::WriteToXBOX()
{
	ULONG type = 3;
	ULONG size = 256;
	ExSaveNonVolatileSetting(0xFFFF, type, &m_EEPROMData, size);
}

/* Return EEPROM data for this object, Check if it is Encrypted with IsEncrypted() */
void XKEEPROM::GetEEPROMData(LPEEPROMDATA pEEPROMData)
{
	memcpy(pEEPROMData, &m_EEPROMData, EEPROM_SIZE);
}

/* Set a Decrypted EEPROM image as EEPROM data for this object */
void XKEEPROM::SetDecryptedEEPROMData(XBOX_VERSION Version, LPEEPROMDATA pEEPROMData)
{
	memcpy(&m_EEPROMData, pEEPROMData, EEPROM_SIZE);
	m_EncryptedState = FALSE;
	m_XBOX_Version = Version;
}

/* Set a encrypted EEPROM image as EEPROM data for this object */
void XKEEPROM::SetEncryptedEEPROMData(LPEEPROMDATA pEEPROMData)
{
	memcpy(&m_EEPROMData, pEEPROMData, EEPROM_SIZE);
	m_EncryptedState = TRUE;
}

/* Get current detected XBOX Version for this EEPROM */
XBOX_VERSION XKEEPROM::GetXBOXVersion()
{
	return m_XBOX_Version;
}

/* Get confounder in the form of BYTES in  a  Hex String representation */
void XKEEPROM::GetConfounderString(LPSTR Confounder, LPDWORD Length)
{
	DWORD len = CONFOUNDER_SIZE;
     UCHAR Confounder2[0x8] = { 0x4c,0x70,0x33,0xcb,0x5b,0xb5,0x97,0xd2 };
	//Check if this is currently an encrypted image.. if it is, then decrypt it first.. 
	BOOL oldEncryptedState = m_EncryptedState;
	if (m_EncryptedState)
		Decrypt();

		XKGeneral::BytesToHexStr((LPBYTE)&m_EEPROMData.Confounder/*Confounder2*/, len, Confounder);
		*Length = len;

	//Check if this is was an encrypted image.. if it was, then re-encrypt it.. 
	if (oldEncryptedState)
		EncryptAndCalculateCRC();
}

/* Set Confounder in the form of BYTES in  a  Hex String representation */
void XKEEPROM::SetConfounderString(LPCSTR Confounder)
{
	const char* Confounder2 = "4c7033cb5bb597d2";
	DWORD len = CONFOUNDER_SIZE * 2;
	BYTE tmpData[(CONFOUNDER_SIZE * 2) + 1];
	ZeroMemory(tmpData, len + 1);
	memcpy(tmpData, Confounder2, min(strlen(Confounder2), len));

	XKGeneral::HexStrToBytes(tmpData, &len, TRUE);

	//Check if this is currently an encrypted image.. if it is, then decrypt it first.. 
	BOOL oldEncryptedState = m_EncryptedState;
	if (m_EncryptedState)
		Decrypt();

		memcpy(&m_EEPROMData.Confounder, tmpData, CONFOUNDER_SIZE);

	//Check if this is was an encrypted image.. if it was, then re-encrypt it.. 
	if (oldEncryptedState)
		EncryptAndCalculateCRC();
}
	
/* Set HDD Key in the form of BYTES in  a  Hex String representation */
void XKEEPROM::GetHDDKeyString(LPSTR HDDKey, LPDWORD Length)
{
	DWORD len = HDDKEY_SIZE;

	//Check if this is currently an encrypted image.. if it is, then decrypt it first.. 
	BOOL oldEncryptedState = m_EncryptedState;
	if (m_EncryptedState)
		Decrypt();

		XKGeneral::BytesToHexStr((LPBYTE)&m_EEPROMData.HDDKkey, len, HDDKey);
		*Length = len;

	//Check if this is was an encrypted image.. if it was, then re-encrypt it.. 
	if (oldEncryptedState)
		EncryptAndCalculateCRC();

}

/* Set HDD Key in the form of BYTES in  a  Hex String representation */
void XKEEPROM::SetHDDKeyString(LPCSTR HDDKey)
{
	DWORD len = HDDKEY_SIZE * 2;
	BYTE tmpData[(HDDKEY_SIZE * 2) + 1];
	ZeroMemory(tmpData, len + 1);
	memcpy(tmpData, HDDKey, min(strlen(HDDKey), len));

	XKGeneral::HexStrToBytes(tmpData, &len, TRUE);

	//Check if this is currently an encrypted image.. if it is, then decrypt it first.. 
	BOOL oldEncryptedState = m_EncryptedState;
	if (m_EncryptedState)
		Decrypt();

		memcpy(&m_EEPROMData.HDDKkey, tmpData, HDDKEY_SIZE);

	//Check if this is was an encrypted image.. if it was, then re-encrypt it.. 
	if (oldEncryptedState)
		EncryptAndCalculateCRC();

}


/* Get XBE Region in the form of BYTES in  a  Hex String representation */
void XKEEPROM::GetXBERegionString(LPSTR XBERegion, LPDWORD Length)
{
	DWORD len = XBEREGION_SIZE;

	//Check if this is currently an encrypted image.. if it is, then decrypt it first.. 
	BOOL oldEncryptedState = m_EncryptedState;
	if (m_EncryptedState)
		Decrypt();

		XKGeneral::BytesToHexStr((LPBYTE)&m_EEPROMData.XBERegion, len, XBERegion);
		*Length = len;

	//Check if this is was an encrypted image.. if it was, then re-encrypt it.. 
	if (oldEncryptedState)
		EncryptAndCalculateCRC();
}

/* Set XBE Region in the form of BYTES in  a  Hex String representation */
void XKEEPROM::SetXBERegionString(LPCSTR XBERegion)
{
	DWORD len = XBEREGION_SIZE * 2;
	BYTE tmpData[(XBEREGION_SIZE * 2) + 1];
	ZeroMemory(tmpData, len + 1);
	memcpy(tmpData, XBERegion, min(strlen(XBERegion), len));

	XKGeneral::HexStrToBytes(tmpData, &len, TRUE);

	//Check if this is currently an encrypted image.. if it is, then decrypt it first.. 
	BOOL oldEncryptedState = m_EncryptedState;
	if (m_EncryptedState)
		Decrypt();

		memcpy(&m_EEPROMData.XBERegion, tmpData, XBEREGION_SIZE);

	//Check if this is was an encrypted image.. if it was, then re-encrypt it.. 
	if (oldEncryptedState)
		EncryptAndCalculateCRC();
}

/* Set XBE Region using Enum Value */
void XKEEPROM::SetXBERegionVal(XBE_REGION RegionVal)
{
	//Check if this is currently an encrypted image.. if it is, then decrypt it first.. 
	BOOL oldEncryptedState = m_EncryptedState;
	if (m_EncryptedState)
		Decrypt();

		switch (RegionVal)
		{
			case(NORTH_AMERICA):
			case(JAPAN):
			case(EURO_AUSTRALIA):
			{
				m_EEPROMData.XBERegion[0] = RegionVal; //Only use first byte of Region...
				break;
			}
			default:
				m_EEPROMData.XBERegion[0] = NORTH_AMERICA; //If invalid,use Default of US
		}

	//Check if this is was an encrypted image.. if it was, then re-encrypt it.. 
	if (oldEncryptedState)
		EncryptAndCalculateCRC();
}

/* Get XBE Region as Enum Value */
XBE_REGION XKEEPROM::GetXBERegionVal()
{
	//Check if this is currently an encrypted image.. if it is, then decrypt it first.. 
	BOOL oldEncryptedState = m_EncryptedState;
	if (m_EncryptedState)
		Decrypt();

		XBE_REGION retVal = XBE_REGION (m_EEPROMData.XBERegion[0]);

	//Check if this is was an encrypted image.. if it was, then re-encrypt it.. 
	if (oldEncryptedState)
		EncryptAndCalculateCRC();

	return retVal;
}

/* Get Serial Number in the form of Decimal String representation */
char* XKEEPROM::GetSerialNumberString()
{
	char serial[13];
	memcpy(serial, &m_EEPROMData.SerialNumber[0], 12);
	serial[12]='\0';
	return strdup(serial);
}

/* Set Serial Number in the form of Decimal String representation */
void XKEEPROM::SetSerialNumberString(const char* SerialNumber)
{
	DWORD len = SERIALNUMBER_SIZE;
	strncpy((LPSTR)&m_EEPROMData.SerialNumber, SerialNumber, len);

	CalculateChecksum2();
}

/* Get MAC Address in the form of BYTES in  a  Hex String representation */
char* XKEEPROM::GetMACAddressString()
{
	return stringUtility::formatString("%02X%02X%02X%02X%02X%02X", m_EEPROMData.MACAddress[0], m_EEPROMData.MACAddress[1], m_EEPROMData.MACAddress[2], m_EEPROMData.MACAddress[3], m_EEPROMData.MACAddress[4], m_EEPROMData.MACAddress[5]);
}

/* Set MAC Address in the form of BYTES in  a  Hex String representation */
void XKEEPROM::SetMACAddressString(const char* MACAddress)
{
	DWORD len = MACADDRESS_SIZE * 3;
	BYTE tmpData[(HDDKEY_SIZE * 3) + 1];
	ZeroMemory(tmpData, len + 1);
	memcpy(tmpData, MACAddress, min(strlen(MACAddress), len));

	XKGeneral::HexStrToBytes(tmpData, &len, TRUE);

	memcpy(&m_EEPROMData.MACAddress, tmpData, MACADDRESS_SIZE);

	CalculateChecksum2();

}

/* Get Online Key in the form of BYTES in  a  Hex String representation */
void XKEEPROM::GetOnlineKeyString(LPSTR OnlineKey, LPDWORD Length)
{
	DWORD len = ONLINEKEY_SIZE;

	XKGeneral::BytesToHexStr((LPBYTE)&m_EEPROMData.OnlineKey, len, OnlineKey);
	*Length = len;

}


/* Set Online Key in the form of BYTES in  a  Hex String representation */
void XKEEPROM::SetOnlineKeyString(LPCSTR OnlineKey)
{
	DWORD len = ONLINEKEY_SIZE * 2;
	BYTE tmpData[(HDDKEY_SIZE * 2) + 1];
	ZeroMemory(tmpData, len + 1);
	memcpy(tmpData, OnlineKey, min(strlen(OnlineKey), len));

	XKGeneral::HexStrToBytes(tmpData, &len, TRUE);

	memcpy(&m_EEPROMData.OnlineKey, tmpData, ONLINEKEY_SIZE);

	CalculateChecksum2();
}


/*Set Video Standard with Enum */
void XKEEPROM::SetDVDRegionVal(unsigned int ZoneVal)
{
	unsigned int* Zone = ((unsigned int*)&m_EEPROMData.DVDPlaybackKitZone);	
	*Zone = ZoneVal;
	CalculateChecksum3();
}


/*Get DVD Region as Enum */
unsigned int XKEEPROM::GetDVDRegionVal()
{
	uint32_t retVal = *(uint32_t*)&m_EEPROMData.DVDPlaybackKitZone;	
	return retVal;
}


void XKEEPROM::SetAudioFlagsVal(unsigned int AudioFlagsVal)
{
	unsigned int* AudioFlags = ((unsigned int*)&m_EEPROMData.AudioFlags);	
	*AudioFlags = (AudioFlagsVal & EEPROM_AUDIO_FLAGS_MASK);
	CalculateChecksum3();
}

/*Get Video Standard as Enum */
unsigned int XKEEPROM::GetAudioFlagsVal()
{
	uint32_t retVal = *(uint32_t*)&m_EEPROMData.AudioFlags;	
	return retVal;
}

/* Set Video Standard with Enum */
void XKEEPROM::SetVideoFlagsVal(unsigned int VideoFlagsVal)
{
	unsigned int* VideoFlags = ((unsigned int*)&m_EEPROMData.VideoFlags);	
	*VideoFlags = (VideoFlagsVal & EEPROM_VIDEO_FLAGS_MASK);
	CalculateChecksum3();
}

/*Get Video Standard as Enum */
unsigned int XKEEPROM::GetVideoFlagsVal()
{
	uint32_t retVal = *(uint32_t*)&m_EEPROMData.VideoFlags;	
	return retVal;
}


/* Set Video Standard with Enum */
void XKEEPROM::SetVideoStandardVal(unsigned int StandardVal)
{
	unsigned int* VidStandard = ((unsigned int*)&m_EEPROMData.VideoStandard);	
	*VidStandard = StandardVal;
	CalculateChecksum2();
}

/*Get Video Standard as Enum */
unsigned int XKEEPROM::GetVideoStandardVal()
{
	uint32_t retVal = *(uint32_t*)&m_EEPROMData.VideoStandard;	
	return retVal;
}


/* Encrypt the EEPROM Data for Specific XBOX Version by means of the SHA1 Middle Message hack..*/
BOOL XKEEPROM::EncryptAndCalculateCRC(XBOX_VERSION XBOXVersion)
{
	if (!m_EncryptedState)
	{
		m_XBOX_Version = XBOXVersion;
		return EncryptAndCalculateCRC();
	}
	else return FALSE;
}

/*Encrypt with Current XBOX version by means of the SHA1 Middle Message hack..*/
BOOL XKEEPROM::EncryptAndCalculateCRC()
{
	BOOL retVal = FALSE;
	UCHAR key_hash[20];					//rc4 key initializer

	XKRC4 RC4Obj;
	XKSHA1 SHA1Obj;
    
	if (((m_XBOX_Version == V1_0) ||(m_XBOX_Version == V1_1)||(m_XBOX_Version == V1_6)) && (!m_EncryptedState))
	{
		//clear and re-create data_hash from decrypted data
		ZeroMemory(&m_EEPROMData.HMAC_SHA1_Hash, 20);
		SHA1Obj.XBOX_HMAC_SHA1(m_XBOX_Version, (UCHAR*)&m_EEPROMData.HMAC_SHA1_Hash, &m_EEPROMData.Confounder, 8, &m_EEPROMData.HDDKkey, 20, NULL);

		//calculate rc4 key initializer data from eeprom key and data_hash
		SHA1Obj.XBOX_HMAC_SHA1(m_XBOX_Version, key_hash, &m_EEPROMData.HMAC_SHA1_Hash, 20, NULL);
		
		XKRC4::RC4KEY  RC4_key;

		//initialize RC4 key
		RC4Obj.InitRC4Key(key_hash, 20, &RC4_key);
		
		//Encrypt data (in eeprom) with generated key
		RC4Obj.RC4EnDecrypt ((UCHAR*)&m_EEPROMData.Confounder, 8, &RC4_key);		
		RC4Obj.RC4EnDecrypt ((UCHAR*)&m_EEPROMData.HDDKkey, 20, &RC4_key);		
		
		CalculateChecksum2();
		CalculateChecksum3();

		m_EncryptedState = TRUE;
		retVal = TRUE;
	}
	else
	{
		retVal = FALSE;
		m_XBOX_Version = V_NONE;
	}

	return retVal;

}

/*Decrypt EEPROM using auto-detect by means of the SHA1 Middle Message hack..*/
BOOL XKEEPROM::Decrypt()
{
	BOOL retVal = FALSE;
	UCHAR key_hash[20];					//rc4 key initializer
	UCHAR data_hash_confirm[20];		//20 bytes
	UCHAR XBOX_Version = V1_0;
	
	XKRC4 RC4Obj;
	XKRC4::RC4KEY  RC4_key;
	XKSHA1 SHA1Obj;
	UCHAR eepData[0x30];
	//struct rc4_key RC4_key;
	//Keep the Original Data, incase the function fails we can restore it..
	ZeroMemory(eepData, 0x30);
	memcpy(eepData, &m_EEPROMData, 0x30);
    UCHAR Confounder[0x8] = { 0x4c,0x70,0x33,0xcb,0x5b,0xb5,0x97,0xd2 };
	
	  while (((XBOX_Version < 13) && (!retVal)) && m_EncryptedState)  
	  {
        ZeroMemory(key_hash, 20);
		ZeroMemory(data_hash_confirm, 20);
        memset(&RC4_key,0,sizeof(RC4_key));
		//calculate rc4 key initializer data from eeprom key and data_hash
		SHA1Obj.XBOX_HMAC_SHA1(XBOX_Version, key_hash, &m_EEPROMData.HMAC_SHA1_Hash, 20, NULL);

		//initialize RC4 key
		RC4Obj.InitRC4Key(key_hash, 20, &RC4_key);
		
		//decrypt data (from eeprom) with generated key
		RC4Obj.RC4EnDecrypt ((UCHAR*)&m_EEPROMData.Confounder, 8, &RC4_key);		
		RC4Obj.RC4EnDecrypt ((UCHAR*)&m_EEPROMData.HDDKkey, 20, &RC4_key);			
				
		//re-create data_hash from decrypted data
		SHA1Obj.XBOX_HMAC_SHA1(XBOX_Version, data_hash_confirm, &m_EEPROMData.Confounder, 8, &m_EEPROMData.HDDKkey, 20, NULL);

		//ensure retrieved data_hash matches regenerated data_hash_confirm
		if (strncmp((const char*)&m_EEPROMData.HMAC_SHA1_Hash,(const char*)&data_hash_confirm[0],0x14)) 
		{
			//error: hash stored in eeprom[0:19] does not match
			//hash of data which should have been used to generate it.
			
			//The Key used was wrong.. Restore the Data back to original
			ZeroMemory(&m_EEPROMData, 0x30);
			memcpy(&m_EEPROMData, eepData, 0x30);

			m_XBOX_Version = V_NONE;
			retVal = FALSE;
			XBOX_Version++;
		}
		else
		{				
			m_XBOX_Version = (XBOX_VERSION)XBOX_Version;
			m_EncryptedState = FALSE;
			retVal = TRUE;
		
		}
   }
	  return retVal;
}


/*Decrypt With Specific RC4 Key, then detect which xbox Version is this key from*/
BOOL XKEEPROM::Decrypt(LPBYTE EEPROM_Key)
{
	BOOL retVal = FALSE;
	UCHAR key_hash[20];					//rc4 key initializer
	UCHAR data_hash_confirm[20];		//20 bytes
	UCHAR XBOX_Version = V1_0;
	
	XKRC4 RC4Obj;
	XKRC4::RC4KEY  RC4_key;
	XKSHA1 SHA1Obj;
	UCHAR eepData[0x30];

	if (m_EncryptedState) //Can only decrypt if currently encrypted..
	{
		//Keep the Original Data, incase the function fails we can restore it..
		ZeroMemory(eepData, 0x30);
		memcpy(eepData, &m_EEPROMData, 0x30);

		ZeroMemory(key_hash, 20);
		ZeroMemory(data_hash_confirm, 20);

		//calculate rc4 key initializer data from eeprom key and data_hash
		SHA1Obj.HMAC_SHA1(key_hash, EEPROM_Key, 16, (UCHAR*)&m_EEPROMData.HMAC_SHA1_Hash, 20, NULL, 0);

		//initialize RC4 key
		RC4Obj.InitRC4Key(key_hash, 20, &RC4_key);
		
		//decrypt data (from eeprom) with generated key
		RC4Obj.RC4EnDecrypt ((UCHAR*)&m_EEPROMData.Confounder, 8, &RC4_key);		
		RC4Obj.RC4EnDecrypt ((UCHAR*)&m_EEPROMData.HDDKkey, 20, &RC4_key);			
				
		//re-create data_hash from decrypted data
		SHA1Obj.HMAC_SHA1(data_hash_confirm, EEPROM_Key, 16, (UCHAR*)&m_EEPROMData.Confounder, 8, (UCHAR*)&m_EEPROMData.HDDKkey, 20);

		//ensure retrieved data_hash matches regenerated data_hash_confirm
		if (strncmp((const char*)&m_EEPROMData.HMAC_SHA1_Hash,(const char*)&data_hash_confirm[0],0x14)) 
		{
			//error: hash stored in eeprom[0:19] does not match
			//hash of data which should have been used to generate it.
			
			//The Key used was wrong.. Restore the Data back to original
			ZeroMemory(&m_EEPROMData, 0x30);
			memcpy(&m_EEPROMData, eepData, 0x30);

			m_XBOX_Version = V_NONE;
			retVal = FALSE;
		}
		else 
		{
			//Key supplied is indeed correct for this eeprom..
			//Now detect Version by simply decrypting again with auto detect..
			ZeroMemory(&m_EEPROMData, 0x30);
			memcpy(&m_EEPROMData, eepData, 0x30);
			m_XBOX_Version = V_NONE;
			retVal = Decrypt();
		}
	}
	return retVal;
}

//Encapsulated variable..
BOOL XKEEPROM::IsEncrypted()
{
	return m_EncryptedState;
}

//Calculate Checksum2
void XKEEPROM::CalculateChecksum2()
{
	//Calculate CRC for Serial, Mac, OnlineKey, video region
	XKCRC::QuickCRC(m_EEPROMData.Checksum2, m_EEPROMData.SerialNumber, 0x2C);
}

//Calculate Checksum3
void XKEEPROM::CalculateChecksum3()
{
	//calculate CRC's for time zones, time standards, language, dvd region etc.
	XKCRC::QuickCRC(m_EEPROMData.Checksum3, m_EEPROMData.TimeZoneBias, 0x5C);
}

