#include "expansion.h"
#include "lpcWindow.h"

#include <conio.h>

void expansion::wait_ready(uint8_t param)
{
	while (true)
	{
		unsigned char ready = lpcWindow::receiveCustomCommandData(param);
		if (ready == 1)
		{
			break;
		}
		Sleep(100);
	}
}

void expansion::open()
{
	lpcWindow::sendCommandData(LPC_MEM_WIN_CMD_WIN_TYPE, LPC_MEM_WIN_TYPE_EXPANSION);
	lpcWindow::sendCommandData(LPC_MEM_WIN_CMD_CUR_WIN_ID, 0);
	lpcWindow::sendLong(0xff000000);
	lpcWindow::sendCommandData(LPC_MEM_WIN_CMD_WIN_BASE_ADDR, 0);
	lpcWindow::sendLong(0x01000000);
	lpcWindow::sendCommandData(LPC_MEM_WIN_CMD_WIN_LENGTH, 0);
	lpcWindow::sendCommandData(LPC_MEM_WIN_CMD_WIN_FLAGS, LPC_MEM_WIN_FLAG_READ_ENABLED | LPC_MEM_WIN_FLAG_WRITE_ENABLED);
	lpcWindow::sendCommandData(LPC_MEM_WIN_CMD_ENABLE, 1);
}

void expansion::close()
{
	lpcWindow::sendCommandData(LPC_MEM_WIN_CMD_WIN_FLAGS, 0);
	lpcWindow::sendCommandData(LPC_MEM_WIN_CMD_ENABLE, 0);
}

bool expansion::i2c_device_exists(uint8_t address)
{
	lpcWindow::sendCustomCommandData(EXPANSION_COMMAND_REQUEST_I2C_ADDRESS_EXIST, address);
	wait_ready(EXPANSION_COMMAND_RESPONSE_I2C_ADDRESS_EXIST_READY);
	unsigned char result = lpcWindow::receiveCustomCommandData(EXPANSION_COMMAND_RESPONSE_I2C_ADDRESS_EXIST_RESULT);
	return result == EXPANSION_RESULT_OK;
}

bool expansion::send_data(uint8_t address, void* payload, uint16_t payload_length)
{
	lpcWindow::sendCustomCommandData(EXPANSION_COMMAND_SET_PAYLOAD_TYPE, EXPANSION_PAYLOAD_TYPE_OUTGOING);

	if (payload_length >= EXPANSION_MAX_PAYLOAD) 
	{
		payload_length = EXPANSION_MAX_PAYLOAD;
	}
	lpcWindow::sendLong(payload_length);
		
	volatile unsigned char* lpcMemMap = (volatile unsigned char*)LPC_MEMORY_BASE;
	if (payload_length > 0) 
	{
		memcpy((void*)lpcMemMap, payload, payload_length);
	}

	lpcWindow::sendCustomCommandData(EXPANSION_COMMAND_REQUEST_SEND_RECIEVE, address);

	wait_ready(EXPANSION_COMMAND_RESPONSE_SEND_RECIEVE_READY);
	uint8_t sendResult = lpcWindow::receiveCustomCommandData(EXPANSION_COMMAND_RESPONSE_SEND_RESULT);
 	return sendResult == EXPANSION_RESULT_OK;
}

uint8_t* expansion::get_recieved_data(uint32_t* length)
{
	lpcWindow::sendCustomCommandData(EXPANSION_COMMAND_SET_PAYLOAD_TYPE, EXPANSION_PAYLOAD_TYPE_INCOMING);
	uint8_t recieveResult = lpcWindow::receiveCustomCommandData(EXPANSION_COMMAND_RESPONSE_RECIEVE_RESULT);
	if (recieveResult != EXPANSION_RESULT_OK) 
	{
		return NULL;
	}

	if (length != NULL) 
	{
		*length = lpcWindow::readLong();
	}

	volatile uint8_t* lpcMemMap = (volatile uint8_t*)LPC_MEMORY_BASE;
	return (uint8_t*)lpcMemMap;
}

uint8_t expansion::get_device_id(uint8_t address)
{
	uint8_t payload = EXPANSION_COMMAND_REQUEST_DEVICE_ID;
	bool sent = send_data(address, (void*)&payload, sizeof(payload));
	if (!sent) 
	{
		return 0xff;
	}

	uint32_t device_id_length = 0;
	uint8_t* data = get_recieved_data(&device_id_length);
	return device_id_length == 1 ? data[0] : 0xff;
}

char* expansion::get_device_name(uint8_t address)
{
	uint8_t payload = EXPANSION_COMMAND_REQUEST_DEVICE_NAME;
	bool sent = send_data(address, (void*)&payload, sizeof(payload));
	if (!sent) 
	{
		return NULL;
	}

	uint32_t device_name_length = 0;
	uint8_t* data = get_recieved_data(&device_name_length);

	char* device_name = (char*)malloc(device_name_length + 1);
	memcpy(device_name, (void*)data, device_name_length);
	device_name[device_name_length] = 0;
	return device_name;
}

char* expansion::get_device_version(uint8_t address)
{
	uint8_t payload = EXPANSION_COMMAND_REQUEST_DEVICE_VERSION;
	bool sent = send_data(address, (void*)&payload, sizeof(payload));
	if (!sent) 
	{
		return NULL;
	}

	uint32_t device_version_length = 0;
	uint8_t* data = get_recieved_data(&device_version_length);

	char* device_version = (char*)malloc(device_version_length + 1);
	memcpy(device_version, (void*)data, device_version_length);
	device_version[device_version_length] = 0;
	return device_version;
}