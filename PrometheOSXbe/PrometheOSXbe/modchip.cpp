#include "modchip.h"
#include "xboxInternals.h"
#include "globalTypes.h"

uint8_t modchip::inputByte(uint16_t port) 
{
	unsigned char value;
	__asm {
		mov dx, port
		in al, dx
		mov value, al
	}
	return value;
}

void modchip::outputByte(uint16_t port, uint8_t value) 
{
	__asm {
		mov     dx, port
		mov     al, value
		out dx, al
	}
}

modchipType modchip::detectModchip()
{
	//int x = inputByte(0xf701);
	//if (inputByte(0xEE) == 0x55)
	//{
	//	uint8_t currentBank = inputByte(0xef);
	//	outputByte(0xef, 0x0B);
	//	bool isXtremium = (inputByte(0xef) & 0x0c) == 0x0c;
	//	outputByte(0xef, currentBank);
	//	return isXtremium ? modchipTypeXtremium : modchipTypeXenium;
	//}
	if (inputByte(0xf500) == 0xE1)
	{
		return modchipTypeXecuter;
	}
	//if (inputByte(0xf701) == 0xf1 || inputByte(0xf701) == 0xf2 || inputByte(0xf701) == 0xf8) 
	//{
	//	return modchipTypeSmartxx;
	//}
	if (inputByte(0xdead) == 0xaf) 
	{
		return modchipTypeModxo;
	}
	if (inputByte(0xf701) == 0x11) // Lattice Based
	{
		return modchipTypeAladdin1mb;
	}
	if (inputByte(0xf701) == 0x15) // Xilinx Based
	{
		return modchipTypeAladdin1mb;
	}
	if (inputByte(0xf701) == 0x69) // Latice Based
	{
		return modchipTypeAladdin2mb;
	}
	if (inputByte(0x1912) != 0xff)
	{
		return modchipTypeXchanger;
	}
	// Just default to xenium if unknown
	return modchipTypeXenium;
}