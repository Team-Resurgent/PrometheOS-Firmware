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
**	     XKGENERAL.CPP - General Utility and Helper function Implementation
********************************************************************************************************
**
**	This Class contains mostly static helper functions to do all kinds of data/type conversions
**	and read from INI files etc..  This is a work in progress and will continue to grow..
**
********************************************************************************************************

UPDATE LOG:
--------------------------------------------------------------------------------------------------------
Date: 02/18/2003
By: UNDEAD [team-assembly]
Reason: Prepared 0.2 for Public Release
--------------------------------------------------------------------------------------------------------
Date: 01/08/2003
By: UNDEAD [team-assembly]
Reason: Fixed two Memory leaks  - Thanks to tommyhl for spotting it!!!
--------------------------------------------------------------------------------------------------------
Date: 01/06/2003
By: UNDEAD [team-assembly]
Reason: Prepared for Public Release
--------------------------------------------------------------------------------------------------------

*/

#include <stdio.h>
#include "xkgeneral.h"

XKGeneral::XKGeneral(void) {
}

XKGeneral::~XKGeneral(void) {
}

// This Funciton converts BYTES to a String representation of the HEX value of the Bytes
void XKGeneral::BytesToHexStr(LPBYTE SrcBytes, DWORD byteCount, LPSTR DstString) {
    BytesToHexStr(SrcBytes, byteCount, DstString, 0x00);
}

// This Funciton converts BYTES to a String representation of the HEX value of the Bytes
// Includes a paramter to specify a seperator or Field delimiter between Hex BYTE values
void XKGeneral::BytesToHexStr(LPBYTE SrcBytes, DWORD byteCount, LPSTR DstString, UCHAR Seperator) {
    USHORT Inc = (Seperator == 0x00) ? 2 : 3;

    for (ULONG i = 0; i < byteCount; i++) {
        if ((UCHAR) * (SrcBytes + i) > 0x0F) {
            itoa((UCHAR) * (SrcBytes + i), DstString + (i * Inc), 16);
        } else {
            *(DstString + i * Inc) = '0';
            itoa((UCHAR) * (SrcBytes + i), DstString + (i * Inc + 1), 16);
        }
    }

    if (Seperator != 0x00) {
        for (ULONG i = 1; i < byteCount; i++)
            *(DstString + i * Inc - 1) = Seperator;
    }
}

// This Function converts a String that Contains anyting to into
// a String that containts ONLY valid numerical values..
// Specify the base, eg. Hex = 16, Decimal=10 etc.
// RemoveInvalid indacates if invalid values should be Zero or removed..
void XKGeneral::MixedStrToDecStr(LPSTR StringData, LPDWORD StrLen, CHAR Base, BOOL RemoveInvalid) {
    UCHAR DecChars[16] = HEX_VALUES;

    DWORD dwSize = *StrLen;
    DWORD currentOffset = 0;
    LPSTR sData = new CHAR[dwSize + 1];
    ZeroMemory(sData, dwSize + 1);
    strncpy(sData, StringData, dwSize);

    if (!RemoveInvalid)
        memset(StringData, '0', dwSize);
    else
        ZeroMemory(StringData, dwSize);

    strupr(sData);

    for (DWORD i = 0; i < dwSize; i++) {
        LPSTR ChrOffset = strchr((LPCSTR)&DecChars, *(sData + i));
        if ((ChrOffset != NULL) && ((ChrOffset - (LPSTR)&DecChars) <= Base - 1)) {
            memcpy(StringData + currentOffset, ChrOffset, 1);
            currentOffset++;
        } else if (!RemoveInvalid) {
            currentOffset++;
        }
    }

    delete[] sData;
    *StrLen = (DWORD)strlen(StringData);
}

DWORD XKGeneral::HexStrToDWORD(LPBYTE StringData, LPDWORD pBufferLen, BOOL RemoveInvalid, BOOL FlipByteOrder) {
    DWORD retVal = 0;
    BYTE temp[4];

    HexStrToBytes(StringData, pBufferLen, RemoveInvalid);

    if (FlipByteOrder) {
        memcpy(temp, StringData, 4);
        StringData[3] = temp[0];
        StringData[2] = temp[1];
        StringData[1] = temp[2];
        StringData[0] = temp[3];
    }

    retVal = *((LPDWORD)StringData);
    return retVal;
}

