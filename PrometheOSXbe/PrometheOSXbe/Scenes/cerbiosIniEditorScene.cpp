#include "cerbiosIniEditorScene.h"
#include "sceneManager.h"

#include "..\ListItems\listItemText.h"
#include "..\ListItems\listItemAction.h"
#include "..\ListItems\listItemPath.h"
#include "..\ListItems\listItemTitle.h"
#include "..\ListItems\listItemBool.h"
#include "..\ListItems\listItemHex.h"
#include "..\ListItems\listItemEnum.h"
#include "..\ListItems\listItemScene.h"
#include "..\ListItems\listItemInteger.h"

#include "..\Scenes\frontLedSelectorScene.h"

#include "..\utils.h"
#include "..\context.h"
#include "..\drawing.h"
#include "..\component.h"
#include "..\ssfn.h"
#include "..\inputManager.h"
#include "..\stringUtility.h"
#include "..\xboxConfig.h"
#include "..\theme.h"
#include "..\audioPlayer.h"

namespace
{
    static cerbiosConfig mConfig;
    static bool mNeedsSave;
}

enum OptionIndex {
    OPT_DASH_PATH,
    OPT_BOOT_ANIM_PATH,
    OPT_AV_CHECK,
    OPT_FORCE_FLICKER_FILTER,
    OPT_FORCE_480P,
    OPT_FORCE_480I,
    OPT_FORCE_VGA,
    OPT_DRIVE_SETUP,
    OPT_UDMA_MASTER,
    OPT_UDMA_SLAVE,
    OPT_FAN_SPEED,
    OPT_FAN_SPEED_OVERRIDE,
    OPT_XTAL_FREQ,
    OPT_OVERCLOCKING,
    OPT_CPUMPLL_COEFF,
    OPT_NVPLL_COEFF,
    OPT_FRONT_LED,
    OPT_LCD_DISPLAY,
    OPT_LCD_BUS,
    OPT_LCD_I2C_ADDR,
    OPT_LCD_PROTOCOL,
    OPT_BLOCK_DASH_UPDATE,
    OPT_XONLINE_DASH_REDIRECT,
    OPT_ADV_CPU_SUPPORT,
    OPT_XBE_MEM_DELIMIT,
    OPT_TITLE_PATCHES,
    OPT_READ_ONLY_C_PARTITION,
    OPT_READ_ONLY_F_PARTITION,
    OPT_READ_ONLY_G_PARTITION,
    OPT_TR_FW_SUPPORT,
    OPT_FORCE_HDAV_PACK,
    OPT_RESET_ON_EJECT,
    OPT_RTC_ENABLE,
    OPT_TDATA_UDATA_REDIRECT,
    OPT_TDATA_UDATA_REDIRECT_HDD,
    OPT_TDATA_UDATA_REDIRECT_PART,
    OPT_SCREENSHOT_CAPTURE,
    OPT_SCREENSHOT_HDD,
    OPT_SCREENSHOT_PART,
    OPT_IN_GAME_CONTROLLER_PORT,
    OPT_DASH_COMBO,
    OPT_GAME_COMBO,
    OPT_FULL_COMBO,
    OPT_POWER_CYCLE_COMBO,
    OPT_SHUTDOWN_COMBO,
    OPT_SCREENSHOT_COMBO,
    OPT_DEBUG_MODE,
    OPT_AUTO_OFF_TIMEOUT,
};

