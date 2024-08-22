#include "xboxConfig.h"
#include "xboxinternals.h"
#include "stringUtility.h"
#include "utils.h"
#include "XKUtils\XKEEPROM.h"

namespace
{
    ULONG mVideoFlags;
    ULONG mAudioFlags;
	ULONG mGameRegion;
	ULONG mDvdRegion;
	ULONG mVideoStandard;
	char* mSerial;
	char* mMac;
	bool mInitialized = false;
}

void xboxConfig::init()
{
	if (mInitialized == false)
	{
		XKEEPROM* eeprom = new XKEEPROM();
		eeprom->ReadFromXBOX();
		mGameRegion = (ULONG)eeprom->GetXBERegionVal();
		mAudioFlags = (ULONG)eeprom->GetAudioFlagsVal();
		mVideoFlags = (ULONG)eeprom->GetVideoFlagsVal();
		mVideoStandard = (ULONG)eeprom->GetVideoStandardVal();
		mDvdRegion = (ULONG)eeprom->GetDVDRegionVal();
		mMac = eeprom->GetMACAddressString();
		mSerial = eeprom->GetSerialNumberString();
		delete(eeprom);

		mInitialized = true;
	}
}

void xboxConfig::save() 
{
	XKEEPROM* eeprom = new XKEEPROM();
	eeprom->ReadFromXBOX();
	eeprom->SetXBERegionVal((XKEEPROM::XBE_REGION)mGameRegion);
	eeprom->SetAudioFlagsVal(mAudioFlags);
	eeprom->SetVideoFlagsVal(mVideoFlags);
	eeprom->SetVideoStandardVal(mVideoStandard);
	eeprom->SetDVDRegionVal(mDvdRegion);
	eeprom->WriteToXBOX();
	delete(eeprom);
}

void xboxConfig::setVideo480p(bool enable) 
{
    mVideoFlags = (mVideoFlags & ~EEPROM_VIDEO_FLAGS_HDTV_480p) + (enable ? EEPROM_VIDEO_FLAGS_HDTV_480p : 0);
	save();
}

bool xboxConfig::getVideo480p() 
{
	return (mVideoFlags & EEPROM_VIDEO_FLAGS_HDTV_480p) > 0;
}

void xboxConfig::setVideo720p(bool enable) 
{
    mVideoFlags = (mVideoFlags & ~EEPROM_VIDEO_FLAGS_HDTV_720p) + (enable ? EEPROM_VIDEO_FLAGS_HDTV_720p : 0);
	save();
}

bool xboxConfig::getVideo720p() 
{
	return (mVideoFlags & EEPROM_VIDEO_FLAGS_HDTV_720p) > 0;
}

void xboxConfig::setVideo1080i(bool enable) 
{
    mVideoFlags = (mVideoFlags & ~EEPROM_VIDEO_FLAGS_HDTV_1080i) + (enable ? EEPROM_VIDEO_FLAGS_HDTV_1080i : 0);
	save();
}

bool xboxConfig::getVideo1080i() 
{
	return (mVideoFlags & EEPROM_VIDEO_FLAGS_HDTV_1080i) > 0;
}

void xboxConfig::setVideoAspectRatio(uint32_t flag) 
{
    mVideoFlags = (mVideoFlags & ~(EEPROM_VIDEO_FLAGS_WIDESCREEN | EEPROM_VIDEO_FLAGS_LETTERBOX)) + flag;
	save();
}

bool xboxConfig::getVideoAspectRatioNormal() 
{
	return (mVideoFlags & (EEPROM_VIDEO_FLAGS_WIDESCREEN | EEPROM_VIDEO_FLAGS_LETTERBOX)) == 0;
}

bool xboxConfig::getVideoAspectRatioWidescreen() 
{
	return (mVideoFlags & EEPROM_VIDEO_FLAGS_WIDESCREEN) > 0;
}

bool xboxConfig::getVideoAspectRatioLetterbox() 
{
	return (mVideoFlags & EEPROM_VIDEO_FLAGS_LETTERBOX) > 0;
}

