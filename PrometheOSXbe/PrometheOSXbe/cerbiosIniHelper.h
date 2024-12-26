#pragma once

#include "utils.h"
#include "xboxInternals.h"

typedef struct cerbiosConfig 
{
    unsigned char AVCheck;
    unsigned char Debug;
    unsigned char DriveSetup;
    char CdPath1[100];
    char CdPath2[100];
    char CdPath3[100];
    char DashPath1[100];
    char DashPath2[100];
    char DashPath3[100];
    char BootAnimPath[100];
    char FrontLed[5];
    unsigned char FanSpeed;
    unsigned char UdmaModeMaster;
    unsigned char UdmaModeSlave;
    unsigned char Force480p;
    unsigned char ForceVGA;
    unsigned char RtcEnable;
    unsigned char BlockDashUpdate;
    unsigned char ResetOnEject;
} cerbiosConfig;

class cerbiosIniHelper
{
public:
	static cerbiosConfig loadConfig();
	static void buildConfig(cerbiosConfig* config, char* buffer);
	static void saveConfig(char* buffer);
	static void setConfigDefault(cerbiosConfig* config);
private:
	static void upperCase(char* value);
	static void trimSpace(char* value);
	static uint8_t parseByte(char* value, uint8_t defaultValue);
	static uint8_t parseBoolean(char* value, uint8_t defaultValue);
	static void parseConfigLine(cerbiosConfig* config, char* param1, char* param2, char* buffer, unsigned long bufferSize);
	static void parseConfig(cerbiosConfig* config, utils::dataContainer* configData);
};