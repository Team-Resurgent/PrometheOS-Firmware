#include "modchip.h"
#include "xboxInternals.h"

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
