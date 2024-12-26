#include "cerbiosIniHelper.h"

#include "xboxInternals.h"
#include "stringUtility.h"
#include "utils.h"
#include "fileSystem.h"

void cerbiosIniHelper::upperCase(char* value)
{
    for (uint32_t i = 0; i < (int)strlen(value); i++) 
	{
        value[i] = (char)(value[i] >= 97 && value[i] <= 122 ? value[i] - 32 : value[i]);
    }
}

void cerbiosIniHelper::trimSpace(char* value)
{
    char* pos = value;
    int length = strlen(pos);

    while (length > 0 && isspace(pos[length - 1])) {
        pos[length - 1] = 0;
        length--;
    }

    while (length > 0 && *pos && isspace(*pos)) {
        ++pos;
        --length;
    }

    memmove(value, pos, length + 1);
}

uint8_t cerbiosIniHelper::parseByte(char* value, uint8_t defaultValue)
{
    char* endPtr;
    uint8_t result;

    upperCase(value);

    if (strncmp(value, "0X", 2) == 0) 
    {
        result = (uint8_t)strtol(value + 2, &endPtr, 16);
    }
    else
    {
        result = (uint8_t)strtol(value, &endPtr, 10);
    }

    if (*endPtr == 0) 
    {
        return result;
    }

    return defaultValue;
}

uint8_t cerbiosIniHelper::parseBoolean(char* value, uint8_t defaultValue)
{
    upperCase(value);
    if (strcmp(value, "TRUE") == 0) {
        return 1;
    }
    else if (strcmp(value, "FALSE") == 0) {
        return 0;
    }
    return defaultValue;
}

void cerbiosIniHelper::parseConfigLine(cerbiosConfig* config, char* param1, char* param2, char* buffer, unsigned long bufferSize)
{
	char* params[2];
    params[0] = param1;
    params[1] = param2;

	uint32_t paramLengths[2];
    paramLengths[0] = 0;
    paramLengths[1] = 0;

    uint32_t paramIndex = 0;
    for (uint32_t i = 0; i < bufferSize; i++) {
        char currentChar = buffer[i];
        if (paramIndex > 1) {
            return;
        }
        else if (currentChar == '=') {
            params[paramIndex][paramLengths[paramIndex]] = 0;
            paramIndex++;
            continue;
        }
        else if (currentChar == ';') {
            break;
        }
        if (paramLengths[paramIndex] > 255) {
            continue;
        }
        params[paramIndex][paramLengths[paramIndex]] = currentChar;
        paramLengths[paramIndex] = paramLengths[paramIndex] + 1;
    }
    params[paramIndex][paramLengths[paramIndex]] = 0;

    if (paramIndex != 1) {
        return;
    }

    trimSpace(params[0]);
    upperCase(params[0]);

    trimSpace(params[1]);

    if (strcmp(params[0], "DRIVESETUP") == 0) {
        config->DriveSetup = parseByte(params[1], config->DriveSetup);
        config->DriveSetup  = min(max(config->DriveSetup , 0), 3);
    }
    else if (strcmp(params[0], "AVCHECK") == 0) {
        config->AVCheck = parseBoolean(params[1], config->AVCheck);
    }
    else if (strcmp(params[0], "DEBUG") == 0) {
        config->Debug = parseBoolean(params[1], config->Debug);
    }
    else if (strcmp(params[0], "CDPATH1") == 0) {        
        strncpy(config->CdPath1, params[1], 99);
    }
    else if (strcmp(params[0], "CDPATH2") == 0) {
        strncpy(config->CdPath2, params[1], 99);
    }
    else if (strcmp(params[0], "CDPATH3") == 0) {
        strncpy(config->CdPath3, params[1], 99);
    }
    else if (strcmp(params[0], "DASHPATH1") == 0) {   
        strncpy(config->DashPath1, params[1], 99);
    }
    else if (strcmp(params[0], "DASHPATH2") == 0) {
        strncpy(config->DashPath2, params[1], 99);        
    }
    else if (strcmp(params[0], "DASHPATH3") == 0) {
        strncpy(config->DashPath3, params[1], 99);
    }
    else if (strcmp(params[0], "BOOTANIMPATH") == 0) {
        strncpy(config->BootAnimPath, params[1], 99);
    }
    else if (strcmp(params[0], "FRONTLED") == 0 && strlen(params[1]) == 4) {
        strncpy(config->FrontLed, params[1], 4);
        upperCase(config->FrontLed);
    }
    else if (strcmp(params[0], "FANSPEED") == 0) {
        config->FanSpeed = parseByte(params[1], config->FanSpeed);
    }
    else if (strcmp(params[0], "UDMAMOE") == 0) {
        config->UdmaModeMaster = parseByte(params[1], config->UdmaModeMaster);
    }
    else if (strcmp(params[0], "UDMAMODEMASTER") == 0) {
        config->UdmaModeMaster = parseByte(params[1], config->UdmaModeMaster);
    }
    else if (strcmp(params[0], "UDMAMODESLAVE") == 0) {
        config->UdmaModeSlave = parseByte(params[1], config->UdmaModeSlave);
    }
    else if (strcmp(params[0], "FORCE480P") == 0) {
        config->Force480p = parseBoolean(params[1], config->Force480p);
    }
    else if (strcmp(params[0], "FORCEVGA") == 0) {
        config->ForceVGA = parseBoolean(params[1], config->ForceVGA);
    }
    else if (strcmp(params[0], "RTCENABLE") == 0) {
        config->RtcEnable = parseBoolean(params[1], config->RtcEnable);
    }
    else if (strcmp(params[0], "BLOCKDASHUPDATE") == 0) {
        config->BlockDashUpdate = parseBoolean(params[1], config->BlockDashUpdate);
    }
    else if (strcmp(params[0], "RESETONEJECT") == 0) {
        config->ResetOnEject = parseBoolean(params[1], config->ResetOnEject);
    }
}

