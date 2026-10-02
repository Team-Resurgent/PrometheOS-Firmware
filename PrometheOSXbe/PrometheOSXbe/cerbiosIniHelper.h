#pragma once

#include "utils.h"
#include "xboxInternals.h"

typedef struct cerbiosConfig {
    unsigned char AVCheck;
    unsigned char Debug;

    unsigned char DriveSetup;

    char DashPath[100];
    char BootAnimPath[100];

    char FrontLed[5];

    unsigned char OverrideFan;
    unsigned char FanSpeed;
    unsigned char UdmaModeMaster;
    unsigned char UdmaModeSlave;
    unsigned char Force480p;
    unsigned char Force480i;
    unsigned char ForceVGA;

    unsigned char ForceFFilter;
    unsigned char InAppLCDEnable;
    unsigned char LCDI2CAddr;
    unsigned char LCDProto;
    unsigned char LCDBus;
    unsigned char RTCEnable;

    unsigned int AutoOffTimeout;

    unsigned char BlockDashUpdate;
    unsigned char XonlineDashRedir;
    unsigned char ResetOnEject;
    unsigned char AdvCPUSupport;
    unsigned char DisableLimitMem;
    unsigned char ApplyTitlePatches;
    unsigned char ReadOnlyC;
    unsigned char ReadOnlyF;
    unsigned char ReadOnlyG;
    unsigned char TrFwSupport;
    unsigned char ForceHDAVPack;
    unsigned char EnableScreenshots;
    unsigned char ScreenshotHDD;
    unsigned char ScreenshotPart;

    unsigned char Overclocking;
    unsigned int CPUMPLLCoeff;
    unsigned int NVPLLCoeff;

    unsigned char TUDATARedir;
    unsigned char TUDATARedirHDD;
    unsigned char TUDATARedirPart;

    unsigned char IGRMasterPort;
    unsigned short IGRDash;
    unsigned short IGRGame;
    unsigned short IGRFull;
    unsigned short IGRShutdown;
    unsigned short IGRCycle;
    unsigned short IGRScreen;

    unsigned char XtalFreq;

} cerbiosConfig;

class cerbiosIniHelper {
  public:
    static cerbiosConfig loadConfig();
    static void buildConfig(cerbiosConfig* configData, char* buffer);
    static void saveConfig(char* buffer);
    static void setConfigDefault(cerbiosConfig* configData);
    static char* shortToIGR(unsigned short value);
    static unsigned short parseIGR(char* value);
  private:
    static void upperCase(char* value);
    static void trimSpace(char* value);
    static uint8_t parseByte(char* value, uint8_t defaultValue);
    static uint32_t parseLong(char* value, uint32_t defaultValue);
    static uint8_t parseBoolean(char* value, uint8_t defaultValue);
    static void parseConfigLine(cerbiosConfig* config, char* param1, char* param2, char* buffer, unsigned long bufferSize);
    static void parseConfig(cerbiosConfig* config, utils::dataContainer* configData);
};