static const char* const optionInfoTexts[] = {
    "Dash Path (DashPath):\n\nDefault Dashboard XBE To Load On BIOS Boot.\nSupports Dual-HDD Prefixes (HDD0- And HDD1-).\nExample: HDD0-C:\\evoxdash.xbe.\n\nIf Missing, Will Fallback To E:\\Cerbios\\Recovery\\default.xbe,\nAnd Then To C:\\xboxdash.xbe.",
    "Boot Animation Path (BootAnimPath):\n\nOptional Animation XBE To Play During Boot.\n\nIf Unavailable, Uses E:\\Cerbios\\BootAnims\\Xbox\\bootanim.xbe.",
    "AV Check (AVCheck):\n\nChecks For A Valid AV Cable On Boot. Halts With Flashing Yellow & Green Lights If Cable Is Missing.\n\nTrue  = Perform AV Cable Check.\nFalse = Skips AV Cable Check.",
    "Force Flicker Filter (ForceFFilter):\n\nSets The Flicker Filter Level Manually.\n\n0 = Off.\n1-5 = Levels Of Filter.\n6 = Don't Force (Use System/Game Default).",
    "Force 480p (Force480p):\n\nForce Games/Apps To Use 480p HD Mode. Component Cable Required. 720p/1080i And PAL Modes Unaffected. Must Be Enabled In The MS Dashboard. Only For NTSC Consoles.\n\nTrue  = Enable Forced 480p.\nFalse = Keep Stock Video Mode.",
    "Force 480i (Force480i):\n\nForce Games/Apps To Use 480i Mode If Component Cable Is Connected. Useful for 480i-only CRT or other TVs. Only For NTSC Consoles.\n\nTrue  = Enable Forced 480i.\nFalse = Keep Stock Video Mode.",
    "Force VGA (ForceVGA):\n\nForce Games And Apps To Output VGA Video Signal (RGsB). An Appropriate Cable Or Adapter Is Required And An Appropriate (SoG, Sync-On-Green) Monitor Is Required, Depending On The Cable Used. This Option Automatically Enables Force480p Option. 1.6 Revision Consoles Are Unsupported.\n\nTrue  = Enable VGA Output.\nFalse = Keep Stock Video Output.",
    "Drive Setup (DriveSetup):\n\n0 = HDD & DVD (Stock) - Must Have DVD Drive.\n1 = HDD Only (Legacy) - Boots Without DVD.\n2 = HDD Only (Modern) - Skips DVD Code Entirely (Faster).\n3 = Dual HDD - Connect Second HDD In DVD Bay (Slave Jumper).",
    "UDMA Mode (UdmaModeMaster):\n\nSets Master HDD UDMA Mode. SATA Adapters Require 80-Wire IDE.\n0 = Auto (Startech)\n1 = Auto (Generic)\n2 = UDMA 2 (Stock)\n3 = UDMA 3 (Requires 80-Wire Cable)\n4 = UDMA 4 (Requires 80-Wire Cable)\n5 = UDMA 5 (Requires 80-Wire Cable)\n6 = UDMA 6 (Requires 80-Wire Cable, Experimental)",
    "UDMA Mode (UdmaModeSlave):\n\nSets Slave HDD UDMA Mode. SATA Adapters Require 80-Wire IDE.\n0 = Auto (Startech)\n1 = Auto (Generic)\n2 = UDMA 2 (Stock)\n3 = UDMA 3 (Requires 80-Wire Cable)\n4 = UDMA 4 (Requires 80-Wire Cable)\n5 = UDMA 5 (Requires 80-Wire Cable)\n6 = UDMA 6 (Requires 80-Wire Cable, Experimental)",
    "Fan Speed (FanSpeed):\n\nCustom Fan Speed (%) Set During Boot.\n\n0 = Auto\n10-100 = Manual Control, Supports Increments Of 10%",
    "Fan Speed Override (OverrideFan):\n\nAllows Cerbios To Control Fan Speed On Boot, Reboot, And App Launch.\n\nTrue  = Cerbios Controls Fan Speed Via Ini Settings.\nFalse = Fan Speed Controlled By Other Applications / Dashboards Instead.",
    "XTAL Frequency (XtalFreq):\n\nTells Cerbios Which XTAL Frequency Your Xbox Is Running At, Usually Only Need To Change This With Some CPU-Upgraded Mods\n\n0 = Auto\n1 = Force 16.66MHz Frequency (Stock)\n2 = Force 13.5MHz Frequency",   
    "Overclocking (Overclocking):\n\nWARNING: Overclocking Is For Advanced Users Only As Setting Incorrect Values Can Lead To Instability Or Damage.\n\nTeam Cerbios Is Not Responsible For Misuse Of This Feature.\n\nPlease Set Custom Fan Speed When Overlcocking To Prevent Overheating.\n\nTrue  = Enable Overclocking.\nFalse = Disable Overclocking.",
    "CPU PLL Coefficient (CPUMPLLCoeff):\n\nEnter a 6-digit Hex Value (e.g. 0x230801) To Adjust The CPU PLL Overclock.\nOnly The Least-significant 3 Bytes Are Used.",
    "NV PLL Coefficient (NVPLLCoeff):\n\nEnter a 6-digit Hex Value (e.g. 0x011C01) To Adjust The GPU PLL Overclock.\nOnly The Least-significant 3 Bytes Are Used.",
    "Front LED (FrontLed):\n\nCustom Front Eject Button LED Colors Set During Boot, Split Into 4 Phases. The Pattern Will Continue To Cycle Until Another Application Takes Control Of The LEDs.\n\nLED Ring Color Code: G = Green, R = Red, A = Amber, O = Off",
    "In-App LCD Enable (InAppLCDEnable):\n\nEnables In-Game LCD Support For Compatible Displays Connected Over The Xbox's SMBus.\n\nWill Display (Mem Usage, CPU Usage, FPS, Temps, HDD Space).\n\nTrue  = LCD Support On.\nFalse = LCD Support Off.",
    "LCD Bus (LCDBus):\n\nChoose LCD Bus:\n0 = Xbox's SMBus\n1 = X3LCD Via X3 Modchip",
    "LCD I2C Address (LCDI2CAddr):\n\nSet I2C Address For The Display Module.\nValid Addresses: (0x20-0x27) And (0x38-0x3F).",
    "LCD Protocol (LCDProto):\n\nChoose I2C Command Set:\n0 = HD44780 (LCD2004) Or X3LCD\n1 = US2066 (NHD-0420CW)",
    "Block Dash Update (BlockDashUpdate):\n\nPrevents Some XBL Games From Updating The MS Dashboard, Which Can Break Softmod Installs. Only Really Useful For Softmods Running Cerbios 2.4.0 Or Higher In BFM Mode.\n\nTrue  = Block Updates.\nFalse = Allow Updates.",
    "Xonline Dash Redirect (XonlineDashRedir):\n\nRedirects Xbox Live Dashboard Calls To Xonlinedash_Original.xbe This Preserves Compatibility With Games That Attempt To Launch The Xbox Live Dash Without Replacing The Dummy Xonlinedash.xbe. Only Really Useful For Softmods Running In BFM Mode\n\nTrue  = Redirect To Xonlinedash_Original.xbe\nFalse = Use Xonlinedash.xbe",
    "Advanced CPU Support (AdvCPUSupport):\n\nAdds On-The-Fly XBE Patching Support For CPU-Upgraded Or Overclocked Xboxes. No Effect On Stock CPUs.\n\nTrue  = Patching Enabled.\nFalse = Patching Disabled.",
    "XBE Mem Limit Bypass (DisableLimitMem):\n\nIgnores /LIMITMEM To Enable Full 128MB RAM Use.\nOnly For 128MB Consoles. May Break Game Compatibility.\n\nTrue = XBE Ignores /LIMITMEM Flag (128MB)\nFalse = XBE Uses /LIMITMEM Flag (64MB)",
    "Apply Title Patches (ApplyTitlePatches):\n\nEnable Game-Specific Fixes For Compatibility Issues.\n\nTrue = Title Patches Are Applied To XBE\nFalse = Title Patches Are Disabled",
    "Read-Only C Partition (ReadOnlyC):\n\nMarks C Partition As Read-Only. Prevents Any Writes To The System Partition While Cerbios Is Running. This Is Especially Useful For Softmod Setups To Prevent Overwrites Or Damage To Dashboard Files.\n\nTrue  = Prevent Writes To C Partition\nFalse = Allow Writes To C Partition",
    "Read-Only F Partition (ReadOnlyF):\n\nMarks F Partition As Read-Only. Prevents Any Writes To The Extended Partition While Cerbios Is Running.\n\nTrue  = Prevent Writes To F Partition\nFalse = Allow Writes To F Partition",
    "Read-Only G Partition (ReadOnlyG):\n\nMarks G Partition As Read-Only. Prevents Any Writes To The Extended Partition While Cerbios Is Running.\n\nTrue  = Prevent Writes To G Partition\nFalse = Allow Writes To G Partition",
    "TR HDMI FW Support (TrFwSupport):\n\nEnables Support For Team-Resurgent's HDMI Firmware For HD+ Devices. The FW Must Be Installed On The HD+ First.\n\nTrue  = Enables Support\nFalse = Disables Support",
    "Force HDTV Mode (ForceHDAVPack):\n\nIf No AV Pack Is Connected, Force To HDTV Mode.\nRequires AVCheck = False.\n\nTrue  = Force HDTV Pack Mode\nFalse = Don't Force HDTV Pack Mode (stock)",
    "Reset On Eject (ResetOnEject):\n\nRestore Stock Behavior Of Resetting When The Tray Ejects.\nReloads Chosen Dashboard On Eject.\n\nTrue  = Reset On Eject.\nFalse = Ignore Eject Reset.",
    "RTC Enable (RtcEnable):\n\nEnables Automatic Clock Syncing With An Appropriate External\nBattery Powered RTC Module Connected To The Xbox's SMBus.\nRequires Cerbios 2.4.0 Or Higher, Or PrometheOS 1.4.0 Or Higher.\n\nTrue  = Auto Sync Clock Via RTC Module.\nFalse = No Sync.",
    "TDATA/UDATA Redir (TUDATARedir):\n\nRedirects Save Game (UDATA/TDATA) To A Different Location.\nUsed To Store Saves On Alternate Drive / Partition.\n\nTrue  = Enable Redirection\nFalse = Use Default Location",
    "TDATA/UDATA HDD: (TUDATARedirHDD)\n\nChoose Target Drive For Redirection:\n0 = Primary HDD (IDE/SATA Master)\n1 = Secondary HDD (DVD Port Slave)",
    "TDATA/UDATA Partition: (TUDATARedirPart)\n\nTarget Partition To Store TDATA/UDATA:\n1 = E:\n6 = F:\n7 = G:",
    "Enable Screenshot Capture (EnableScreenshots):\n\nCaptures A Screenshot Of The Current Framebuffer While In-Game. Only Supports XDK Apps & Games.\nSaves The Image To E:\\Cerbios\\Screenshots\\(Title ID) As A BMP Image.\nTriggered With L-Thumb + R-Thumb Combo Press.\n\nTrue  = Enable Screenshot Capture.\nFalse = Disabled.",
    "Screenshot HDD (ScreenshotHDD):\n\nSelect the target HDD for saving screenshots.\n0 = Primary HDD\n1 = Secondary HDD (if dual HDD setup used)",
    "Screenshot Partition (ScreenshotPart):\n\nChoose where to save screenshots:\nE:, F:, or G: partition.",
    "In-Game Controller Port (IGRMasterPort):\n\nDetermines Which Controller Port Can Trigger IGR Combos.\n\n0 = ALL Controllers\n1-4 = Restrict To Specific Controller Port",
    "In-Game Dash Combo (IGRDash):\n\nSet A 4-Digit Hex Combo To Trigger An In-Game Soft Reset Back To The Dashboard.\nKeeps Any Mounted ISO/CCI Or Game Image Active.",
    "In-Game Game Combo (IGRGame):\n\nSet A 4-Digit Hex Combo To Reload The Currently Running Game Or Application Without A Full Reboot.",
    "In-Game Full Combo (IGRFull):\n\nSet A 4-Digit Hex Combo To Perform A Full Kernel Reset.\nThis Will Unmount Any Loaded ISO/CCI Or Game Image And Return To The Dashboard.",
    "In-Game Cycle Combo (IGRCycle):\n\nSet A 4-Digit Hex Combo To Trigger A Full Power Cycle Of The Console.\nThis Simulates A Physical Reboot.",
    "In-Game Shutdown Combo (IGRShutdown):\n\nSet A 4-Digit Hex Combo To Trigger A Complete Shutdown Of The Console From In-Game.",
    "In-Game Screenshot Combo (IGRScreen):\n\nSet Upto A 4-Digit Hex Combo To Capture A Screenshot Of The Current Framebuffer.\n\nRequires 'EnableScreenshots' To Be Enabled.",
    "Debug Mode (Debug):\n\nEnables booting into C:\\xshell.xbe (XDK Launcher Dashboard) and automatically loads E:\\xbdm.dll to allow network debugging from Visual Studio & XDK.\n\nTrue  = XDK Debug Enabled.\nFalse = Retail Mode.",
    "Auto Off Timeout (AutoOffTimeout):\n\nSets A Custom Auto Power-Down Timeout, In Seconds.\nRequires The 'Auto Off' Feature To Be Enabled In The MS Dashboard.\n\n0 = Keep Stock Behaviour (6 Hours).\n30 Or Higher = Custom Timeout In Seconds.\n\nValues Between 1 And 29 Are Raised To 30.",
};

static const unsigned char lcdI2CAddresses[] = {
    0x20, 0x21, 0x22, 0x23,
    0x24, 0x25, 0x26, 0x27,
    0x38, 0x39, 0x3A, 0x3B,
    0x3C, 0x3D, 0x3E, 0x3F
};