void cerbiosIniHelper::parseConfig(cerbiosConfig* config, utils::dataContainer* configData)
{
    char* lineBuffer = (char*)malloc(1024);
    char* param1 = (char*)lineBuffer + 512;
    char* param2 = (char*)param1 + 256;
    bool skip = FALSE;

    uint32_t lineLength = 0;
    for (uint32_t i = 0; i < configData->size; i++) 
    {
        char currentChar = configData->data[i];
        if (currentChar == '\t') 
        {
            continue;
        }
        else if (currentChar == ';') 
        {
            skip = true;
        }
        else if (currentChar == '\r' || currentChar == '\n') 
        {
            if (lineLength > 0) 
            {
                parseConfigLine(config, param1, param2, lineBuffer, lineLength);
                lineLength = 0;
            }
            skip = false;
            continue;
        }
        else if (lineLength < 512 && skip == FALSE) 
        {
            lineBuffer[lineLength] = currentChar;
            lineLength++;
        }
    }

    if (lineLength > 0) 
    {
        parseConfigLine(config, param1, param2, lineBuffer, lineLength);
    }

    free(lineBuffer);
}

cerbiosConfig cerbiosIniHelper::loadConfig()
{
	// Init default config

	cerbiosConfig config;
	memset(&config, 0, sizeof(config));
	setConfigDefault(&config);

	uint32_t fileHandle;
	if (fileSystem::fileOpen("HDD0-C:\\cerbios.ini", fileSystem::FileModeRead, fileHandle))
	{
		uint32_t fileSize;
		if (fileSystem::fileSize(fileHandle, fileSize))
		{
			char* buffer = (char*)malloc(fileSize);
			uint32_t bytesRead;
			if (fileSystem::fileRead(fileHandle, buffer, fileSize, bytesRead) && bytesRead == fileSize)
			{
				utils::dataContainer* configData;
				configData = new utils::dataContainer(buffer, fileSize, fileSize);
				parseConfig(&config, configData);
				delete(configData);
			}
			free(buffer);
		}
		fileSystem::fileClose(fileHandle);
	}

	config.FanSpeed = min(((config.FanSpeed / 10) * 10), 100);
	config.DriveSetup = min(config.DriveSetup, 3);
	for (uint32_t i = 0; i < 4; i++)
	{
		char value = config.FrontLed[i];
		if (value != 'G' && value != 'R' && value != 'A' && value != 'O')
		{
			config.FrontLed[i] = 'G';
		}
	}
	config.UdmaModeMaster = min(config.UdmaModeMaster, 6);
	config.UdmaModeSlave = min(config.UdmaModeSlave, 6);

	return config;
}

