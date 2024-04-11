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
**	     XKCRC.CPP - General CRC Class' Implementation
********************************************************************************************************
**
**	Containss functions to do a simple CRC check on a block of data..  
**	might be more ways of elegantly doing this, but for now this has to work...
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
#include "XKCRC.h"

XKCRC::XKCRC(void)
{
}

XKCRC::~XKCRC(void)
{
}

void XKCRC::QuickCRC(UCHAR* CRCVAL, UCHAR* inData, DWORD dataLen)
{
	unsigned long high = 0, low = 0;

	for (unsigned long i = 0; i < dataLen / sizeof(unsigned long); i++)
	{
		unsigned long val = ((unsigned long *)inData)[i];
		unsigned long long sum = ((unsigned long long)high << 32) | low;

		high = (unsigned long)((sum + val) >> 32);
		low += val;
	}

	*(unsigned long *)CRCVAL = ~(high + low);
}