static int getI2CAddrIndex(unsigned char value) {
    for (int i = 0; i < 16; i++) {
        if (lcdI2CAddresses[i] == value) {
            return i;
        }
    }
    return 0;
}

static const unsigned char hddParts[] = {
    1, 6, 7
};

static int getHddPartIndex(unsigned char value) {
    for (int i = 0; i < 3; i++) {
        if (hddParts[i] == value) {
            return i;
        }
    }
    return 0;
}

static char* shortenString(const char* value) {
    int tw, th;
    drawing::measureBitmapString(context::getBitmapFontSmall(), value, &tw, &th);
    char* tmp = strdup(value);
    if (tmp == NULL) {
        return NULL;
    }
    int maxW = (640 - 165) - 20;
    if (tw >= maxW) {
        for (int i = 0; i < (int)strlen(value); i++) {
            char* newTmp = stringUtility::formatString("...%s", value + i);
            if (newTmp == NULL) {
                break; // Keep the last valid tmp
            }
            free(tmp);
            tmp = newTmp;
            drawing::measureBitmapString(context::getBitmapFontSmall(), tmp, &tw, &th);
            if (tw < maxW)
                break;
        }
    }
    return tmp;
}

static void onAction(listItemAction* sender, void* context) {

    sceneContainer* container = new sceneContainer(sceneItemCerbiosIniEditorScene, new cerbiosIniEditorScene(sender->getId()), "Cerbios INI Editor");
    sceneManager::pushScene(container);
}

static const char* onGetPath(listItemPath* sender, void* context) {
    cerbiosIniEditorScene* self = (cerbiosIniEditorScene*)context;
    cerbiosConfig* config = self->getConfig();

    if (sender->getId() == OPT_DASH_PATH) {
        return config->DashPath;
    } else if (sender->getId() == OPT_BOOT_ANIM_PATH) {
        return config->BootAnimPath;
    }

    return "";
}

static void onSetPath(listItemPath* sender, void* context, const char* value) {
    cerbiosIniEditorScene* self = (cerbiosIniEditorScene*)context;
    cerbiosConfig* config = self->getConfig();
    mNeedsSave = true;

    if (sender->getId() == OPT_DASH_PATH) {
        strncpy(config->DashPath, value, 100 - 1);
    } else if (sender->getId() == OPT_BOOT_ANIM_PATH) {
        strncpy(config->BootAnimPath, value, 100 - 1);
    }
}

static void onFrontLedClosing(sceneResult result, void* context, scene* caller) {

    listItemScene* listItem = (listItemScene*)context;

    if (listItem->getId() == OPT_FRONT_LED)
    {
        frontLedSelectorScene* ledScene = (frontLedSelectorScene*)caller;
        listItem->setValue(ledScene->getLedSequence());
    }
}

static const char* onGetScene(listItemScene* sender, void* context) {
    cerbiosIniEditorScene* self = (cerbiosIniEditorScene*)context;
    cerbiosConfig* config = self->getConfig();

    if (sender->getId() == OPT_FRONT_LED) {
        return config->FrontLed;
    }
    return "";
}

static void onSetScene(listItemScene* sender, void* context, const char* value) {
    cerbiosIniEditorScene* self = (cerbiosIniEditorScene*)context;
    cerbiosConfig* config = self->getConfig();
    mNeedsSave = true;

    if (sender->getId() == OPT_FRONT_LED) {
        strncpy(config->FrontLed, value, 4);
    }
}

static unsigned int onGetEnum(listItemEnum* sender, void* context) {
    cerbiosIniEditorScene* self = (cerbiosIniEditorScene*)context;
    cerbiosConfig* config = self->getConfig();

    if (sender->getId() == OPT_FORCE_FLICKER_FILTER) {
        return config->ForceFFilter;
    } else if (sender->getId() == OPT_DRIVE_SETUP) {
        return config->DriveSetup;
    } else if (sender->getId() == OPT_UDMA_MASTER) {
        return config->UdmaModeMaster;
    } else if (sender->getId() == OPT_UDMA_SLAVE) {
        return config->UdmaModeSlave;
    } else if (sender->getId() == OPT_XTAL_FREQ) {
        return config->XtalFreq;
    } else if (sender->getId() == OPT_FAN_SPEED) {
        return min(config->FanSpeed / 10, 10);
    } else if (sender->getId() == OPT_LCD_BUS) {
        return config->LCDBus;
    } else if (sender->getId() == OPT_LCD_I2C_ADDR) {
        return getI2CAddrIndex(config->LCDI2CAddr);
    } else if (sender->getId() == OPT_LCD_PROTOCOL) {
        return config->LCDProto;
    } else if (sender->getId() == OPT_TDATA_UDATA_REDIRECT_HDD) {
        return config->TUDATARedirHDD;
    } else if (sender->getId() == OPT_TDATA_UDATA_REDIRECT_PART) {
        return getHddPartIndex(config->TUDATARedirPart);
    } else if (sender->getId() == OPT_SCREENSHOT_HDD) {
        return config->ScreenshotHDD;
    } else if (sender->getId() == OPT_SCREENSHOT_PART) {
        return getHddPartIndex(config->ScreenshotPart);
    } else if (sender->getId() == OPT_IN_GAME_CONTROLLER_PORT) {
        return config->IGRMasterPort;
    }

    return 0;
}

static void onSetEnum(listItemEnum* sender, void* context, unsigned int value) {
    cerbiosIniEditorScene* self = (cerbiosIniEditorScene*)context;
    cerbiosConfig* config = self->getConfig();
    mNeedsSave = true;

    if (sender->getId() == OPT_FORCE_FLICKER_FILTER) {
        config->ForceFFilter = value;
    } else if (sender->getId() == OPT_DRIVE_SETUP) {
        config->DriveSetup = value;
    } else if (sender->getId() == OPT_UDMA_MASTER) {
        config->UdmaModeMaster = value;
    } else if (sender->getId() == OPT_UDMA_SLAVE) {
        config->UdmaModeSlave = value;
    } else if (sender->getId() == OPT_XTAL_FREQ) {
        config->XtalFreq = value;
    } else if (sender->getId() == OPT_FAN_SPEED) {
        config->FanSpeed = value * 10;
    } else if (sender->getId() == OPT_LCD_BUS) {
        config->LCDBus = value;
        if (value == 1) {
            config->LCDProto = 0;
        }
    } else if (sender->getId() == OPT_LCD_I2C_ADDR) {
        config->LCDI2CAddr = lcdI2CAddresses[value];
    } else if (sender->getId() == OPT_LCD_PROTOCOL) {
        config->LCDProto = value;
    } else if (sender->getId() == OPT_TDATA_UDATA_REDIRECT_HDD) {
        config->TUDATARedirHDD = value;
    } else if (sender->getId() == OPT_TDATA_UDATA_REDIRECT_PART) {
        config->TUDATARedirPart = hddParts[value];
    } else if (sender->getId() == OPT_SCREENSHOT_HDD) {
        config->ScreenshotHDD = value;
    } else if (sender->getId() == OPT_SCREENSHOT_PART) {
        config->ScreenshotPart = hddParts[value];
    } else if (sender->getId() == OPT_IN_GAME_CONTROLLER_PORT) {
        config->IGRMasterPort = value;
    }
}

static unsigned int onGetHex(listItemHex* sender, void* context) {
    cerbiosIniEditorScene* self = (cerbiosIniEditorScene*)context;
    cerbiosConfig* config = self->getConfig();

    if (sender->getId() == OPT_CPUMPLL_COEFF) {
        return config->CPUMPLLCoeff;
    } else if (sender->getId() == OPT_NVPLL_COEFF) {
        return config->NVPLLCoeff;
    } else if (sender->getId() == OPT_DASH_COMBO) {
        return config->IGRDash;
    } else if (sender->getId() == OPT_GAME_COMBO) {
        return config->IGRGame;
    } else if (sender->getId() == OPT_FULL_COMBO) {
        return config->IGRFull;
    } else if (sender->getId() == OPT_POWER_CYCLE_COMBO) {
        return config->IGRCycle;
    } else if (sender->getId() == OPT_SHUTDOWN_COMBO) {
        return config->IGRShutdown;
    } else if (sender->getId() == OPT_SCREENSHOT_COMBO) {
        return config->IGRScreen;
    }

    return 0;
}

