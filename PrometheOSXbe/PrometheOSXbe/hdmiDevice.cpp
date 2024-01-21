#include "hdmiDevice.h"
#include "xboxinternals.h"
#include "versioning.h"

NTSTATUS hdmiDevice::address1WriteByte(UCHAR cmd, UCHAR data)
{
    NTSTATUS status;
    status = HalWriteSMBusByte(I2C_HDMI_ADDRESS1, cmd, data);
    return status;
}

NTSTATUS hdmiDevice::address1ReadByte(UCHAR cmd, UCHAR* data)
{
    NTSTATUS status;
    ULONG temp;
    status = HalReadSMBusByte(I2C_HDMI_ADDRESS1, cmd, &temp);
	if (data != NULL)
	{
		*data = (UCHAR)temp;
	}
	return status;
}

bool hdmiDevice::detectDevice()
{
    NTSTATUS status;
    status = address1WriteByte(0, 0);
	status |= address1ReadByte(0, NULL);
	return status == STATUS_SUCCESS;
}

semver hdmiDevice::getVersion()
{
	semver version;
	memset(&version, 0, sizeof(version));
    address1ReadByte(I2C_HDMI_VERSION1, &version.major);
    address1ReadByte(I2C_HDMI_VERSION2, &version.minor);
    address1ReadByte(I2C_HDMI_VERSION3, &version.patch);
	if (version.major == 0 && version.minor == 0 && version.patch == 0)
	{
		version.major = 1;
	}
	return version;
}