void xboxConfig::setVideo60hz(bool enable)
{
	ULONG flag = enable ? EEPROM_VIDEO_FLAGS_60Hz : 0;
	mVideoFlags = (mVideoFlags & ~EEPROM_VIDEO_FLAGS_60Hz) + flag;
	save();
}

bool xboxConfig::getVideo50hz()
{
	return (mVideoFlags & EEPROM_VIDEO_FLAGS_60Hz) == 0;
}

bool xboxConfig::getVideo60hz()
{
	return (mVideoFlags & EEPROM_VIDEO_FLAGS_60Hz) > 0;
}

void xboxConfig::setAudioMode(uint32_t flag) 
{
    mAudioFlags = (mAudioFlags & ~(EEPROM_AUDIO_FLAGS_STEREO | EEPROM_AUDIO_FLAGS_MONO | EEPROM_AUDIO_FLAGS_SURROUND)) + flag;
	save();
}

bool xboxConfig::getAudioModeStereo() 
{
	return (mAudioFlags & (EEPROM_AUDIO_FLAGS_MONO | EEPROM_AUDIO_FLAGS_SURROUND)) == 0;
}

bool xboxConfig::getAudioModeMono() 
{
	return (mAudioFlags & EEPROM_AUDIO_FLAGS_MONO) > 0;
}

bool xboxConfig::getAudioModeSurround() 
{
	return (mAudioFlags & EEPROM_AUDIO_FLAGS_SURROUND) > 0;
}

char* xboxConfig::getAudioModeString() 
{
	if (getAudioModeMono() == true)
	{
		return strdup("Mono");
	}
	else if (getAudioModeSurround() == true)
	{
		return strdup("Surround");
	}
	return strdup("Stereo");
}

void xboxConfig::setAudioAC3(bool enable) 
{
    mAudioFlags = (mAudioFlags & ~EEPROM_AUDIO_FLAGS_ENABLE_AC3) + (enable ? EEPROM_AUDIO_FLAGS_ENABLE_AC3 : 0);
	save();
}

bool xboxConfig::getAudioAC3() 
{
	return (mAudioFlags & XC_AUDIO_FLAGS_ENABLE_AC3) > 0;
}

void xboxConfig::setAudioDTS(bool enable) 
{
    mAudioFlags = (mAudioFlags & ~EEPROM_AUDIO_FLAGS_ENABLE_DTS) + (enable ? EEPROM_AUDIO_FLAGS_ENABLE_DTS : 0);
	save();
}

bool xboxConfig::getAudioDTS() 
{
	return (mAudioFlags & EEPROM_AUDIO_FLAGS_ENABLE_DTS) > 0;
}

void xboxConfig::setGameRegionNorthAmerica()
{
	mGameRegion = 0x1;
	save();
}

bool xboxConfig::getGameRegionNorthAmerica()
{
	return mGameRegion == 0x1;
}

void xboxConfig::setGameRegionJapan()
{
	mGameRegion = 0x2;
	save();
}

bool xboxConfig::getGameRegionJapan()
{
	return mGameRegion == 0x2;
}

void xboxConfig::setGameRegionRestOfWorld()
{
	mGameRegion = 0x4;
	save();
}

bool xboxConfig::getGameRegionRestOfWorld()
{
	return mGameRegion == 0x4;
}

char* xboxConfig::getGameRegionString()
{
	switch (mGameRegion)
	{
	case 0x1:
		return strdup("North America");
	case 0x2:
        return strdup("Japan");
    case 0x4:
		return strdup("Rest of World");
    case 0x80000000:
		return strdup("Manufacturing");
	}
	return strdup("Unknown");
}

void xboxConfig::setDvdRegion(uint32_t region)
{
	mDvdRegion = region;
	save();
}

uint32_t xboxConfig::getDvdRegion()
{
	return mDvdRegion;
}