static void onSetHex(listItemHex* sender, void* context, unsigned int value) {
    cerbiosIniEditorScene* self = (cerbiosIniEditorScene*)context;
    cerbiosConfig* config = self->getConfig();
    mNeedsSave = true;

    if (sender->getId() == OPT_CPUMPLL_COEFF) {
        config->CPUMPLLCoeff = value;
    } else if (sender->getId() == OPT_NVPLL_COEFF) {
        config->NVPLLCoeff = value;
    } else if (sender->getId() == OPT_DASH_COMBO) {
        config->IGRDash = (unsigned short)value;
    } else if (sender->getId() == OPT_GAME_COMBO) {
        config->IGRGame = (unsigned short)value;
    } else if (sender->getId() == OPT_FULL_COMBO) {
        config->IGRFull = (unsigned short)value;
    } else if (sender->getId() == OPT_POWER_CYCLE_COMBO) {
        config->IGRCycle = (unsigned short)value;
    } else if (sender->getId() == OPT_SHUTDOWN_COMBO) {
        config->IGRShutdown = (unsigned short)value;
    } else if (sender->getId() == OPT_SCREENSHOT_COMBO) {
        config->IGRScreen = (unsigned short)value;
    }
}

static unsigned int onGetInteger(listItemInteger* sender, void* context) {
    cerbiosIniEditorScene* self = (cerbiosIniEditorScene*)context;
    cerbiosConfig* config = self->getConfig();

    if (sender->getId() == OPT_AUTO_OFF_TIMEOUT) {
        return config->AutoOffTimeout;
    }

    return 0;
}

static void onSetInteger(listItemInteger* sender, void* context, unsigned int value) {
    cerbiosIniEditorScene* self = (cerbiosIniEditorScene*)context;
    cerbiosConfig* config = self->getConfig();
    mNeedsSave = true;

    if (sender->getId() == OPT_AUTO_OFF_TIMEOUT) {
        if (value != 0 && value < 30) {
            value = 30;
        }
        config->AutoOffTimeout = value;
    }
}

static unsigned char onGetBool(listItemBool* sender, void* context) {
    cerbiosIniEditorScene* self = (cerbiosIniEditorScene*)context;
    cerbiosConfig* config = self->getConfig();

    if (sender->getId() == OPT_AV_CHECK) {
        return config->AVCheck;
    } else if (sender->getId() == OPT_FORCE_480P) {
        return config->Force480p;
    } else if (sender->getId() == OPT_FORCE_480I) {
        return config->Force480i;
    } else if (sender->getId() == OPT_FORCE_VGA) {
        return config->ForceVGA;
    } else if (sender->getId() == OPT_FAN_SPEED_OVERRIDE) {
        return config->OverrideFan;
    } else if (sender->getId() == OPT_OVERCLOCKING) {
        return config->Overclocking;
    } else if (sender->getId() == OPT_LCD_DISPLAY) {
        return config->InAppLCDEnable;
    } else if (sender->getId() == OPT_BLOCK_DASH_UPDATE) {
        return config->BlockDashUpdate;
    } else if (sender->getId() == OPT_XONLINE_DASH_REDIRECT) {
        return config->XonlineDashRedir;
    } else if (sender->getId() == OPT_ADV_CPU_SUPPORT) {
        return config->AdvCPUSupport;
    } else if (sender->getId() == OPT_XBE_MEM_DELIMIT) {
        return config->DisableLimitMem;
    } else if (sender->getId() == OPT_TITLE_PATCHES) {
        return config->ApplyTitlePatches;
    } else if (sender->getId() == OPT_READ_ONLY_C_PARTITION) {
        return config->ReadOnlyC;
    } else if (sender->getId() == OPT_READ_ONLY_F_PARTITION) {
        return config->ReadOnlyF;
    } else if (sender->getId() == OPT_READ_ONLY_G_PARTITION) {
        return config->ReadOnlyG;
    } else if (sender->getId() == OPT_TR_FW_SUPPORT) {
        return config->TrFwSupport;
    } else if (sender->getId() == OPT_FORCE_HDAV_PACK) {
        return config->ForceHDAVPack;
    } else if (sender->getId() == OPT_RESET_ON_EJECT) {
        return config->ResetOnEject;
    } else if (sender->getId() == OPT_RTC_ENABLE) {
        return config->RTCEnable;
    } else if (sender->getId() == OPT_TDATA_UDATA_REDIRECT) {
        return config->TUDATARedir;
    } else if (sender->getId() == OPT_SCREENSHOT_CAPTURE) {
        return config->EnableScreenshots;
    } else if (sender->getId() == OPT_DEBUG_MODE) {
        return config->Debug;
    }

    return 0;
}

static void onSetBool(listItemBool* sender, void* context, unsigned char value) {
    cerbiosIniEditorScene* self = (cerbiosIniEditorScene*)context;
    cerbiosConfig* config = self->getConfig();
    mNeedsSave = true;

    if (sender->getId() == OPT_AV_CHECK) {
        config->AVCheck = value;
    } else if (sender->getId() == OPT_FORCE_480P) {
        config->Force480p = value;
    } else if (sender->getId() == OPT_FORCE_480I) {
        config->Force480i = value;
    } else if (sender->getId() == OPT_FORCE_VGA) {
        config->ForceVGA = value;
    } else if (sender->getId() == OPT_FAN_SPEED_OVERRIDE) {
        config->OverrideFan = value;
    } else if (sender->getId() == OPT_OVERCLOCKING) {
        config->Overclocking = value;
        self->refreshPage();
    } else if (sender->getId() == OPT_LCD_DISPLAY) {
        config->InAppLCDEnable = value;
        self->refreshPage();
    } else if (sender->getId() == OPT_BLOCK_DASH_UPDATE) {
        config->BlockDashUpdate = value;
    } else if (sender->getId() == OPT_XONLINE_DASH_REDIRECT) {
        config->XonlineDashRedir = value;
    } else if (sender->getId() == OPT_ADV_CPU_SUPPORT) {
        config->AdvCPUSupport = value;
    } else if (sender->getId() == OPT_XBE_MEM_DELIMIT) {
        config->DisableLimitMem = value;
    } else if (sender->getId() == OPT_TITLE_PATCHES) {
        config->ApplyTitlePatches = value;
    } else if (sender->getId() == OPT_READ_ONLY_C_PARTITION) {
        config->ReadOnlyC = value;
    } else if (sender->getId() == OPT_READ_ONLY_F_PARTITION) {
        config->ReadOnlyF = value;
    } else if (sender->getId() == OPT_READ_ONLY_G_PARTITION) {
        config->ReadOnlyG = value;
    } else if (sender->getId() == OPT_TR_FW_SUPPORT) {
        config->TrFwSupport = value;
    } else if (sender->getId() == OPT_FORCE_HDAV_PACK) {
        config->ForceHDAVPack = value;
    } else if (sender->getId() == OPT_RESET_ON_EJECT) {
        config->ResetOnEject = value;
    } else if (sender->getId() == OPT_RTC_ENABLE) {
        config->RTCEnable = value;
    } else if (sender->getId() == OPT_TDATA_UDATA_REDIRECT) {
        config->TUDATARedir = value;
        self->refreshPage();
    } else if (sender->getId() == OPT_SCREENSHOT_CAPTURE) {
        config->EnableScreenshots = value;
        self->refreshPage();
    } else if (sender->getId() == OPT_DEBUG_MODE) {
        config->Debug = value;
    }
}

