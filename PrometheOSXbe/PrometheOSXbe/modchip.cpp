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
	if (inputByte(0xf500) == 0xE1)
	{
		return modchipTypeXecuter;
	}
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