#pragma once

#include "xboxinternals.h"

class xboxConfig 
{
public:

	enum xboxVersionEnum
	{
		xboxVersionUnknown,
		xboxVersion10,
		xboxVersion11,
		xboxVersion12or13,
		xboxVersion14or15,
		xboxVersion16,
		xboxVersion16b,
		xboxVersionDebugKit,
		xboxVersionDevKit
	};

	static void init();
    static void save();
    static void setVideo480p(bool enable);
	static bool getVideo480p();
    static void setVideo720p(bool enable);
	static bool getVideo720p();
    static void setVideo1080i(bool enable);
	static bool getVideo1080i();
    static void setVideoAspectRatio(uint32_t flag);
	static bool getVideoAspectRatioNormal();
	static bool getVideoAspectRatioWidescreen();
	static bool getVideoAspectRatioLetterbox();
	static void setVideo60hz(bool enable);
	static bool getVideo50hz();
	static bool getVideo60hz();
    static void setAudioMode(uint32_t flag);
    static bool getAudioModeStereo();
	static bool getAudioModeMono();
	static bool getAudioModeSurround();
	static char* getAudioModeString();
    static void setAudioAC3(bool enable);
	static bool getAudioAC3();
    static void setAudioDTS(bool enable);
	static bool getAudioDTS();
	static void setGameRegionNorthAmerica();
	static bool getGameRegionNorthAmerica();
	static void setGameRegionJapan();
	static bool getGameRegionJapan();
	static void setGameRegionRestOfWorld();
	static bool getGameRegionRestOfWorld();
	static char* getGameRegionString();
	static void setDvdRegion(uint32_t region);
	static uint32_t getDvdRegion();
	static char* getDvdRegionString();
	static void setVideoStandardNTSCM();
	static bool getVideoStandardNTSCM();
	static void setVideoStandardNTSCJ();
	static bool getVideoStandardNTSCJ();
	static void setVideoStandardPALI50();
	static bool getVideoStandardPALI50();
	static void setVideoStandardPALI60();
	static bool getVideoStandardPALI60();
	static char* getVideoStandardString();
	static char* getMacString();
	static char* getSerialString();
	static xboxVersionEnum getXboxVersion();
	static char* getXboxVersionString();
	static char* getAvPackString();
	static char* getEncoderString();
	static char* getHdModString();
	static bool getHasRtcExpansion();
	static double getCPUFreq();
	static void autoFix();
private:
	static double RDTSC();
};