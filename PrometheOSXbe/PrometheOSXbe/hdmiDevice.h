#pragma once

#include "xboxinternals.h"
#include "versioning.h"

#define I2C_HDMI_ADDRESS1 0x88
#define I2C_HDMI_ADDRESS2 0x86

#define I2C_HDMI_VERSION1 0x57
#define I2C_HDMI_VERSION2 0x58
#define I2C_HDMI_VERSION3 0x59

class hdmiDevice
{
public:
	static NTSTATUS address1WriteByte(UCHAR cmd, UCHAR data);
	static NTSTATUS address1ReadByte(UCHAR cmd, UCHAR* data);
	static bool detectDevice();
	static semver getVersion();
};