void cerbiosIniHelper::buildConfig(cerbiosConfig* config, char* buffer)
{
	buffer[0] = 0;

	strcat(buffer, "; Cerbios Config\r\n");
	strcat(buffer, "\r\n");

	strcat(buffer, "; Check For AV Pack\r\n");
	strcat(buffer, "AVCheck = ");
	strcat(buffer, config->AVCheck == 1 ? "True" : "False");
	strcat(buffer, "\r\n");
	strcat(buffer, "\r\n");

	strcat(buffer, "; LED Ring Color, G = Green, R = Red, A = Amber, O = Off\r\n");
	strcat(buffer, "FrontLed = ");
	strcat(buffer, config->FrontLed);
	strcat(buffer, "\r\n");
	strcat(buffer, "\r\n");

	strcat(buffer, "; Fan Speed 0 = Default, 10-100 = Manual Control, Supports increments of 2's\r\n");
	strcat(buffer, "FanSpeed = ");
	char* fanSpeed = stringUtility::formatString("%i", config->FanSpeed);
	strcat(buffer, fanSpeed);
	free(fanSpeed);
	strcat(buffer, "\r\n");
	strcat(buffer, "\r\n");

	strcat(buffer, "; Drive Setup\r\n");
	strcat(buffer, "; 0 = HDD & DVD,  1 = HDD & No DVD (Legacy Mode), 2 = HDD & No DVD (Modern Mode), 3 = Dual HDD\r\n");
	strcat(buffer, "DriveSetup = ");
	char* driveSetup = stringUtility::formatString("%i", config->DriveSetup);
	strcat(buffer, driveSetup);
	free(driveSetup);
	strcat(buffer, "\r\n");
	strcat(buffer, "\r\n");

	strcat(buffer, "; Set Master Drive UDMA mode 0-6 on cold-boot\r\n");
	strcat(buffer, "UdmaMode = ");
	char* udmaMode = stringUtility::formatString("%i", config->UdmaModeMaster);
	strcat(buffer, udmaMode);
	strcat(buffer, "\r\n");
	strcat(buffer, "UdmaModeMaster = ");
	strcat(buffer, udmaMode);
	strcat(buffer, "\r\n");
	free(udmaMode);
	strcat(buffer, "\r\n");

	strcat(buffer, "; Set Slave Drive UDMA mode 0-6 on cold-boot (if enabled by DriveSetup = 3)\r\n");
	strcat(buffer, "UdmaModeSlave = ");
	char* udmaModeSlave = stringUtility::formatString("%i", config->UdmaModeSlave);
	strcat(buffer, udmaModeSlave);
	free(udmaModeSlave);
	strcat(buffer, "\r\n");
	strcat(buffer, "\r\n");

	strcat(buffer, "; Enables Automatic Time Sync With Optional RTC Hardware Connected to SMBus\r\n");
	strcat(buffer, "RtcEnable = ");
	strcat(buffer, config->RtcEnable == 1 ? "True" : "False");
	strcat(buffer, "\r\n");
	strcat(buffer, "\r\n");

	strcat(buffer, "; Forces AV Modes That Would Normally Be Rendered At 480i to 480p. Requires 480p Set In MS Dash And Component Cables\r\n");
	strcat(buffer, "Force480p = ");
	strcat(buffer, config->Force480p == 1 ? "True" : "False");
	strcat(buffer, "\r\n");
	strcat(buffer, "\r\n");

	strcat(buffer, "; Forces VGA Modes For Component Cables Or Custom VGA Cables Using Mode(2+3) for VGA Displays Only, This Enables Force480p By Default & Sets Console To NTSC.\r\n");
	strcat(buffer, "ForceVGA = ");
	strcat(buffer, config->ForceVGA == 1 ? "True" : "False");
	strcat(buffer, "\r\n");
	strcat(buffer, "\r\n");

	strcat(buffer, "; Load XDK Launcher/XBDM if it exists (Debug Bios Only)\r\n");
	strcat(buffer, "Debug = ");
	strcat(buffer, config->Debug == 1 ? "True" : "False");
	strcat(buffer, "\r\n");
	strcat(buffer, "\r\n");

	strcat(buffer, "; Blocks Games From Updating The Original Xbox Dashboard, Useful for softmods.\r\n");
	strcat(buffer, "BlockDashUpdate = ");
	strcat(buffer, config->BlockDashUpdate == 1 ? "True" : "False");
	strcat(buffer, "\r\n");
	strcat(buffer, "\r\n");

	strcat(buffer, "; Restore original stock xbox reset functionality when the DVD tray is ejected.\r\n");
	strcat(buffer, "ResetOnEject = ");
	strcat(buffer, config->ResetOnEject == 1 ? "True" : "False");
	strcat(buffer, "\r\n");
	strcat(buffer, "\r\n");

	strcat(buffer, "; CD Paths (always falls back to D:\\default.xbe)\r\n");
	strcat(buffer, "CdPath1 = ");
	strcat(buffer, config->CdPath1);
	strcat(buffer, "\r\n");
	strcat(buffer, "CdPath2 = ");
	strcat(buffer, config->CdPath2);
	strcat(buffer, "\r\n");
	strcat(buffer, "CdPath3 = ");
	strcat(buffer, config->CdPath3);
	strcat(buffer, "\r\n");
	strcat(buffer, "\r\n");

	strcat(buffer, "; Dash Paths (always falls back to C:\\xboxdash.xbe)\r\n");
	strcat(buffer, "DashPath1 = ");
	strcat(buffer, config->DashPath1);
	strcat(buffer, "\r\n");
	strcat(buffer, "DashPath2 = ");
	strcat(buffer, config->DashPath2);
	strcat(buffer, "\r\n");
	strcat(buffer, "DashPath3 = ");
	strcat(buffer, config->DashPath3);
	strcat(buffer, "\r\n");
	strcat(buffer, "\r\n");

	strcat(buffer, "; Boot Animation Path (always falls back to C:\\BootAnims\\Xbox\\bootanim.xbe)\r\n");
	strcat(buffer, "BootAnimPath = ");
	strcat(buffer, config->BootAnimPath);
	strcat(buffer, "\r\n");
}

