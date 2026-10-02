#include "cerbiosIniHelper.h"

#include "xboxInternals.h"
#include "stringUtility.h"
#include "utils.h"
#include "fileSystem.h"

void cerbiosIniHelper::upperCase(char* value) {
    for (uint32_t i = 0; i < (int)strlen(value); i++) {
        value[i] = (char)(value[i] >= 97 && value[i] <= 122 ? value[i] - 32 : value[i]);
    }
}

void cerbiosIniHelper::trimSpace(char* value) {
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

uint8_t cerbiosIniHelper::parseByte(char* value, uint8_t defaultValue) {
    char* endPtr;
    uint8_t result;

    trimSpace(value);
    upperCase(value);

    if (strncmp(value, "0X", 2) == 0) {
        result = (uint8_t)strtol(value + 2, &endPtr, 16);
    } else {
        result = (uint8_t)strtol(value, &endPtr, 10);
    }

    if (*endPtr == 0) {
        return result;
    }

    return defaultValue;
}

uint32_t cerbiosIniHelper::parseLong(char* value, uint32_t defaultValue) {
    char* endPtr;
    uint32_t result;

    upperCase(value);

    if (strncmp(value, "0X", 2) == 0) {
        result = (uint32_t)strtoul(value + 2, &endPtr, 16);
    } else {
        result = (uint32_t)strtoul(value, &endPtr, 10);
    }

    if (*endPtr == 0) {
        return result;
    }

    return defaultValue;
}

uint8_t cerbiosIniHelper::parseBoolean(char* value, uint8_t defaultValue) {
    upperCase(value);
    if (strcmp(value, "TRUE") == 0) {
        return 1;
    } else if (strcmp(value, "FALSE") == 0) {
        return 0;
    }
    return defaultValue;
}

unsigned short cerbiosIniHelper::parseIGR(char* value)
{
    int i;
    unsigned short shift;
    unsigned short igrValue;

    upperCase(value);

    igrValue = 0;
    for (i = 0; i < (int)strlen(value); i++) {
        shift = (value[i] >= 'A') ? (value[i] - 'A' + 10) : (value[i] - '0');
        igrValue = igrValue | (1 << (shift ^ 8));
    }
    return igrValue;
}

char* cerbiosIniHelper::shortToIGR(unsigned short value)
{
    char buf[5];
    int pos = 0;
    for (int i = 0; i < 16 && pos < 4; i++)
    {
        uint16_t shift = (uint16_t)(1u << (i ^ 8));
        if (value & shift)
        {
            unsigned v = (unsigned)i;
            buf[pos++] = (v < 10) ? ('0' + v) : ('A' + (v - 10));
        }
    }
    buf[pos] = '\0';
    return strdup(buf);
}

void cerbiosIniHelper::parseConfigLine(cerbiosConfig* configData, char* param1, char* param2, char* buffer, unsigned long bufferSize)
{
    unsigned long i;
    unsigned int paramIndex;
    unsigned long paramLengths[2];
    char currentChar;
    char* params[2];

    params[0] = param1;
    params[1] = param2;

    paramLengths[0] = 0;
    paramLengths[1] = 0;

    paramIndex = 0;
    for (i = 0; i < bufferSize; i++) {
        currentChar = buffer[i];
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
        configData->DriveSetup = (UCHAR)parseLong(params[1], configData->DriveSetup);
        configData->DriveSetup = min(max(configData->DriveSetup , 0), 3);
    }
    else if (strcmp(params[0], "AVCHECK") == 0) {
        configData->AVCheck = parseBoolean(params[1], configData->AVCheck);
    }
    else if (strcmp(params[0], "DEBUG") == 0) {
        configData->Debug = parseBoolean(params[1], configData->Debug);
    }
    else if (strcmp(params[0], "DASHPATH") == 0 || strcmp(params[0], "DASHPATH1") == 0) {
        strncpy(configData->DashPath, params[1], 99);
    }
    else if (strcmp(params[0], "BOOTANIMPATH") == 0) {
        strncpy(configData->BootAnimPath, params[1], 99);
    }
    else if (strcmp(params[0], "FRONTLED") == 0 && strlen(params[1]) == 4) {
        strncpy(configData->FrontLed, params[1], 4);
        upperCase(configData->FrontLed);
    }
    else if (strcmp(params[0], "OVERRIDEFAN") == 0) {
        configData->OverrideFan = parseBoolean(params[1], configData->OverrideFan);
    }
    else if (strcmp(params[0], "FANSPEED") == 0) {
        configData->FanSpeed = (UCHAR)parseLong(params[1], configData->FanSpeed);
    }
    else if (strcmp(params[0], "UDMAMODEMASTER") == 0) {
        configData->UdmaModeMaster = (UCHAR)parseLong(params[1], configData->UdmaModeMaster);
    }
    else if (strcmp(params[0], "UDMAMODESLAVE") == 0) {
        configData->UdmaModeSlave = (UCHAR)parseLong(params[1], configData->UdmaModeSlave);
    }
    else if (strcmp(params[0], "FORCE480P") == 0) {
        configData->Force480p = parseBoolean(params[1], configData->Force480p);
    }
    else if (strcmp(params[0], "FORCE480I") == 0) {
        configData->Force480i = parseBoolean(params[1], configData->Force480i);
    }
    else if (strcmp(params[0], "FORCEVGA") == 0) {
        configData->ForceVGA = parseBoolean(params[1], configData->ForceVGA);
    }
    else if (strcmp(params[0], "FORCEFFILTER") == 0) {
        configData->ForceFFilter = (UCHAR)parseLong(params[1], configData->ForceFFilter);
        configData->ForceFFilter = min(max(configData->ForceFFilter , 0), 6);
    }
    else if (strcmp(params[0], "INAPPLCDENABLE") == 0) {
        configData->InAppLCDEnable = parseBoolean(params[1], configData->InAppLCDEnable);
    }
    else if (strcmp(params[0], "RTCENABLE") == 0) {
        configData->RTCEnable = parseBoolean(params[1], configData->RTCEnable);
    }
    else if (strcmp(params[0], "AUTOOFFTIMEOUT") == 0) {
        configData->AutoOffTimeout = parseLong(params[1], configData->AutoOffTimeout);
        if (configData->AutoOffTimeout != 0 && configData->AutoOffTimeout < 30) {
            configData->AutoOffTimeout = 30;
        }
    }
    else if (strcmp(params[0], "BLOCKDASHUPDATE") == 0) {
        configData->BlockDashUpdate = parseBoolean(params[1], configData->BlockDashUpdate);
    }
    else if (strcmp(params[0], "XONLINEDASHREDIR") == 0) {
        configData->XonlineDashRedir = parseBoolean(params[1], configData->XonlineDashRedir);
    }
    else if (strcmp(params[0], "RESETONEJECT") == 0) {
        configData->ResetOnEject = parseBoolean(params[1], configData->ResetOnEject);
    }
    else if (strcmp(params[0], "ADVCPUSUPPORT") == 0) {
        configData->AdvCPUSupport = parseBoolean(params[1], configData->AdvCPUSupport);
    }
    else if (strcmp(params[0], "DISABLELIMITMEM") == 0) {
        configData->DisableLimitMem = parseBoolean(params[1], configData->DisableLimitMem);
    }
    else if (strcmp(params[0], "APPLYTITLEPATCHES") == 0) {
        configData->ApplyTitlePatches = parseBoolean(params[1], configData->ApplyTitlePatches);
    }
    else if (strcmp(params[0], "READONLYC") == 0) {
        configData->ReadOnlyC = parseBoolean(params[1], configData->ReadOnlyC);
    }
    else if (strcmp(params[0], "READONLYF") == 0) {
        configData->ReadOnlyF = parseBoolean(params[1], configData->ReadOnlyF);
    }
    else if (strcmp(params[0], "READONLYG") == 0) {
        configData->ReadOnlyG = parseBoolean(params[1], configData->ReadOnlyG);
    }
    else if (strcmp(params[0], "TRFWSUPPORT") == 0) {
        configData->TrFwSupport = parseBoolean(params[1], configData->TrFwSupport);
    }
    else if (strcmp(params[0], "FORCEHDAVPACK") == 0) {
        configData->ForceHDAVPack = parseBoolean(params[1], configData->ForceHDAVPack);
    }
    else if (strcmp(params[0], "ENABLESCREENSHOTS") == 0) {
        configData->EnableScreenshots = parseBoolean(params[1], configData->EnableScreenshots);
    }
    else if (strcmp(params[0], "SCREENSHOTHDD") == 0) {
        configData->ScreenshotHDD = (UCHAR)parseLong(params[1], configData->ScreenshotHDD);
        configData->ScreenshotHDD = min(max(configData->ScreenshotHDD , 0), 1);
    }
    else if (strcmp(params[0], "SCREENSHOTPART") == 0) {
        configData->ScreenshotPart = (UCHAR)parseLong(params[1], configData->ScreenshotPart);
        configData->ScreenshotPart = min(max(configData->ScreenshotPart , 1), 9);
    }
    else if (strcmp(params[0], "LCDI2CADDR") == 0) {
        configData->LCDI2CAddr = (UCHAR)parseLong(params[1], configData->LCDI2CAddr);
    }
    else if (strcmp(params[0], "LCDPROTO") == 0) {
        configData->LCDProto = (UCHAR)parseLong(params[1], configData->LCDProto);
        configData->LCDProto = min(max(configData->LCDProto , 0), 1);
    }
    else if (strcmp(params[0], "LCDBUS") == 0) {
        configData->LCDBus = (UCHAR)parseLong(params[1], configData->LCDBus);
        configData->LCDBus = min(max(configData->LCDBus , 0), 1);
    }
    else if (strcmp(params[0], "XTALFREQ") == 0) {
        configData->XtalFreq = (UCHAR)parseLong(params[1], configData->XtalFreq);
        configData->XtalFreq = min(max(configData->XtalFreq , 0), 1);
    }
    else if (strcmp(params[0], "OVERCLOCKING") == 0) {
        configData->Overclocking = parseBoolean(params[1], configData->Overclocking);
    }
    else if (strcmp(params[0], "CPUMPLLCOEFF") == 0) {
        configData->CPUMPLLCoeff = parseLong(params[1], configData->CPUMPLLCoeff);
    }
    else if (strcmp(params[0], "NVPLLCOEFF") == 0) {
        configData->NVPLLCoeff = parseLong(params[1], configData->NVPLLCoeff);
    }
    else if (strcmp(params[0], "TUDATAREDIR") == 0) {
        configData->TUDATARedir = parseBoolean(params[1], configData->TUDATARedir);
    }
    else if (strcmp(params[0], "TUDATAREDIRHDD") == 0) {
        configData->TUDATARedirHDD = (UCHAR)parseLong(params[1], configData->TUDATARedirHDD);
        configData->TUDATARedirHDD  = min(max(configData->TUDATARedirHDD , 0), 1);
    }
    else if (strcmp(params[0], "TUDATAREDIRPART") == 0) {
        configData->TUDATARedirPart = (UCHAR)parseLong(params[1], configData->TUDATARedirPart);
        configData->TUDATARedirPart = min(max(configData->TUDATARedirPart , 1), 9);
    }
    else if (strcmp(params[0], "IGRMASTERPORT") == 0) {
        configData->IGRMasterPort = min(max((UCHAR)parseLong(params[1], configData->IGRMasterPort), 0), 4);
    }
    else if (strcmp(params[0], "IGRDASH") == 0 && strlen(params[1]) > 1) {
        configData->IGRDash = parseIGR(params[1]);
    }
    else if (strcmp(params[0], "IGRGAME") == 0 && strlen(params[1]) > 1) {
        configData->IGRGame = parseIGR(params[1]);
    }
    else if (strcmp(params[0], "IGRFULL") == 0 && strlen(params[1]) > 1) {
        configData->IGRFull = parseIGR(params[1]);
    }
    else if (strcmp(params[0], "IGRSHUTDOWN") == 0 && strlen(params[1]) > 1) {
        configData->IGRShutdown = parseIGR(params[1]);
    }
    else if (strcmp(params[0], "IGRCYCLE") == 0 && strlen(params[1]) > 1) {
        configData->IGRCycle = parseIGR(params[1]);
    }
    else if (strcmp(params[0], "IGRSCREEN") == 0 && strlen(params[1]) > 1) {
        configData->IGRScreen = parseIGR(params[1]);
    }
}

void cerbiosIniHelper::parseConfig(cerbiosConfig* config, utils::dataContainer* configData) {
    char* lineBuffer = (char*)malloc(1024);
    char* param1 = (char*)lineBuffer + 512;
    char* param2 = (char*)param1 + 256;
    bool skip = FALSE;

    uint32_t lineLength = 0;
    for (uint32_t i = 0; i < configData->size; i++) {
        char currentChar = configData->data[i];
        if (currentChar == '\t') {
            continue;
        } else if (currentChar == ';') {
            skip = true;
        } else if (currentChar == '\r' || currentChar == '\n') {
            if (lineLength > 0) {
                parseConfigLine(config, param1, param2, lineBuffer, lineLength);
                lineLength = 0;
            }
            skip = false;
            continue;
        } else if (lineLength < 512 && skip == FALSE) {
            lineBuffer[lineLength] = currentChar;
            lineLength++;
        }
    }

    if (lineLength > 0) {
        parseConfigLine(config, param1, param2, lineBuffer, lineLength);
    }

    free(lineBuffer);
}

cerbiosConfig cerbiosIniHelper::loadConfig() {
    // Init default config

    cerbiosConfig config;
    memset(&config, 0, sizeof(config));
    setConfigDefault(&config);

    uint32_t fileHandle;
    if (fileSystem::fileOpen("HDD0-E:\\Cerbios\\cerbios.ini", fileSystem::FileModeRead, fileSystem::FileSystem, fileHandle)) {
        uint32_t fileSize;
        if (fileSystem::fileSize(fileHandle, fileSize)) {
            char* buffer = (char*)malloc(fileSize);
            uint32_t bytesRead;
            if (fileSystem::fileRead(fileHandle, buffer, fileSize, bytesRead) && bytesRead == fileSize) {
                utils::dataContainer* configData;
                configData = new utils::dataContainer(buffer, fileSize, fileSize);
                parseConfig(&config, configData);
                delete (configData);
            }
            free(buffer);
        }
        fileSystem::fileClose(fileHandle);
    }

    config.FanSpeed = min(((config.FanSpeed / 10) * 10), 100);
    config.DriveSetup = min(config.DriveSetup, 3);
    for (uint32_t i = 0; i < 4; i++) {
        char value = config.FrontLed[i];
        if (value != 'G' && value != 'R' && value != 'A' && value != 'O') {
            config.FrontLed[i] = 'G';
        }
    }
    config.UdmaModeMaster = min(config.UdmaModeMaster, 6);
    config.UdmaModeSlave = min(config.UdmaModeSlave, 6);

    return config;
}

void cerbiosIniHelper::buildConfig(cerbiosConfig* config, char* buffer) {
    char* tmp;
    buffer[0] = 0;

    // Launch Settings
    strcat(buffer, "; ========================\r\n");
    strcat(buffer, "; Launch Settings\r\n");
    strcat(buffer, "; ========================\r\n\r\n");

    strcat(buffer,
        "; Sets The Primary Dashboard XBE To Load On Boot. Supports HDD0-/HDD1- Prefixes. Falls Back To "
        "Recovery E:\\Cerbios\\Recovery\\default.xbe Then C:\\xboxdash.xbe If Missing.\r\n");
    strcat(buffer, "DashPath = ");
    strcat(buffer, config->DashPath);
    strcat(buffer, "\r\n\r\n");

    strcat(buffer,
        "; Plays A Custom Boot Animation XBE During Startup. Falls Back To "
        "E:\\Cerbios\\BootAnims\\Xbox\\bootanim.xbe If Not Set.\r\n");
    strcat(buffer, "BootAnimPath = ");
    strcat(buffer, config->BootAnimPath);
    strcat(buffer, "\r\n\r\n");

    // Video & Display Settings
    strcat(buffer, "; ========================\r\n");
    strcat(buffer, "; Video & Display Settings\r\n");
    strcat(buffer, "; ========================\r\n\r\n");

    strcat(buffer,
        "; Performs AV Cable Check On Boot. Halts Boot With Flashing Green And Amber LEDs If No Cable Detected.\r\n");
    strcat(buffer, "AVCheck = ");
    strcat(buffer, config->AVCheck ? "True" : "False");
    strcat(buffer, "\r\n\r\n");

    strcat(buffer,
        "; Manually Sets Flicker Filter Level. 0 = Off, 1-5 = Filter Strength, 6 = Use System/Game Default.\r\n");
    strcat(buffer, "ForceFFilter = ");
    tmp = stringUtility::formatString("%i\r\n\r\n", config->ForceFFilter);
    strcat(buffer, tmp);
    free(tmp);

    strcat(buffer,
        "; Forces 480i Games And Apps To Render In 480p. Requires Component Cables And 480p Enabled In MS "
        "Dashboard. PAL Modes Unaffected.\r\n");
    strcat(buffer, "Force480p = ");
    strcat(buffer, config->Force480p ? "True" : "False");
    strcat(buffer, "\r\n\r\n");

    strcat(buffer,
        "; Force 480i AV mode if component cable is connected. Useful for 480i-only CRT or other TVs.\r\n");
    strcat(buffer, "Force480i = ");
    strcat(buffer, config->Force480i ? "True" : "False");
    strcat(buffer, "\r\n\r\n");

    strcat(buffer,
        "; Forces VGA Output Using RGsB Signal. Requires VGA Or Component Cable And Sync-On-Green Compatible "
        "Display. Automatically Enables Force480p And NTSC Mode. Not Compatible With 1.6 Consoles.\r\n");
    strcat(buffer, "ForceVGA = ");
    strcat(buffer, config->ForceVGA ? "True" : "False");
    strcat(buffer, "\r\n\r\n");

    strcat(buffer, "; Enables Support For Team-Resurgent's HDMI FW.\r\n");
    strcat(buffer, "TrFwSupport = ");
    strcat(buffer, config->TrFwSupport ? "True" : "False");
    strcat(buffer, "\r\n\r\n");

    strcat(buffer, "; If No AV Pack Is Connected, Force To HDTV Mode. Requires AVCheck = False.\r\n");
    strcat(buffer, "ForceHDAVPack = ");
    strcat(buffer, config->ForceHDAVPack ? "True" : "False");
    strcat(buffer, "\r\n\r\n");

    // Storage & Drive Config
    strcat(buffer, "; ========================\r\n");
    strcat(buffer, "; Storage & Drive Config\r\n");
    strcat(buffer, "; ========================\r\n\r\n");

    strcat(buffer,
        "; Defines Console Drive Setup: 0 = HDD+DVD (Stock), 1 = HDD Only (Legacy Mode), 2 = HDD Only "
        "(Modern Mode), 3 = Dual HDD With Slave In DVD Bay.\r\n");
    strcat(buffer, "DriveSetup = ");
    tmp = stringUtility::formatString("%i\r\n\r\n", config->DriveSetup);
    strcat(buffer, tmp);
    free(tmp);

    strcat(buffer,
        "; Sets UDMA Mode For Master HDD On Cold Boot. 0 = Auto (Startech), 1 = Auto (Generic), 2-6 = UDMA "
        "Modes (Higher = Faster, Requires 80-Wire IDE Cable).\r\n");
    strcat(buffer, "UdmaModeMaster = ");
    tmp = stringUtility::formatString("%i\r\n\r\n", config->UdmaModeMaster);
    strcat(buffer, tmp);
    free(tmp);

    strcat(buffer,
        "; Sets UDMA Mode For Slave HDD In Dual HDD Setup. 0 = Auto (Startech), 1 = Auto (Generic), 2-6 = "
        "UDMA Modes (Higher = Faster, Requires 80-Wire IDE Cable And Jumper).\r\n");
    strcat(buffer, "UdmaModeSlave = ");
    tmp = stringUtility::formatString("%i\r\n\r\n", config->UdmaModeSlave);
    strcat(buffer, tmp);
    free(tmp);

    // Cooling & Performance
    strcat(buffer, "; ========================\r\n");
    strcat(buffer, "; Cooling & Performance\r\n");
    strcat(buffer, "; ========================\r\n\r\n");

    strcat(buffer, "; Custom Fan Speed (%) Set During Boot. 0 = Auto, 10-100 = Manual Control In Steps Of 2.\r\n");
    strcat(buffer, "FanSpeed = ");
    tmp = stringUtility::formatString("%i\r\n\r\n", config->FanSpeed);
    strcat(buffer, tmp);
    free(tmp);

    strcat(buffer,
        "; Allows Cerbios To Control Fan Speed On Boot, Reboot, And App Launch. Disable To Let Apps / Dash "
        "Override Fan Speed Instead.\r\n");
    strcat(buffer, "OverrideFan = ");
    tmp = stringUtility::formatString("%s\r\n\r\n", config->OverrideFan ? "True" : "False");
    strcat(buffer, tmp);
    free(tmp);

    strcat(buffer, "; Tells Cerbios Which XTAL Frequency Your Xbox Is Running At, Usually Only Need To Change This With Some CPU-Upgraded Mods.\r\n");
    strcat(buffer, "; 0 = Auto, Based On The CPU's Returned MSR Value\r\n");
    strcat(buffer, "; 1 = Force 16.66MHz XTAL Frequency (Stock)\r\n");
    strcat(buffer, "; 2 = Force 13.5MHz XTAL Frequency\r\n");
    strcat(buffer, "; Requires Full Reboot If Changed\r\n");
    strcat(buffer, "XtalFreq = ");
    tmp = stringUtility::formatString("%i\r\n\r\n", config->XtalFreq);
    strcat(buffer, tmp);
    free(tmp);

    strcat(buffer, "; ========================\r\n");
    strcat(buffer, "; Overclocking\r\n");
    strcat(buffer, "; ========================\r\n");
    strcat(buffer,
        "; Warning: This Is For Advanced Users Only As Setting The Incorrect Values Can Lead To Instability "
        "Or Damage.\r\n");
    strcat(buffer,
        "; Team Cerbios Is Not Responsible For Misuse Of This Feature. This Feature Directly Exposes The "
        "Various Hardware\r\n");
    strcat(buffer,
        "; Registers That Handle CPU And GPU Frequency. The Register Values Are Represented As 3-Byte Hex "
        "Numbers, Preceded By \"0x\"\r\n");
    strcat(buffer,
        "; Pressing Eject Will Enter Safe Mode And Use Default Values. Trial-And-Error Is Required To Find "
        "A Suitable Value\r\n");
    strcat(buffer,
        "; For Your Xbox. Test In Small Increments From Stock Values, Then Let The Xbox Sit For Several "
        "Minutes To See If Any\r\n");
    strcat(buffer,
        "; Crashes Or Freezing Occur And Temperatures Are Within Acceptable Range. Increasing Fan Speed Is "
        "Recommended.");
    strcat(buffer, "\r\n\r\n");

    strcat(buffer, "Overclocking = ");
    strcat(buffer, config->Overclocking ? "True" : "False");
    strcat(buffer, "\r\n\r\n");

    strcat(buffer, "; FSB  = Front-Side Bus - Data Bus Used To Connect CPU To Northbridge\r\n");
    strcat(buffer, "; XTAL = Xbox's Crystal Oscillator, On Retail Xbox, This Value Is Fixed At 16.66666MHz\r\n");
    strcat(buffer, "; PLL  = Phase-Locked Loop - Hardware Used To Generate Various Clock Frequencies\r\n");
    strcat(buffer,
        "; VCO  = Voltage-Controlled Oscillator - Hardware Used To Generate Various Clock Frequencies, "
        "Feeds Into PLL\r\n");
    strcat(buffer, ";\r\n");
    strcat(buffer, "; === CPU/RAM Overclocking ===\r\n");
    strcat(buffer,
        "; The CPU Multiplier For A Stock Xbox 733MHz CPU Is 5.5x, You Will Need To Lookup The CPU "
        "Multiplier For Any Other CPU\r\n");
    strcat(buffer, "; This Is How The Values Are Calculated (Stock 0x230801 Value Used In This Example):\r\n");
    strcat(buffer, ";\r\n");
    strcat(buffer, "; CPUMPLLCoeff Fields (In Hex):\r\n");
    strcat(buffer, ";\r\n");
    strcat(buffer, ";                      2 3 08 01\r\n");
    strcat(buffer, ";                      │ │  │  │\r\n");
    strcat(buffer, "; Memory Divider ──────┘ │  │  │\r\n");
    strcat(buffer, "; FSB Multiplier ────────┘  │  │\r\n");
    strcat(buffer, "; PLL_N Multiplier ─────────┘  │\r\n");
    strcat(buffer, "; PLL_M Divider ───────────────┘\r\n");
    strcat(buffer, ";\r\n");
    strcat(buffer, ";  FSB_FREQ =  XTAL / PLL_M * PLL_N\r\n");
    strcat(buffer, "; 133.33MHz = 16.66 /   1   *   8\r\n");
    strcat(buffer, ";\r\n");
    strcat(buffer, ";  CPU_FREQ = FSB_FREQ * CPU_MULTI\r\n");
    strcat(buffer, "; 733.33MHz =  133.33  *   5.5\r\n");
    strcat(buffer, ";\r\n");
    strcat(buffer, ";  VCO_FREQ = FSB_FREQ * FSB_MULTI * 2\r\n");
    strcat(buffer, ";    800MHz =  133.33  *     3     * 2\r\n");
    strcat(buffer, ";\r\n");
    strcat(buffer, ";  MEM_FREQ = VCO_FREQ / (MEM_DIV * 2)\r\n");
    strcat(buffer, ";    200MHz =    800   / (   2    * 2)\r\n");
    strcat(buffer, ";\r\n");
    strcat(buffer, ";\r\n");
    strcat(buffer, "; Example Value To Get 802MHz Overclock On Stock CPU: 0x232304\r\n");
    strcat(buffer, "; Don't Forget That Changing The FSB Frequency Will Also\r\n");
    strcat(buffer, "; Affect The Memory Frequency.\r\n");
    strcat(buffer, ";  FSB_FREQ =  XTAL / PLL_M * PLL_N\r\n");
    strcat(buffer, "; 145.83MHz = 16.66 /   4   *   35\r\n");
    strcat(buffer, ";\r\n");
    strcat(buffer, ";  CPU_FREQ = FSB_FREQ * CPU_MULTI\r\n");
    strcat(buffer, ";    802MHz =  145.83  *   5.5\r\n");
    strcat(buffer, ";\r\n");
    strcat(buffer, ";  VCO_FREQ = FSB_FREQ * FSB_MULTI * 2\r\n");
    strcat(buffer, ";    875MHz =  145.83  *     3     * 2\r\n");
    strcat(buffer, ";\r\n");
    strcat(buffer, ";  MEM_FREQ = VCO_FREQ / (MEM_DIV * 2)\r\n");
    strcat(buffer, "; 218.75MHz =    875   / (   2    * 2)\r\n");
    strcat(buffer, "\r\n");

    strcat(buffer, "; CPUMPLLCoeff (hex): Sets The CPU Overclock (Stock: 0x230801) Or 0x0\r\n");
    strcat(buffer, "CPUMPLLCoeff = ");
    tmp = stringUtility::formatString("0x%06X\r\n\r\n", config->CPUMPLLCoeff);
    strcat(buffer, tmp);
    free(tmp);

    strcat(buffer, "; === GPU Overclocking ===\r\n");
    strcat(buffer, "; The Stock GPU Clock Frequency Value Is 0x011C01\r\n");
    strcat(buffer, ";\r\n");
    strcat(buffer, "; NVPLLCoeff Fields (In Hex):\r\n");
    strcat(buffer, ";\r\n");
    strcat(buffer, ";                     01 1C 01\r\n");
    strcat(buffer, ";                      │  │  │\r\n");
    strcat(buffer, "; PLL_P Exponent ──────┘  │  │\r\n");
    strcat(buffer, "; PLL_N Multiplier ───────┘  │\r\n");
    strcat(buffer, "; PLL_M Divider ─────────────┘\r\n");
    strcat(buffer, ";\r\n");
    strcat(buffer, ";  GPU_FREQ = PLL_N *  XTAL / (2 ^ PLL_P) / PLL_M\r\n");
    strcat(buffer, "; 233.33MHz =   28  * 16.66 / (2 ^   1  ) /   1\r\n");
    strcat(buffer, ";\r\n");
    strcat(buffer, "; Example Value To Get 266MHz Overclock On GPU: 0x012001\r\n");
    strcat(buffer, ";  GPU_FREQ = PLL_N *  XTAL / (2 ^ PLL_P) / PLL_M\r\n");
    strcat(buffer, "; 266.66MHz =   32  * 16.66 / (2 ^   1  ) /   1\r\n\r\n");

    strcat(buffer, "; NVPLLCoeff (hex): Sets The GPU Overclock (Stock: 0x011C01) Or 0x0\r\n");
    strcat(buffer, "NVPLLCoeff = ");
    tmp = stringUtility::formatString("0x%06X\r\n\r\n", config->NVPLLCoeff);
    strcat(buffer, tmp);
    free(tmp);

    // Front Panel & Visuals
    strcat(buffer, "; ========================\r\n");
    strcat(buffer, "; Front Panel & Visuals\r\n");
    strcat(buffer, "; ========================\r\n\r\n");

    strcat(buffer,
        "; Configures Front Panel LED Ring Pattern. Uses 4-Character Code: G = Green, R = Red, A = Amber, O "
        "= Off. Cycles Pattern Until Overridden.\r\n");
    strcat(buffer, "FrontLed = ");
    strcat(buffer, config->FrontLed);
    strcat(buffer, "\r\n\r\n");

    strcat(buffer, "; Enables In-Game LCD Display For FPS, Temps, RAM Usage, And More.\r\n");
    strcat(buffer, "InAppLCDEnable = ");
    strcat(buffer, config->InAppLCDEnable ? "True" : "False");
    strcat(buffer, "\r\n\r\n");

    strcat(buffer, "; Sets The LCD Bus. 0 = Xbox's SMBus, 1 = X3LCD Via X3 Modchip.\r\n");
    strcat(buffer, "LCDBus = ");
    tmp = stringUtility::formatString("%i\r\n\r\n", config->LCDBus);
    strcat(buffer, tmp);
    free(tmp);

    strcat(buffer,
        "; Sets I2C Address For LCD Display. Valid Range: 0x20-0x27 Or 0x38-0x3F Depending On Module Type.\r\n");
    strcat(buffer, "LCDI2CAddr = ");
    tmp = stringUtility::formatString("0x%X\r\n\r\n", config->LCDI2CAddr);
    strcat(buffer, tmp);
    free(tmp);

    strcat(buffer, "; Sets Protocol For LCD. 0 = HD44780-Compatible (LCD2004) Or X3LCD, 1 = US2066 (NHD-0420CW).\r\n");
    strcat(buffer, "LCDProto = ");
    tmp = stringUtility::formatString("%i\r\n\r\n", config->LCDProto);
    strcat(buffer, tmp);
    free(tmp);

    // Enhancements
    strcat(buffer, "; ========================\r\n");
    strcat(buffer, "; Enhancements\r\n");
    strcat(buffer, "; ========================\r\n\r\n");

    strcat(buffer,
        "; Blocks Games From Updating The MS Dashboard. Useful For Softmod Installs Or When Running Cerbios "
        "In BFM Mode.\r\n");
    strcat(buffer, "BlockDashUpdate = ");
    strcat(buffer, config->BlockDashUpdate ? "True" : "False");
    strcat(buffer, "\r\n\r\n");

    strcat(buffer,
        "; Redirects Xbox Live Dashboard Calls To Xonlinedash_Original.xbe. Useful For Softmod Setups That "
        "Use A Dummy Xonlinedash.xbe To Prevent Disc Boot Updates.\r\n");
    strcat(buffer, "XonlineDashRedir = ");
    strcat(buffer, config->XonlineDashRedir ? "True" : "False");
    strcat(buffer, "\r\n\r\n");

    strcat(buffer,
        "; Enables Runtime XBE Patching For CPU-Upgraded Or Overclocked Consoles. Has No Effect On Stock CPUs.\r\n");
    strcat(buffer, "AdvCPUSupport = ");
    strcat(buffer, config->AdvCPUSupport ? "True" : "False");
    strcat(buffer, "\r\n\r\n");

    strcat(buffer,
        "; Ignores /LIMITMEM Flag In XBE Files To Allow Access To Full 128MB RAM. Only Use On Consoles With "
        "128MB RAM. May Cause Compatibility Issues Or Crashes.\r\n");
    strcat(buffer, "DisableLimitMem = ");
    strcat(buffer, config->DisableLimitMem ? "True" : "False");
    strcat(buffer, "\r\n\r\n");

    strcat(buffer,
        "; Enables Compatibility Fixes For Specific Games By Applying XBE Patches At Runtime. Useful For "
        "Titles With Known Issues.\r\n");
    strcat(buffer, "ApplyTitlePatches = ");
    strcat(buffer, config->ApplyTitlePatches ? "True" : "False");
    strcat(buffer, "\r\n\r\n");

    strcat(buffer,
        "; Marks C Partition As Read-Only. Prevents Writes To The System Partition While Cerbios Is "
        "Running. Useful For Softmod Installs.\r\n");
    strcat(buffer, "ReadOnlyC = ");
    strcat(buffer, config->ReadOnlyC ? "True" : "False");
    strcat(buffer, "\r\n\r\n");

    strcat(buffer, "; Marks F Partition As Read-Only. Prevents Writes To The Extended Partition While Cerbios Is Running.\r\n");
    strcat(buffer, "ReadOnlyF = ");
    strcat(buffer, config->ReadOnlyF ? "True" : "False");
    strcat(buffer, "\r\n\r\n");

    strcat(buffer, "; Marks G Partition As Read-Only. Prevents Writes To The Extended Partition While Cerbios Is Running.\r\n");
    strcat(buffer, "ReadOnlyG = ");
    strcat(buffer, config->ReadOnlyG ? "True" : "False");
    strcat(buffer, "\r\n\r\n");

    strcat(buffer,
        "; Restores Original Xbox Behavior Of Resetting When Tray Is Ejected. Reloads Dashboard Automatically.\r\n");
    strcat(buffer, "ResetOnEject = ");
    strcat(buffer, config->ResetOnEject ? "True" : "False");
    strcat(buffer, "\r\n\r\n");

    strcat(buffer,
        "; Enables Automatic Time Sync Using RTC Module Connected To SMBus. Requires Cerbios 2.4+ Or "
        "PrometheOS 1.4+.\r\n");
    strcat(buffer, "RTCEnable = ");
    strcat(buffer, config->RTCEnable ? "True" : "False");
    strcat(buffer, "\r\n\r\n");

    strcat(buffer, "; Set a custom auto power-down timeout, in seconds. 0 = default: keep stock behaviour, minimum 30\r\n");
    strcat(buffer, "; Requires \"Auto Off\" feature to be enabled in MS dashboard\r\n");
    strcat(buffer, "AutoOffTimeout = ");
    tmp = stringUtility::formatString("%u\r\n\r\n", config->AutoOffTimeout);
    strcat(buffer, tmp);
    free(tmp);

    strcat(buffer, "; Enables UDATA/TDATA Redirection To Alternate Drive/Partition.\r\n");
    strcat(buffer, "TUDATARedir = ");
    strcat(buffer, config->TUDATARedir ? "True" : "False");
    strcat(buffer, "\r\n\r\n");

    strcat(buffer, "; Selects Which HDD To Use For Redirection. 0 = Master, 1 = Slave (Dual HDD).\r\n");
    strcat(buffer, "TUDATARedirHDD = ");
    tmp = stringUtility::formatString("%i\r\n\r\n", config->TUDATARedirHDD);
    strcat(buffer, tmp);
    free(tmp);

    strcat(buffer, "; Selects Which Partition To Use For UDATA/TDATA:, 1 = E:, 6 = F:, 7 = G:\r\n");
    strcat(buffer, "TUDATARedirPart = ");
    tmp = stringUtility::formatString("%i\r\n\r\n", config->TUDATARedirPart);
    strcat(buffer, tmp);
    free(tmp);

    strcat(buffer,
        "; Enables Screenshot Capture Of The Running XBEs Framebuffer. Saved To "
        "E:\\Cerbios\\Screenshots\\(Title ID) When L-Thumb + R-Thumb Combo Is Pressed.\r\n");
    strcat(buffer, "EnableScreenshots = ");
    strcat(buffer, config->EnableScreenshots ? "True" : "False");
    strcat(buffer, "\r\n\r\n");

    strcat(buffer, "; Selects Which HDD To Use For Screenshots. 0 = Master, 1 = Slave (Dual HDD).\r\n");
    strcat(buffer, "ScreenshotHDD = ");
    tmp = stringUtility::formatString("%i\r\n\r\n", config->ScreenshotHDD);
    strcat(buffer, tmp);
    free(tmp);

    strcat(buffer, "; Selects Which Partition To Use For Screenshots:, 1 = E:, 6 = F:, 7 = G:\r\n");
    strcat(buffer, "ScreenshotPart = ");
    tmp = stringUtility::formatString("%i\r\n\r\n", config->ScreenshotPart);
    strcat(buffer, tmp);
    free(tmp);

    // In-Game Key Combo
    strcat(buffer, "; ========================\r\n");
    strcat(buffer, "; In-Game Key Combos\r\n");
    strcat(buffer, "; ========================\r\n");
    strcat(buffer, "; Use The Values Listed Below To Define The Combination:\r\n");
    strcat(buffer, "; A = 0, B = 1, X = 2, Y = 3, BLACK = 4, WHITE = 5, LEFT_TRIGGER = 6, RIGHT_TRIGGER = 7\r\n");
    strcat(buffer,
        "; DPAD_UP = 8, DPAD_DOWN = 9, DPAD_LEFT = A, DPAD_RIGHT = B, START = C, BACK = D, LEFT_THUMB = E, "
        "RIGHT_THUMB = F \r\n\r\n");

    strcat(buffer, "; Select Which Controller Can Trigger IGR Combos. 0 = ALL, 1-4 = Controller Ports.\r\n");
    strcat(buffer, "IGRMasterPort = ");
    tmp = stringUtility::formatString("%i\r\n\r\n", config->IGRMasterPort);
    strcat(buffer, tmp);
    free(tmp);

    strcat(buffer, "; Soft Reset Back To Dashboard. Keeps Any Mounted ISO/CCI Or Game Active.\r\n");
    strcat(buffer, "IGRDash = ");
    char* igrDash = shortToIGR(config->IGRDash);
    strcat(buffer, igrDash);
    free(igrDash);
    strcat(buffer, "\r\n\r\n");

    strcat(buffer, "; Reloads The Currently Running Game Or Application.\r\n");
    strcat(buffer, "IGRGame = ");
    char* igrGame = shortToIGR(config->IGRGame);
    strcat(buffer, igrGame);
    free(igrGame);
    strcat(buffer, "\r\n\r\n");

    strcat(buffer, "; Full Kernel Reset. Unmounts Any Loaded ISO/CCI Or Game And Returns To Dashboard.\r\n");
    strcat(buffer, "IGRFull = ");
    char* igrFull = shortToIGR(config->IGRFull);
    strcat(buffer, igrFull);
    free(igrFull);
    strcat(buffer, "\r\n\r\n");

    strcat(buffer, "; Performs A Full Power Cycle. Reboots The Console Completely.\r\n");
    strcat(buffer, "IGRCycle = ");
    char* igrCycle = shortToIGR(config->IGRCycle);
    strcat(buffer, igrCycle);
    free(igrCycle);
    strcat(buffer, "\r\n\r\n");

    strcat(buffer, "; Shuts Down The Console From In-Game.\r\n");
    strcat(buffer, "IGRShutdown = ");
    char* igrShutdown = shortToIGR(config->IGRShutdown);
    strcat(buffer, igrShutdown);
    free(igrShutdown);
    strcat(buffer, "\r\n\r\n");

    strcat(buffer,
        "; Captures A Screenshot Of The Current Framebuffer. Requires EnableScreenshots To Be Set To True.\r\n");
    strcat(buffer, "IGRScreen = ");
    char* igrScreen = shortToIGR(config->IGRScreen);
    strcat(buffer, igrScreen);
    free(igrScreen);
    strcat(buffer, "\r\n\r\n");

    // Debugging & Development
    strcat(buffer, "; ========================\r\n");
    strcat(buffer, "; Debugging & Development\r\n");
    strcat(buffer, "; ========================\r\n\r\n");

    strcat(buffer,
        "; Enables Debug Mode. Boots C:\\xshell.xbe And Loads E:\\xbdm.dll To Allow Visual Studio Debugging "
        "Over Network.\r\n");
    strcat(buffer, "Debug = ");
    strcat(buffer, config->Debug ? "True" : "False");
    strcat(buffer, "\r\n");
}

void cerbiosIniHelper::saveConfig(char* buffer) {
    const char* configPath = "HDD0-E:\\Cerbios\\cerbios.ini";
    const char* dirPath = "HDD0-E:\\Cerbios\\";

    bool exists = false;
    if (fileSystem::directoryExists(dirPath, exists) && !exists) {
        fileSystem::directoryCreate(dirPath);
    }

    uint32_t fileHandle;
    if (fileSystem::fileOpen(configPath, fileSystem::FileModeWrite, fileSystem::FileSystem, fileHandle)) {
        uint32_t bytesWritten = 0;
        fileSystem::fileWrite(fileHandle, buffer, strlen(buffer), bytesWritten);
        fileSystem::fileClose(fileHandle);
    } else {
        utils::debugPrint("Failed to open cerbios.ini for writing. Check if directory exists.\n");
    }
}

void cerbiosIniHelper::setConfigDefault(cerbiosConfig* config) {

    strcpy(config->DashPath, "HDD0-C:\\evoxdash.xbe");
    strcpy(config->BootAnimPath, "HDD0-E:\\Cerbios\\BootAnims\\Xbox\\bootanim.xbe");

    config->AVCheck = 1;
    config->ForceFFilter = 6;
    config->Force480p = 0;
    config->Force480i = 0;
    config->ForceVGA = 0;

    config->DriveSetup = 1;
    config->UdmaModeMaster = 2;
    config->UdmaModeSlave = 2;

    config->FanSpeed = 0;
    config->OverrideFan = 1;
    config->XtalFreq = 0;
    config->Overclocking = 0;
    config->CPUMPLLCoeff = 0x000000;
    config->NVPLLCoeff = 0x000000;

    strcpy(config->FrontLed, "GGGG");
    config->InAppLCDEnable = 0;
    config->LCDBus = 0;
    config->LCDI2CAddr = 0x3C;
    config->LCDProto = 0;

    config->BlockDashUpdate = 0;
    config->XonlineDashRedir = 0;
    config->AdvCPUSupport = 1;
    config->DisableLimitMem = 0;
    config->ApplyTitlePatches = 1;
    config->ReadOnlyC = 0;
    config->ReadOnlyF = 0;
    config->ReadOnlyG = 0;
    config->TrFwSupport = 0;
    config->ForceHDAVPack = 0;
    config->ResetOnEject = 0;
    config->RTCEnable = 0;
    config->AutoOffTimeout = 0;
    config->TUDATARedir = 0;
    config->TUDATARedirHDD = 0;
    config->TUDATARedirPart = 0;
    config->EnableScreenshots = 0;
    config->ScreenshotHDD = 0;
    config->ScreenshotPart = 0;

    config->IGRMasterPort = 0;
    config->IGRDash = 0xC030;
    config->IGRGame = 0xD010;
    config->IGRFull = 0xD020;
    config->IGRCycle = 0xC021;
    config->IGRShutdown = 0xD001;
    config->IGRScreen = 0x00C0;

    config->Debug = 0;
}