char* xboxConfig::getDvdRegionString()
{
	switch (mDvdRegion)
	{
	case 0x0:
		return strdup("Region 0 (None)");
	case 0x1:
		return strdup("Region 1 (North America)");
	case 0x2:
        return strdup("Region 2 (Japan and Europe)");
    case 0x3:
		return strdup("Region 3 (SE Asia)");
	case 0x4:
		return strdup("Region 4 (Australia / SA)");
	case 0x5:
        return strdup("Region 5 (NW Asia / N Africa)");
    case 0x6:
		return strdup("Region 6 (China)");
	}
	return strdup("Unknown");
}

void xboxConfig::setVideoStandardNTSCM()
{
	mVideoStandard = 0x00400100;
	setVideo60hz(false);
}

bool xboxConfig::getVideoStandardNTSCM()
{
	return mVideoStandard == 0x00400100 && getVideo50hz();
}

void xboxConfig::setVideoStandardNTSCJ()
{
	mVideoStandard = 0x00400200;
	setVideo60hz(false);
}

bool xboxConfig::getVideoStandardNTSCJ()
{
	return mVideoStandard == 0x00400200 && getVideo50hz();;
}

void xboxConfig::setVideoStandardPALI50()
{
	mVideoStandard = 0x00800300;
	setVideo60hz(false);
}

bool xboxConfig::getVideoStandardPALI50()
{
	return mVideoStandard == 0x00800300 && getVideo50hz();
}

void xboxConfig::setVideoStandardPALI60()
{
	mVideoStandard = 0x00800300;
	setVideo60hz(true);
}

bool xboxConfig::getVideoStandardPALI60()
{
	return mVideoStandard == 0x00800300 && getVideo60hz();
}

char* xboxConfig::getVideoStandardString()
{
	if (getVideoStandardNTSCM() == true)
	{
		return strdup("NTSC-M");
	}
	else if (getVideoStandardNTSCJ() == true)
	{
		return strdup("NTSC-J");
	}
	else if (getVideoStandardPALI50() == true)
	{
		return strdup("PAL-I (50hz)");
	}
	else if (getVideoStandardPALI50() == true)
	{
		return strdup("PAL-I (60hz)");
	}
	return strdup("Invalid");
}

char* xboxConfig::getMacString()
{
	return strdup(mMac);
}

char* xboxConfig::getSerialString()
{
	return strdup(mSerial);
}

xboxConfig::xboxVersionEnum xboxConfig::getXboxVersion()
{
	DWORD temp;
	char ver[6];

	HalReadSMBusByte(0x20, 0x01, &temp);
	ver[0] = (char)temp;
	HalReadSMBusByte(0x20, 0x01, &temp);
	ver[1] = (char)temp;
	HalReadSMBusByte(0x20, 0x01, &temp);
	ver[2] = (char)temp;
	ver[3] = 0; ver[4] = 0; ver[5] = 0;

	if (strcmp(ver, "01D") == 0 || strcmp(ver, "D01") == 0 || strcmp(ver, "1D0") == 0 || strcmp(ver, "0D1") == 0) { 
		return xboxVersionDevKit;
	} else if (strcmp(ver, "DBG") == NULL || strcmp(ver, "B11") == NULL) { 
		return xboxVersionDebugKit;
	} else if (strcmp(ver, "P01") == NULL) { 
		return xboxVersion10;
	} else if (strcmp(ver, "P05") == NULL) { 
		return xboxVersion11;
	} else if (strcmp(ver, "P11") == NULL ||  strcmp(ver, "1P1") == NULL || strcmp(ver, "11P") == NULL ) {
		if (HalReadSMBusWord(0xD4, 0x00, &temp) == 0) {  
			return xboxVersion14or15;
		} else {  
			return xboxVersion12or13;
		}
	} else if (strcmp(ver,("P2L")) == NULL) {
		ULONG StrapEMRS = ((*((volatile ULONG*)0xFD101000)) & 0x0000C0000) >> 18;
		if(StrapEMRS == 3) { // Hynix
			return xboxVersion16b;
		} else { // 0x01 = Samsung
			return xboxVersion16;
		}
	}
	return xboxVersionUnknown;
}