void cerbiosIniHelper::saveConfig(char* buffer)
{
	uint32_t fileHandle;
	if (fileSystem::fileOpen("HDD0-C:\\cerbios.ini", fileSystem::FileModeWrite, fileHandle))
	{
		uint32_t bytesWritten = 0;
		fileSystem::fileWrite(fileHandle, buffer, strlen(buffer), bytesWritten); 
		fileSystem::fileClose(fileHandle);
	}
}

void cerbiosIniHelper::setConfigDefault(cerbiosConfig* config)
{
	config->DriveSetup = 1;
	config->AVCheck = 1;
	config->Debug = 0;
	strcpy(config->CdPath1, "");
	strcpy(config->CdPath2, "");
	strcpy(config->CdPath3, "");
	strcpy(config->DashPath1, "\\Device\\Harddisk0\\Partition2\\evoxdash.xbe");
	strcpy(config->DashPath2, "\\Device\\Harddisk0\\Partition2\\avalaunch.xbe");
	strcpy(config->DashPath3, "\\Device\\Harddisk0\\Partition2\\nexgen.xbe");
	strcpy(config->BootAnimPath, "\\Device\\Harddisk0\\Partition2\\BootAnims\\Xbox\\bootanim.xbe");
	strcpy(config->FrontLed, "GGGG");
	config->FanSpeed = 0;
	config->UdmaModeMaster = 2;
	config->UdmaModeSlave = 2;
	config->Force480p = 0;
	config->ForceVGA = 0;
	config->RtcEnable = 0;
	config->BlockDashUpdate = 0;
    config->ResetOnEject = 0;
}