void cerbiosIniEditorScene::refreshPage()
{
    cerbiosConfig* config = getConfig();

    mSceneItems->clear();
    if (mPage == -1) {
        mSceneItems->add(new listItemAction(0, "Launch Paths", this, &onAction));
        mSceneItems->add(new listItemAction(1, "Video & Display", this, &onAction));
        mSceneItems->add(new listItemAction(2, "Storage & Drive Config", this, &onAction));
        mSceneItems->add(new listItemAction(3, "Cooling & Performance", this, &onAction));
        mSceneItems->add(new listItemAction(4, "Front Panel & Visuals", this, &onAction));
        mSceneItems->add(new listItemAction(5, "Enhancements", this, &onAction));
        mSceneItems->add(new listItemAction(6, "In-Game Key Combo", this, &onAction));
        mSceneItems->add(new listItemAction(7, "Debugging & Development", this, &onAction));
    } else if (mPage == 0) {
        mSceneItems->add(new listItemTitle(-1, "Launch Paths"));
        mSceneItems->add(new listItemPath(OPT_DASH_PATH, "Dash Path:", 100 - 1, this, &onGetPath, &onSetPath));
        mSceneItems->add(new listItemPath(OPT_BOOT_ANIM_PATH, "Boot Anim Path:", 100 - 1, this, &onGetPath, &onSetPath));
    } else if (mPage == 1) {
        mSceneItems->add(new listItemTitle(-1, "Video & Display"));
        mSceneItems->add(new listItemBool(OPT_AV_CHECK, "AV Check:", this, &onGetBool, &onSetBool));
        listItemEnum* forceFFilterEnum = new listItemEnum(OPT_FORCE_FLICKER_FILTER, "Force Flicker Filter:", this, &onGetEnum, &onSetEnum);
        forceFFilterEnum->addOption("Off");
        forceFFilterEnum->addOption("1");
        forceFFilterEnum->addOption("2");
        forceFFilterEnum->addOption("3");
        forceFFilterEnum->addOption("4");
        forceFFilterEnum->addOption("5");
        forceFFilterEnum->addOption("Use System/Game Default");
        mSceneItems->add(forceFFilterEnum);
        mSceneItems->add(new listItemBool(OPT_FORCE_480P, "Force 480p:", this, &onGetBool, &onSetBool));
        mSceneItems->add(new listItemBool(OPT_FORCE_480I, "Force 480i:", this, &onGetBool, &onSetBool));
        mSceneItems->add(new listItemBool(OPT_FORCE_VGA, "Force VGA:", this, &onGetBool, &onSetBool));
        mSceneItems->add(new listItemBool(OPT_TR_FW_SUPPORT, "TR HDMI FW Support:", this, &onGetBool, &onSetBool));
        mSceneItems->add(new listItemBool(OPT_FORCE_HDAV_PACK, "Force HDTV Pack:", this, &onGetBool, &onSetBool));
    } else if (mPage == 2) {
        mSceneItems->add(new listItemTitle(-1, "Storage & Drive Config"));
        listItemEnum* driveSetupEnum = new listItemEnum(OPT_DRIVE_SETUP, "Drive Setup:", this, &onGetEnum, &onSetEnum);
        driveSetupEnum->addOption("HDD & DVD");
        driveSetupEnum->addOption("HDD only (Legacy)");
        driveSetupEnum->addOption("HDD only (Modern)");
        driveSetupEnum->addOption("Dual HDD");
        mSceneItems->add(driveSetupEnum);
        listItemEnum* udmaModeMasterEnum = new listItemEnum(OPT_UDMA_MASTER, "UdmaModeMaster", this, &onGetEnum, &onSetEnum);
        udmaModeMasterEnum->addOption("Auto (Startech)");
        udmaModeMasterEnum->addOption("Auto (Generic)");
        udmaModeMasterEnum->addOption("UDMA Mode 2");
        udmaModeMasterEnum->addOption("UDMA Mode 3");
        udmaModeMasterEnum->addOption("UDMA Mode 4");
        udmaModeMasterEnum->addOption("UDMA Mode 5");
        udmaModeMasterEnum->addOption("UDMA Mode 6");
        mSceneItems->add(udmaModeMasterEnum);
        listItemEnum* udmaModeSlaveEnum = new listItemEnum(OPT_UDMA_SLAVE, "UdmaModeSlave", this, &onGetEnum, &onSetEnum);
        udmaModeSlaveEnum->addOption("Auto (Startech)");
        udmaModeSlaveEnum->addOption("Auto (Generic)");
        udmaModeSlaveEnum->addOption("UDMA Mode 2");
        udmaModeSlaveEnum->addOption("UDMA Mode 3");
        udmaModeSlaveEnum->addOption("UDMA Mode 4");
        udmaModeSlaveEnum->addOption("UDMA Mode 5");
        udmaModeSlaveEnum->addOption("UDMA Mode 6");
        mSceneItems->add(udmaModeSlaveEnum);
    } else if (mPage == 3) {
        mSceneItems->add(new listItemTitle(-1, "Cooling & Performance"));
        listItemEnum* fanSpeedEnum = new listItemEnum(OPT_FAN_SPEED, "Fan Speed:", this, &onGetEnum, &onSetEnum);
        fanSpeedEnum->addOption("Auto");
        fanSpeedEnum->addOption("10%");
        fanSpeedEnum->addOption("20%");
        fanSpeedEnum->addOption("30%");
        fanSpeedEnum->addOption("40%");
        fanSpeedEnum->addOption("50%");
        fanSpeedEnum->addOption("60%");
        fanSpeedEnum->addOption("70%");
        fanSpeedEnum->addOption("80%");
        fanSpeedEnum->addOption("90%");
        fanSpeedEnum->addOption("100%");
        mSceneItems->add(fanSpeedEnum);
        mSceneItems->add(new listItemBool(OPT_FAN_SPEED_OVERRIDE, "Fan Speed Overide:", this, &onGetBool, &onSetBool));
        listItemEnum* xtalFreqEnum = new listItemEnum(OPT_XTAL_FREQ, "XTAL Frequency:", this, &onGetEnum, &onSetEnum);
        xtalFreqEnum->addOption("Auto");
        xtalFreqEnum->addOption("Force 16.66MHz (Stock)");
        xtalFreqEnum->addOption("Force 13.5MHz");
        mSceneItems->add(xtalFreqEnum);
        mSceneItems->add(new listItemBool(OPT_OVERCLOCKING, "Overclocking:", this, &onGetBool, &onSetBool));
        if (config->Overclocking)
        {
            mSceneItems->add(new listItemHex(OPT_CPUMPLL_COEFF, "CPUMPLLCoeff", hexpadTypeCoeff, this, &onGetHex, &onSetHex));
            mSceneItems->add(new listItemHex(OPT_NVPLL_COEFF, "NVPLLCoeff", hexpadTypeCoeff, this, &onGetHex, &onSetHex));
        }
    } else if (mPage == 4) {
        mSceneItems->add(new listItemTitle(-1, "Front Panel & Visuals"));
        mSceneItems->add(new listItemScene(OPT_FRONT_LED, "Front Led:", sceneItemFrontLedSelectorScene, "", this, &onGetScene, &onSetScene, &onFrontLedClosing));
        mSceneItems->add(new listItemBool(OPT_LCD_DISPLAY, "LCD Display:", this, &onGetBool, &onSetBool));
        if (config->InAppLCDEnable)
        {
            listItemEnum* lcdBusEnum = new listItemEnum(OPT_LCD_BUS, "LCD Bus:", this, &onGetEnum, &onSetEnum);
            lcdBusEnum->addOption("Xbox's SMBus");
            lcdBusEnum->addOption("X3LCD Via X3 Modchip");
            mSceneItems->add(lcdBusEnum);
            listItemEnum* lcdI2CAddrEnum = new listItemEnum(OPT_LCD_I2C_ADDR, "LCD I2C Addr:", this, &onGetEnum, &onSetEnum);
            lcdI2CAddrEnum->addOption("0x20");
            lcdI2CAddrEnum->addOption("0x21");
            lcdI2CAddrEnum->addOption("0x22");
            lcdI2CAddrEnum->addOption("0x23");
            lcdI2CAddrEnum->addOption("0x24");
            lcdI2CAddrEnum->addOption("0x25");
            lcdI2CAddrEnum->addOption("0x26");
            lcdI2CAddrEnum->addOption("0x27");
            lcdI2CAddrEnum->addOption("0x38");
            lcdI2CAddrEnum->addOption("0x39");
            lcdI2CAddrEnum->addOption("0x3A");
            lcdI2CAddrEnum->addOption("0x3B");
            lcdI2CAddrEnum->addOption("0x3C");
            lcdI2CAddrEnum->addOption("0x3D");
            lcdI2CAddrEnum->addOption("0x3E");
            lcdI2CAddrEnum->addOption("0x3F");
            mSceneItems->add(lcdI2CAddrEnum);
            listItemEnum* lcdI2CProtoEnum = new listItemEnum(OPT_LCD_PROTOCOL, "LCD Protocol:", this, &onGetEnum, &onSetEnum);
            lcdI2CProtoEnum->addOption("HD44780 (LCD2004) Or X3LCD");
            lcdI2CProtoEnum->addOption("US2066 (NHD-0420CW)");
            mSceneItems->add(lcdI2CProtoEnum);
        }
    } else if (mPage == 5) {
        mSceneItems->add(new listItemTitle(-1, "Enhancements"));
        mSceneItems->add(new listItemBool(OPT_BLOCK_DASH_UPDATE, "Block Dash Update:", this, &onGetBool, &onSetBool));
        mSceneItems->add(new listItemBool(OPT_XONLINE_DASH_REDIRECT, "Xonline Dash Redirect:", this, &onGetBool, &onSetBool));
        mSceneItems->add(new listItemBool(OPT_ADV_CPU_SUPPORT, "Adv CPU Support:", this, &onGetBool, &onSetBool));
        mSceneItems->add(new listItemBool(OPT_XBE_MEM_DELIMIT, "XBE Mem Delimit:", this, &onGetBool, &onSetBool));
        mSceneItems->add(new listItemBool(OPT_TITLE_PATCHES, "Title Patches:", this, &onGetBool, &onSetBool));
        mSceneItems->add(new listItemBool(OPT_READ_ONLY_C_PARTITION, "Read-Only C Partition:", this, &onGetBool, &onSetBool));
        mSceneItems->add(new listItemBool(OPT_READ_ONLY_F_PARTITION, "Read-Only F Partition:", this, &onGetBool, &onSetBool));
        mSceneItems->add(new listItemBool(OPT_READ_ONLY_G_PARTITION, "Read-Only G Partition:", this, &onGetBool, &onSetBool));
        mSceneItems->add(new listItemBool(OPT_RESET_ON_EJECT, "Reset On Eject", this, &onGetBool, &onSetBool));
        mSceneItems->add(new listItemBool(OPT_RTC_ENABLE, "RTC Enable:", this, &onGetBool, &onSetBool));
        mSceneItems->add(new listItemInteger(OPT_AUTO_OFF_TIMEOUT, "Auto Off Timeout:", (7 * 86400) + 86399, this, &onGetInteger, &onSetInteger, true, 30, "Stock (6 Hours)"));
        mSceneItems->add(new listItemBool(OPT_TDATA_UDATA_REDIRECT, "TDATA/UDATA Redirect:", this, &onGetBool, &onSetBool));
        if (config->TUDATARedir)
        {
            listItemEnum* tuDataRedirHDDEnum = new listItemEnum(OPT_TDATA_UDATA_REDIRECT_HDD, "TDATA/UDATA Redirect HDD:", this, &onGetEnum, &onSetEnum);
            tuDataRedirHDDEnum->addOption("Master");
            tuDataRedirHDDEnum->addOption("Slave (Dual-Hdd)");
            mSceneItems->add(tuDataRedirHDDEnum);
            listItemEnum* tuDataRedirPartEnum = new listItemEnum(OPT_TDATA_UDATA_REDIRECT_PART, "TDATA/UDATA Redirect Partition:", this, &onGetEnum, &onSetEnum);
            tuDataRedirPartEnum->addOption("E:");
            tuDataRedirPartEnum->addOption("F:");
            tuDataRedirPartEnum->addOption("G:");
            mSceneItems->add(tuDataRedirPartEnum);
        }
        mSceneItems->add(new listItemBool(OPT_SCREENSHOT_CAPTURE, "Screenshot Capture:", this, &onGetBool, &onSetBool));
        if (config->EnableScreenshots)
        {
            listItemEnum* screenshotHDDEnum = new listItemEnum(OPT_SCREENSHOT_HDD, "Screenshot HDD:", this, &onGetEnum, &onSetEnum);
            screenshotHDDEnum->addOption("Master");
            screenshotHDDEnum->addOption("Slave (Dual-Hdd)");
            mSceneItems->add(screenshotHDDEnum);
            listItemEnum* screenshotPartEnum = new listItemEnum(OPT_SCREENSHOT_PART, "Screenshot Partition:", this, &onGetEnum, &onSetEnum);
            screenshotPartEnum->addOption("E:");
            screenshotPartEnum->addOption("F:");
            screenshotPartEnum->addOption("G:");
            mSceneItems->add(screenshotPartEnum);
        }
    } else if (mPage == 6) {
        mSceneItems->add(new listItemTitle(-1, "In-Game Key Combo"));
        listItemEnum* igrMasterPorttEnum = new listItemEnum(OPT_IN_GAME_CONTROLLER_PORT, "In-Game Controller Port:", this, &onGetEnum, &onSetEnum);
        igrMasterPorttEnum->addOption("All");
        igrMasterPorttEnum->addOption("Controller 1");
        igrMasterPorttEnum->addOption("Controller 2");
        igrMasterPorttEnum->addOption("Controller 3");
        igrMasterPorttEnum->addOption("Controller 4");
        mSceneItems->add(igrMasterPorttEnum);
        mSceneItems->add(new listItemHex(OPT_DASH_COMBO, "Dash Combo:", hexpadTypeIGR, this, &onGetHex, &onSetHex));
        mSceneItems->add(new listItemHex(OPT_GAME_COMBO, "Game Combo:", hexpadTypeIGR, this, &onGetHex, &onSetHex));
        mSceneItems->add(new listItemHex(OPT_FULL_COMBO, "Full Combo:", hexpadTypeIGR, this, &onGetHex, &onSetHex));
        mSceneItems->add(new listItemHex(OPT_POWER_CYCLE_COMBO, "Power Cycle Combo:", hexpadTypeIGR, this, &onGetHex, &onSetHex));
        mSceneItems->add(new listItemHex(OPT_SHUTDOWN_COMBO, "Shutdown Combo:", hexpadTypeIGR, this, &onGetHex, &onSetHex));
        mSceneItems->add(new listItemHex(OPT_SCREENSHOT_COMBO, "Screenshot Combo:", hexpadTypeIGR, this, &onGetHex, &onSetHex));
    } else if (mPage == 7) {
        mSceneItems->add(new listItemTitle(-1, "Debugging & Development"));
        mSceneItems->add(new listItemBool(OPT_DEBUG_MODE, "Debug Mode:", this, &onGetBool, &onSetBool));
    }
}

