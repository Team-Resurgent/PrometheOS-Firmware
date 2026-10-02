#include "pythia.h"

#include "expansion.h"

bool pythia::set_brightness(uint8_t address, uint8_t brightness)
{
	uint8_t payload[2];
	payload[0] = PYTHIA_COMMAND_SET_BRIGHTNESS;
	payload[1] = brightness;

	expansion::open();
	bool sent = expansion::send_data(address, (void*)&payload, sizeof(payload));
	expansion::close();
	return sent;
}

bool pythia::set_contrast(uint8_t address, uint8_t contrast)
{
	uint8_t payload[2];
	payload[0] = PYTHIA_COMMAND_SET_CONTRAST;
	payload[1] = contrast;

	expansion::open();
	bool sent = expansion::send_data(address, (void*)&payload, sizeof(payload));
	expansion::close();
	return sent;
}

bool pythia::clear_display(uint8_t address)
{
	uint8_t payload = PYTHIA_COMMAND_CLEAR_DISPLAY;

	expansion::open();
	bool sent = expansion::send_data(address, (void*)&payload, sizeof(payload));
	expansion::close();
	return sent;
}

bool pythia::set_cursor(uint8_t address, uint8_t row, uint8_t col)
{
	uint8_t payload[3];
	payload[0] = PYTHIA_COMMAND_SET_CURSOR;
	payload[1] = row;
	payload[2] = col;

	expansion::open();
	bool sent = expansion::send_data(address, (void*)&payload, sizeof(payload));
	expansion::close();
	return sent;
}

bool pythia::write_string(uint8_t address, const char* message)
{
	uint8_t message_length = strlen(message);
	uint8_t* payload = (uint8_t*)malloc(message_length + 1);
	payload[0] = PYTHIA_COMMAND_WRITE_STRING;
	memcpy((void*)&payload[1], message, message_length);
	expansion::open();
	bool sent = expansion::send_data(address, payload, message_length + 1);
	expansion::close();
	free(payload);
	return sent;
}