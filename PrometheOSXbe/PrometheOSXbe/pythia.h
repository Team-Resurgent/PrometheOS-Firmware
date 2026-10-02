#pragma once

#include "xboxinternals.h"

#define PYTHIA_COMMAND_SET_BRIGHTNESS 0x20
#define PYTHIA_COMMAND_SET_CONTRAST 0x21
#define PYTHIA_COMMAND_CLEAR_DISPLAY 0x22
#define PYTHIA_COMMAND_SET_CURSOR 0x23
#define PYTHIA_COMMAND_WRITE_STRING 0x24

class pythia
{
public:
	static bool set_brightness(uint8_t address, uint8_t brightness);
	static bool set_contrast(uint8_t address, uint8_t contrast);
	static bool clear_display(uint8_t address);
	static bool set_cursor(uint8_t address, uint8_t row, uint8_t col);
	static bool write_string(uint8_t address, const char* message);
};