cerbiosIniEditorScene::cerbiosIniEditorScene(int page) {
    pointerVector<listItem*>* settings = new pointerVector<listItem*>(true);
    mSceneItems = settings;

    mShowInfo = false;
    mPage = page;

    if (mPage == -1)
    {
        mConfig = cerbiosIniHelper::loadConfig();
        mNeedsSave = false;
    }

    refreshPage();

    mSelectedControl = 0;
    while (mSelectedControl < (int)settings->count()) {
        if (settings->get(mSelectedControl)->getType() == LISTITEM_TITLE) {
            mSelectedControl++;
            continue;
        }
        break;
    }
}

cerbiosIniEditorScene::~cerbiosIniEditorScene() {
    delete (mSceneItems);
}

void cerbiosIniEditorScene::update() {
    // Exit Action
    if (inputManager::buttonPressed(JoystickButtonB)) {
        sceneManager::popScene();
        return; // important: don't touch this scene after popping it
    }

    // Safety: no items or bad index? bail out before touching mSceneItems
    int count = (int)mSceneItems->count();
    if (count == 0) {
        return;
    }
    if (mSelectedControl < 0) {
        mSelectedControl = 0;
    } else if (mSelectedControl >= count) {
        mSelectedControl = count - 1;
    }

    // Current item shortcut
    listItem* selected = mSceneItems->get(mSelectedControl);
    int type = selected->getType();

    // --- A: only for items that actually use A (no sound on BOOL/ENUM rows) ---
    if ((type == LISTITEM_TEXT  ||
         type == LISTITEM_PATH  ||
         type == LISTITEM_SCENE ||
         type == LISTITEM_HEX   ||
         type == LISTITEM_INTEGER ||
         type == LISTITEM_ACTION) &&
        inputManager::buttonPressed(JoystickButtonA))
    {
        if (type == LISTITEM_TEXT) {
            listItemText* item = (listItemText*)selected;
            item->showEditor();
        } else if (type == LISTITEM_PATH) {
            listItemPath* item = (listItemPath*)selected;
            item->showEditor();
        } else if (type == LISTITEM_SCENE) {
            listItemScene* item = (listItemScene*)selected;
            item->executeScene();
        } else if (type == LISTITEM_HEX) {
            listItemHex* item = (listItemHex*)selected;
            item->showEditor();
        } else if (type == LISTITEM_INTEGER) {
            listItemInteger* item = (listItemInteger*)selected;
            item->showEditor();
        } else if (type == LISTITEM_ACTION) {
            listItemAction* item = (listItemAction*)selected;
            item->executeAction();
        }
    }

    // --- White: only on page 0 AND only if current item is a PATH ---
    if (mPage == 0 &&
        type == LISTITEM_PATH &&
        inputManager::buttonPressed(JoystickButtonWhite))
    {
        listItemPath* item = (listItemPath*)selected;
        item->showFilePicker();
    }

    // --- Black: only if we're not on the main page AND current item has info (id >= 0) ---
    if (mPage != -1 &&
        selected->getId() >= 0 &&
        inputManager::buttonPressed(JoystickButtonBlack))
    {
        mShowInfo = !mShowInfo;
    }

    // --- Left trigger: only if current item is BOOL or ENUM ---
    if ((type == LISTITEM_BOOL ||
         type == LISTITEM_ENUM) &&
        inputManager::buttonPressed(JoystickButtonLeftTrigger))
    {
        if (type == LISTITEM_BOOL) {
            listItemBool* item = (listItemBool*)selected;
            item->toggleValue();
        } else if (type == LISTITEM_ENUM) {
            listItemEnum* item = (listItemEnum*)selected;
            unsigned int index = item->getValue();
            index = index == 0 ? (item->getCount() - 1) : index - 1;
            item->setValue(index);
        }
    }

    // --- Right trigger: only if current item is BOOL or ENUM ---
    if ((type == LISTITEM_BOOL ||
         type == LISTITEM_ENUM) &&
        inputManager::buttonPressed(JoystickButtonRightTrigger))
    {
        if (type == LISTITEM_BOOL) {
            listItemBool* item = (listItemBool*)selected;
            item->toggleValue();
        } else if (type == LISTITEM_ENUM) {
            listItemEnum* item = (listItemEnum*)selected;
            unsigned int index = item->getValue();
            index = index == (item->getCount() - 1) ? 0 : index + 1;
            item->setValue(index);
        }
    }

    // --- Apply Actions (global actions, sound always makes sense) ---
    if (inputManager::buttonPressed(JoystickButtonY)) {
        if (mPage == -1) 
        {
            cerbiosIniHelper::setConfigDefault(&mConfig);
        }
        else
        {
            if (mSceneItems->get(mSelectedControl)->getId() == OPT_DASH_PATH)
            {
                strcpy(mConfig.DashPath, "HDD0-C:\\evoxdash.xbe");
            }
            else if (mSceneItems->get(mSelectedControl)->getId() == OPT_BOOT_ANIM_PATH)
            {
                strcpy(mConfig.BootAnimPath, "HDD0-E:\\Cerbios\\BootAnims\\Xbox\\bootanim.xbe");
            }
            else if (mSceneItems->get(mSelectedControl)->getId() == OPT_AV_CHECK)
            {
                mConfig.AVCheck = 1;
            }
            else if (mSceneItems->get(mSelectedControl)->getId() == OPT_FORCE_FLICKER_FILTER)
            {
                mConfig.ForceFFilter = 6;
            }
            else if (mSceneItems->get(mSelectedControl)->getId() == OPT_FORCE_480P)
            {
                mConfig.Force480p = 0;
            }
            else if (mSceneItems->get(mSelectedControl)->getId() == OPT_FORCE_480I)
            {
                mConfig.Force480i = 0;
            }
            else if (mSceneItems->get(mSelectedControl)->getId() == OPT_FORCE_VGA)
            {
                mConfig.ForceVGA = 0;
            }
            else if (mSceneItems->get(mSelectedControl)->getId() == OPT_DRIVE_SETUP)
            {
                mConfig.DriveSetup = 1;
            }
            else if (mSceneItems->get(mSelectedControl)->getId() == OPT_UDMA_MASTER)
            {
                mConfig.UdmaModeMaster = 2;
            }
            else if (mSceneItems->get(mSelectedControl)->getId() == OPT_UDMA_SLAVE)
            {
                mConfig.UdmaModeSlave = 2;
            }
            else if (mSceneItems->get(mSelectedControl)->getId() == OPT_FAN_SPEED)
            {
                mConfig.FanSpeed = 0;
            }
            else if (mSceneItems->get(mSelectedControl)->getId() == OPT_FAN_SPEED_OVERRIDE)
            {
                mConfig.OverrideFan = 1;
            }
            else if (mSceneItems->get(mSelectedControl)->getId() == OPT_XTAL_FREQ)
            {
                mConfig.XtalFreq = 0;
            }
            else if (mSceneItems->get(mSelectedControl)->getId() == OPT_OVERCLOCKING)
            {
                mConfig.Overclocking = 0;
            }
            else if (mSceneItems->get(mSelectedControl)->getId() == OPT_CPUMPLL_COEFF)
            {
                mConfig.CPUMPLLCoeff = 0x000000;
            }
            else if (mSceneItems->get(mSelectedControl)->getId() == OPT_NVPLL_COEFF)
            {
                mConfig.NVPLLCoeff = 0x000000;
            }
            else if (mSceneItems->get(mSelectedControl)->getId() == OPT_FRONT_LED)
            {
                strcpy(mConfig.FrontLed, "GGGG");
            }
            else if (mSceneItems->get(mSelectedControl)->getId() == OPT_LCD_DISPLAY)
            {
                mConfig.InAppLCDEnable = 0;
            }
            else if (mSceneItems->get(mSelectedControl)->getId() == OPT_LCD_BUS)
            {
                mConfig.LCDBus = 0;
            }
            else if (mSceneItems->get(mSelectedControl)->getId() == OPT_LCD_I2C_ADDR)
            {
                mConfig.LCDI2CAddr = 0x3C;
            }
            else if (mSceneItems->get(mSelectedControl)->getId() == OPT_LCD_PROTOCOL)
            {
                mConfig.LCDProto = 0;
            }
            else if (mSceneItems->get(mSelectedControl)->getId() == OPT_BLOCK_DASH_UPDATE)
            {
                mConfig.BlockDashUpdate = 0;
            }
            else if (mSceneItems->get(mSelectedControl)->getId() == OPT_XONLINE_DASH_REDIRECT)
            {
                mConfig.XonlineDashRedir = 0;
            }
            else if (mSceneItems->get(mSelectedControl)->getId() == OPT_AUTO_OFF_TIMEOUT)
            {
                mConfig.AutoOffTimeout = 0;
            }
            else if (mSceneItems->get(mSelectedControl)->getId() == OPT_ADV_CPU_SUPPORT)
            {
                mConfig.AdvCPUSupport = 1;
            }
            else if (mSceneItems->get(mSelectedControl)->getId() == OPT_XBE_MEM_DELIMIT)
            {
                mConfig.DisableLimitMem = 0;
            }
            else if (mSceneItems->get(mSelectedControl)->getId() == OPT_TITLE_PATCHES)
            {
                mConfig.ApplyTitlePatches = 1;
            }
            else if (mSceneItems->get(mSelectedControl)->getId() == OPT_READ_ONLY_C_PARTITION)
            {
                mConfig.ReadOnlyC = 0;
            }
            else if (mSceneItems->get(mSelectedControl)->getId() == OPT_READ_ONLY_F_PARTITION)
            {
                mConfig.ReadOnlyF = 0;
            }
            else if (mSceneItems->get(mSelectedControl)->getId() == OPT_READ_ONLY_G_PARTITION)
            {
                mConfig.ReadOnlyG = 0;
            }
            else if (mSceneItems->get(mSelectedControl)->getId() == OPT_TR_FW_SUPPORT)
            {
                mConfig.TrFwSupport = 0;
            }
            else if (mSceneItems->get(mSelectedControl)->getId() == OPT_FORCE_HDAV_PACK)
            {
                mConfig.ForceHDAVPack = 0;
            }
            else if (mSceneItems->get(mSelectedControl)->getId() == OPT_RESET_ON_EJECT)
            {
                mConfig.ResetOnEject = 0;
            }
            else if (mSceneItems->get(mSelectedControl)->getId() == OPT_RTC_ENABLE)
            {
                mConfig.RTCEnable = 0;
            }
            else if (mSceneItems->get(mSelectedControl)->getId() == OPT_TDATA_UDATA_REDIRECT)
            {
                mConfig.TUDATARedir = 0;
            }
            else if (mSceneItems->get(mSelectedControl)->getId() == OPT_TDATA_UDATA_REDIRECT_HDD)
            {
                mConfig.TUDATARedirHDD = 0;
            }
            else if (mSceneItems->get(mSelectedControl)->getId() == OPT_TDATA_UDATA_REDIRECT_PART)
            {
                mConfig.TUDATARedirPart = 0;
            }
            else if (mSceneItems->get(mSelectedControl)->getId() == OPT_SCREENSHOT_CAPTURE)
            {
                mConfig.EnableScreenshots = 0;
            }
            else if (mSceneItems->get(mSelectedControl)->getId() == OPT_SCREENSHOT_HDD)
            {
                mConfig.ScreenshotHDD = 0;
            }
            else if (mSceneItems->get(mSelectedControl)->getId() == OPT_SCREENSHOT_PART)
            {
                mConfig.ScreenshotPart = 0;
            }
            else if (mSceneItems->get(mSelectedControl)->getId() == OPT_IN_GAME_CONTROLLER_PORT)
            {
                mConfig.IGRMasterPort = 0;
            }
            else if (mSceneItems->get(mSelectedControl)->getId() == OPT_DASH_COMBO)
            {
                mConfig.IGRDash = 0xC030;
            }
            else if (mSceneItems->get(mSelectedControl)->getId() == OPT_GAME_COMBO)
            {
                mConfig.IGRGame = 0xD010;
            }
            else if (mSceneItems->get(mSelectedControl)->getId() == OPT_FULL_COMBO)
            {
                mConfig.IGRFull = 0xD020;
            }
            else if (mSceneItems->get(mSelectedControl)->getId() == OPT_POWER_CYCLE_COMBO)
            {
                mConfig.IGRCycle = 0xC021;
            }
            else if (mSceneItems->get(mSelectedControl)->getId() == OPT_SHUTDOWN_COMBO)
            {
                mConfig.IGRShutdown = 0xD001;
            }
            else if (mSceneItems->get(mSelectedControl)->getId() == OPT_SCREENSHOT_COMBO)
            {
                mConfig.IGRScreen = 0x00C0;
            }
            else if (mSceneItems->get(mSelectedControl)->getId() == OPT_DEBUG_MODE)
            {
                mConfig.Debug = 0;
            }
        }
        mNeedsSave = true;
        refreshPage();
    }

    if (inputManager::buttonPressed(JoystickButtonX)) {
        char* buf = (char*)malloc(65536);
        cerbiosIniHelper::buildConfig(&mConfig, buf);
        cerbiosIniHelper::saveConfig(buf);
        free(buf);
        mNeedsSave = false;
    }

    // --- Down Actions ---
    if (inputManager::buttonPressed(JoystickButtonDpadDown)) {
        mSelectedControl = (mSelectedControl + 1) % count;
        while (mSelectedControl < count) {
            if (mSceneItems->get(mSelectedControl)->getType() == LISTITEM_TITLE) {
                mSelectedControl = (mSelectedControl + 1) % count;
                continue;
            }
            break;
        }
    }

    // --- Up Actions ---
    if (inputManager::buttonPressed(JoystickButtonDpadUp)) {
        mSelectedControl = (mSelectedControl - 1 + count) % count;
        while (mSelectedControl >= 0) {
            if (mSceneItems->get(mSelectedControl)->getType() == LISTITEM_TITLE) {
                mSelectedControl = (mSelectedControl - 1 + count) % count;
                continue;
            }
            break;
        }
    }
}