// This Funciton converts String representation of the HEX value into BYTES
// RemoveInvalid indacates if invalid values should be Zero or removed..
void XKGeneral::HexStrToBytes(LPBYTE StringData, LPDWORD pBufferLen, BOOL RemoveInvalid) {
    UCHAR retVal = 0x00;

    DWORD dwSize = *pBufferLen;
    LPSTR sData = new CHAR[dwSize + 1];

    ZeroMemory(sData, dwSize + 1);
    strncpy(sData, (LPCSTR)StringData, dwSize);
    ZeroMemory(StringData, dwSize);

    MixedStrToDecStr(sData, &dwSize, 16, RemoveInvalid);

    for (DWORD i = 0; i < dwSize / 2; i++) {
        UCHAR tmpstr[3];
        ZeroMemory(tmpstr, 3);
        memcpy(tmpstr, (LPCSTR)(sData + (i * 2)), 2);
        memset(StringData + i, (UCHAR)strtol((LPCSTR)&tmpstr, NULL, 16), 1);
    }

    delete[] sData;
    *pBufferLen = dwSize / 2;
}

// Strip Carriage Return Characters (10,13) From String
void XKGeneral::StripCarriageReturn(LPSTR sString) {
    for (int iInd = 0; iInd < (int)strlen(sString); iInd++) {
        if (sString[iInd] == 10 || sString[iInd] == 13) {
            sString[iInd] = 0;
            break;
        }
    }
}

// Strip Space Characters On Left And Right From String
void XKGeneral::StripSpaces(LPSTR sString) {
    StripLeftSpaces(sString);
    StripRightSpaces(sString);
}

// Strip Space Characters On Left From String
void XKGeneral::StripLeftSpaces(LPSTR sString) {
    char* pcTmp = NULL;
    int iInd;

    for (iInd = 0; iInd < (int)strlen(sString); iInd++) {
        if (sString[iInd] != ' ') {
            pcTmp = &sString[iInd];
            break;
        }
    }
    if (pcTmp)
        strcpy(sString, pcTmp);
}

// Strip Space Characters On Right From String
void XKGeneral::StripRightSpaces(LPSTR sString) {
    int iInd;

    for (iInd = strlen(sString) - 1; iInd >= 0; iInd--) {
        if (sString[iInd] != ' ') {
            sString[++iInd] = 0;
            break;
        }
    }
}

// Strip quotes from mostly File Paths..  hence FILENAME_MAX
void XKGeneral::StripQuotes(LPSTR sString, LPDWORD strLen) {
    StripEnclosedChars(sString, strLen, '\"');
}

// This Function simply strips out Characters from a enclosed string..
// Especially if reading from INI file and value is enlcosed with quotes..
void XKGeneral::StripEnclosedChars(LPSTR sString, LPDWORD strLen, CHAR EncloseChar) {
    UCHAR tmpString[FILENAME_MAX]; // Work off Max Filename.. should be big enough..
    ZeroMemory(tmpString, FILENAME_MAX);

    // get string to work with..
    strcpy((LPSTR)tmpString, sString);
    ZeroMemory(sString, *strLen);

    LPSTR tmpFirst = strchr((LPCSTR)tmpString, EncloseChar);
    LPSTR tmpLast = strrchr((LPCSTR)tmpString, EncloseChar);

    // check if Null, if null then just beginning of string..
    tmpFirst = (tmpFirst != NULL) ? tmpFirst + 1 : (LPSTR)tmpString;

    // check if Null, if null then just use end of string..
    tmpLast = (tmpLast != NULL) ? tmpLast : (LPSTR)tmpString + *strLen;

    *strLen = (LONG)(tmpLast - tmpFirst);

    if (tmpFirst != NULL)
        strncpy(sString, tmpFirst, *strLen);
}

// Modify Lower Characters Into Upper Characters Into String
void XKGeneral::ToUpperString(LPSTR sString) {
    int iInd;

    for (iInd = 0; iInd < (int)strlen(sString); iInd++) {
        if (sString[iInd] >= 'a' && sString[iInd] <= 'z')
            sString[iInd] = toupper(sString[iInd]);
        else
            sString[iInd] = sString[iInd];
    }
}

// Modify Upper Characters Into Lower Characters Into String
void XKGeneral::ToLowerString(LPSTR sString) {
    int iInd;

    for (iInd = 0; iInd < (int)strlen(sString); iInd++) {
        if (sString[iInd] >= 'A' && sString[iInd] <= 'Z')
            sString[iInd] = tolower(sString[iInd]);
        else
            sString[iInd] = sString[iInd];
    }
}
