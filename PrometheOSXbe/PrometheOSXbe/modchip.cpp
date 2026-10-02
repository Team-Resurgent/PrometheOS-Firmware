#include "modchip.h"
#include "xboxInternals.h"
#include "globalTypes.h"

#include <conio.h>

uint8_t modchip::inputByte(uint16_t port) {
    return (uint8_t)_inp(port);
}

void modchip::outputByte(uint16_t port, uint8_t value) {
    _outp(port, value);
}

modchipType modchip::detectModchip() {
    // Let's do a dummy read to prime X3, otherwise ID commands won't work after a hotswap
    ((volatile uint8_t*)LPC_MEMORY_BASE)[0];

    if (inputByte(0xEE) == 0x55)
    {
    	return modchipTypeXenium;
    }
    if (inputByte(0xf500) == 0xE1) {
        return modchipTypeXecuter;
    }
    if (inputByte(0xdead) == 0xaf) {
        return modchipTypeModxo;
    }

    uint8_t aladdin_smartxx_id = inputByte(0xf701);
    if (aladdin_smartxx_id == 0x11) // Lattice Based
    {
        return modchipTypeAladdin1mb;
    }
    if (aladdin_smartxx_id == 0x15) // Xilinx Based
    {
        return modchipTypeAladdin1mb;
    }
    if (aladdin_smartxx_id == 0x69) // Latice Based
    {
        return modchipTypeAladdin2mb;
    }
    if (aladdin_smartxx_id == 0x79) // Xilinx Based
    {
        return modchipTypeAladdin2mb;
    }

    // aladdin_smartxx_id == 0x9 = 2mb
    aladdin_smartxx_id = aladdin_smartxx_id & 0xf;

    if (aladdin_smartxx_id == 0x5) {
        return modchipTypeSmartxxOpx;
    }

    if (aladdin_smartxx_id == 0x1 || aladdin_smartxx_id == 0x2 || aladdin_smartxx_id == 0x8) {
        return modchipTypeSmartxx;
    }

    if (inputByte(0x1912) != 0xff) {
        return modchipTypeXchanger;
    }

    // Just default to xenium if unknown
    return modchipTypeXenium;
}