void cerbiosIniEditorScene::render() {
    component::panel(theme::getPanelFillColor(), theme::getPanelStrokeColor(), 16, 16, 688, 448);
    drawing::drawBitmapStringAligned(context::getBitmapFontMedium(), "Cerbios INI Editor", theme::getHeaderTextColor(),
        theme::getHeaderAlign(), 40, theme::getHeaderY(), 640);

    int32_t maxItems = 8;

    int32_t start = 0;
    if ((int32_t)mSceneItems->count() >= maxItems) {
        start = min(max(mSelectedControl - (maxItems / 2), 0), (int32_t)mSceneItems->count() - maxItems);
    }

    int buttonWidth = mPage == 0 ? 640 : 480;
    int buttonOffset = mPage == 0 ? 40 : 120;

    int32_t itemCount = min(start + maxItems, (int32_t)mSceneItems->count()) - start;
    if (itemCount > 0) {
        uint32_t yPos = (context::getBufferHeight() - ((itemCount * 40) - 10)) / 2;
        yPos += theme::getCenterOffset();

        for (int32_t i = 0; i < itemCount; i++) {
            uint32_t index = start + i;
            listItem* item = mSceneItems->get(index);
            if (item->getType() == LISTITEM_TITLE) {
                component::text(item->getName(), false, horizAlignmentLeft, buttonOffset, yPos, buttonWidth, 30);
            } else if (item->getType() == LISTITEM_TEXT) {
                listItemText* tempItem = (listItemText*)item;
                component::splitButton(
                    mSelectedControl == index, false, item->getName(), 150, tempItem->getValue(), buttonOffset, yPos, buttonWidth, 30, true);
            } else if (item->getType() == LISTITEM_PATH) {
                listItemPath* tempItem = (listItemPath*)item;
                char* shortPath = shortenString(tempItem->getValue());
                component::splitButton(
                    mSelectedControl == index, false, item->getName(), 150, shortPath, buttonOffset, yPos, buttonWidth, 30, true);
                free(shortPath);
            } else if (item->getType() == LISTITEM_SCENE) {
                listItemScene* tempItem = (listItemScene*)item;
                component::splitButton(
                    mSelectedControl == index, false, item->getName(), 150, tempItem->getValue(), buttonOffset, yPos, buttonWidth, 30, true);
            } else if (item->getType() == LISTITEM_HEX) {
                listItemHex* tempItem = (listItemHex*)item;
                if (tempItem->getHexpadType() == hexpadTypeIGR) {
                    char* igrValue = cerbiosIniHelper::shortToIGR((unsigned short)tempItem->getValue());
                    component::splitButton(
                        mSelectedControl == index, false, item->getName(), 150, igrValue, buttonOffset, yPos, buttonWidth, 30, true);
                    free(igrValue);
                } else {
                    char* hexValue = stringUtility::formatString("%06X", tempItem->getValue());
                    component::splitButton(
                        mSelectedControl == index, false, item->getName(), 150, hexValue, buttonOffset, yPos, buttonWidth, 30, true);
                    free(hexValue);
                }
            } else if (item->getType() == LISTITEM_INTEGER) {
                listItemInteger* tempItem = (listItemInteger*)item;
                char* intValue = tempItem->getDisplayValue();
                component::splitButton(
                    mSelectedControl == index, false, item->getName(), 150, intValue, buttonOffset, yPos, buttonWidth, 30, true);
                free(intValue);
            } else if (item->getType() == LISTITEM_BOOL) {
                listItemBool* tempItem = (listItemBool*)item;
                component::splitButton(
                    mSelectedControl == index, false, item->getName(), 150, tempItem->getValueString(), buttonOffset, yPos, buttonWidth, 30, true);
            } else if (item->getType() == LISTITEM_ENUM) {
                listItemEnum* tempItem = (listItemEnum*)item;
                component::splitButton(
                    mSelectedControl == index, false, item->getName(), 150, tempItem->getValueString(), buttonOffset, yPos, buttonWidth, 30, true);
            } else if (item->getType() == LISTITEM_ACTION) {
                listItemAction* tempItem = (listItemAction*)item;
                component::button(mSelectedControl == index, false, item->getName(), buttonOffset, yPos, buttonWidth, 30);
            }
            yPos += 40;
        }
    } else {
        int yPos = ((context::getBufferHeight() - 44) / 2);
        yPos += theme::getCenterOffset();

        component::textBox("No items", false, false, horizAlignmentCenter, 193, yPos, 322, 44, false, false, false);
    }

    if (mShowInfo && mSceneItems->get(mSelectedControl)->getId() > -1) {
        component::textBox(optionInfoTexts[mSceneItems->get(mSelectedControl)->getId()], true, false, horizAlignmentLeft, 60, theme::getCenterOffset() + 110, 600, 260, true, true, true);
    }

    char* caption = NULL;
    if (mPage == -1) {
        caption = strdup("\xC2\xA1 Select \xC2\xA4 Default All");
    } else {
        if (mSceneItems->get(mSelectedControl)->getType() == LISTITEM_BOOL || mSceneItems->get(mSelectedControl)->getType() == LISTITEM_ENUM) {
            caption = strdup("\xC2\xB2\xC2\xB3 Change Value \xC2\xA4 Default");
        } else if (mSceneItems->get(mSelectedControl)->getType() == LISTITEM_PATH) {
            caption = strdup("\xC2\xA1 Edit \xC2\xB5 Browse \xC2\xA4 Default");
        } else if (mSceneItems->get(mSelectedControl)->getType() == LISTITEM_ACTION) {
            caption = strdup("\xC2\xA1 Select \xC2\xA4 Default");
        } else {
            caption = strdup("\xC2\xA1 Edit \xC2\xA4 Default");
        }
    }

    if (caption != NULL)
    {
        if (mNeedsSave) {
            char* saveCaption = stringUtility::formatString("%s \xC2\xA3 Save", caption);
            utils::swapString(&caption, saveCaption);
            free(saveCaption);
        }
        drawing::drawBitmapString(context::getBitmapFontSmall(), caption, theme::getFooterTextColor(), 40, theme::getFooterY());
        free(caption);
    }

    drawing::drawBitmapStringAligned(context::getBitmapFontSmall(), mPage == -1 ? "\xC2\xA2 Back" : "\xC2\xB6 Info \xC2\xA2 Back", theme::getFooterTextColor(), horizAlignmentRight, 40, theme::getFooterY(), 640);
}

cerbiosConfig* cerbiosIniEditorScene::getConfig() {
    return &mConfig;
}