char* xboxConfig::getXboxVersionString()
{
	xboxVersionEnum version = getXboxVersion();
	
	switch (version)
	{
	case xboxVersionDevKit:
		return strdup("DevKit");
	case xboxVersionDebugKit:
        return strdup("DebugKit");
	case xboxVersion10:
        return strdup("1.0");
	case xboxVersion11:
        return strdup("1.1");
	case xboxVersion12or13:
        return strdup("1.2/1.3");
	case xboxVersion14or15:
        return strdup("1.4/1.5");
	case xboxVersion16:
        return strdup("1.6");
	case xboxVersion16b:
        return strdup("1.6b");
	}
	return strdup("Unknown");
}

char* xboxConfig::getAvPackString()
{
	uint32_t value;
	HalReadSMBusByte(0x21, 0x04, &value);
	
	switch (value)
	{
	case 0:
		return strdup("SDTV SCART Analog/Digital");
	case 1:
        return strdup("HDTV");
	case 2:
        return strdup("VGA");
	case 3:
        return strdup("RFU");
	case 4:
        return strdup("SDTV Analog/Digital");
	case 5:
        return strdup("Power Off");
	case 6:
        return strdup("SDTV Analog");
	case 7:
        return strdup("No Pack");
	}
	return strdup("Unknown");
}

char* xboxConfig::getEncoderString()
{
	DWORD temp = 0;
	if (HalReadSMBusByte(0x8A, 0x00, &temp) == 0)
    {
		return strdup("Conexant");
    }
    else if (HalReadSMBusByte(0xD4, 0x00, &temp) == 0)
    {
		return strdup("Focus");
    }
	return strdup("Xcalibur");
}

char* xboxConfig::getHdModString()
{
    NTSTATUS status;
    status = HalWriteSMBusByte(I2C_HDMI_ADDRESS1, 0, 0);
	uint32_t temp;
	status |= HalReadSMBusByte(I2C_HDMI_ADDRESS1, 0, &temp);
    if (status == STATUS_SUCCESS)
    {
	    uint32_t version1;
		uint32_t version2;
		uint32_t version3;
		HalReadSMBusByte(I2C_HDMI_ADDRESS1, I2C_HDMI_VERSION1, &version1);
		HalReadSMBusByte(I2C_HDMI_ADDRESS1, I2C_HDMI_VERSION2, &version2);
		HalReadSMBusByte(I2C_HDMI_ADDRESS1, I2C_HDMI_VERSION3, &version3);
		return stringUtility::formatString("V%i.%i.%i", version1, version2, version3);
    } 
    return strdup("Not Detected");
}

bool xboxConfig::getHasRtcExpansion()
{
	static int32_t hasRtc = -1;
	if (hasRtc == -1)
	{
		uint32_t temp;
		hasRtc = HalReadSMBusByte(0x68 << 1, 0, &temp) == 0 ? 1 : 0;
	}
	return hasRtc == 1;
}

double xboxConfig::RDTSC()
{
 unsigned long a, b;
  double x;
  __asm
  {
    RDTSC
    mov [a],eax
    mov [b],edx
  }
  x=b;
  x*=0x100000000;
  x+=a;
  return x;
}

double xboxConfig::getCPUFreq()
{
	double tcpu_fsb = RDTSC();
	uint32_t twin_fsb = GetTickCount();

	Sleep(300);

	double tcpu_result = RDTSC();
	uint32_t twin_result = GetTickCount();

	double fcpu = (tcpu_result - tcpu_fsb);
	fcpu /= (twin_result - twin_fsb);

	return fcpu / 1000;
}

void xboxConfig::autoFix()
{
	// Check and clear any unused bits
	if ((mAudioFlags & ~EEPROM_AUDIO_FLAGS_MASK) > 0 || (mVideoFlags & ~EEPROM_VIDEO_FLAGS_MASK) > 0)
	{
		utils::debugPrint("AutoFix: Cleaning Audio/Videio flags\n");
		save();
	}


	// if invalid Video Standard set to NTSCM
	if (getVideoStandardNTSCM() == false && getVideoStandardNTSCJ() == false && getVideoStandardPALI50() == false && getVideoStandardPALI60() == false)
	{
		utils::debugPrint("AutoFix: Fixing invalid video standard\n");
		setVideoStandardNTSCM();
	}
}