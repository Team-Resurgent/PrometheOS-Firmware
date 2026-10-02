#pragma once

#include "xboxinternals.h"

#define EXPANSION_COMMAND_NONE 0
#define EXPANSION_COMMAND_REQUEST_I2C_ADDRESS_EXIST 1 
#define EXPANSION_COMMAND_RESPONSE_I2C_ADDRESS_EXIST_READY 2
#define EXPANSION_COMMAND_RESPONSE_I2C_ADDRESS_EXIST_RESULT 3
#define EXPANSION_COMMAND_REQUEST_SEND_RECIEVE 4 // Set current long to length
#define EXPANSION_COMMAND_RESPONSE_SEND_RECIEVE_READY 5 
#define EXPANSION_COMMAND_RESPONSE_SEND_RESULT 6 
#define EXPANSION_COMMAND_RESPONSE_RECIEVE_RESULT 7 // Sets current long to length
#define EXPANSION_COMMAND_SET_PAYLOAD_TYPE 8

#define EXPANSION_RESULT_IDLE 0 
#define EXPANSION_RESULT_OK 1
#define EXPANSION_RESULT_TIMEOUT 2
#define EXPANSION_RESULT_INVALID_LEN 3

#define EXPANSION_PAYLOAD_TYPE_NONE 0
#define EXPANSION_PAYLOAD_TYPE_INCOMING 1
#define EXPANSION_PAYLOAD_TYPE_OUTGOING 2

#define EXPANSION_COMMAND_REQUEST_NOP 0
#define EXPANSION_COMMAND_REQUEST_DEVICE_ID 1
#define EXPANSION_COMMAND_REQUEST_DEVICE_NAME 2
#define EXPANSION_COMMAND_REQUEST_DEVICE_VERSION 3
#define EXPANSION_COMMAND_SEND_DATA 4

#define EXPANSION_MAX_PAYLOAD 1024

class expansion
{
public:
	static void wait_ready(uint8_t param);
	static void open();
	static void close();
    static bool i2c_device_exists(uint8_t address);
	static bool send_data(uint8_t address, void* payload, uint16_t payload_length);
	static uint8_t* get_recieved_data(uint32_t* length);
    static uint8_t get_device_id(uint8_t address);
    static char* get_device_name(uint8_t address);
	static char* get_device_version(uint8_t address);
};
