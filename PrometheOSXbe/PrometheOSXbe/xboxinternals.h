#pragma once

#include <xtl.h>

#define WSAWOULDBLOCK 10035

typedef signed char int8_t;
typedef short int16_t;
typedef long int32_t;
typedef long long int64_t;
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned long uint32_t;
typedef unsigned long long uint64_t;

#define I2C_HDMI_ADDRESS1 0x88
#define I2C_HDMI_ADDRESS2 0x86
#define I2C_HDMI_VERSION1 0x57
#define I2C_HDMI_VERSION2 0x58
#define I2C_HDMI_VERSION3 0x59

#define XC_VIDEO_FLAGS 0x8
#define XC_AUDIO_FLAGS 0x9
#define XC_DVD_REGION 0x12
#define XC_FACTORY_SERIAL_NUMBER 0x100
#define XC_FACTORY_ETHERNET_ADDR 0x101
#define XC_FACTORY_AV_REGION 0x103
#define XC_FACTORY_GAME_REGION 0x104

#define	SMBDEV_PIC16L 0x20
#define	SMBDEV_VIDEO_ENCODER_CONNEXANT 0x8a,
#define	SMBDEV_VIDEO_ENCODER_FOCUS 0xd4,
#define	SMBDEV_VIDEO_ENCODER_XCALIBUR 0xe0,
#define	SMBDEV_TEMP_MONITOR 0x98,
#define	SMBDEV_EEPROM 0xa8

#define PIC16L_CMD_POWER 0x02
#define PIC16L_CMD_AV_PACK 0x04
#define PIC16L_CMD_LED_MODE 0x07
#define PIC16L_CMD_LED_REGISTER 0x08
#define PIC16L_CMD_EJECT 0x0c
#define PIC16L_CMD_INTERRUPT_REASON 0x11
#define PIC16L_CMD_RESET_ON_EJECT 0x19
#define	PIC16L_CMD_SCRATCH_REGISTER 0x1b

#define POWER_SUBCMD_RESET 0x01
#define POWER_SUBCMD_CYCLE 0x40
#define POWER_SUBCMD_POWER_OFF 0x80

#define	SCRATCH_REGISTER_BITVALUE_EJECT_AFTER_BOOT 0x01
#define	SCRATCH_REGISTER_BITVALUE_DISPLAY_ERROR 0x02
#define	SCRATCH_REGISTER_BITVALUE_NO_ANIMATION 0x04
#define	SCRATCH_REGISTER_BITVALUE_RUN_DASHBOARD 0x08

#define RETURN_FIRMWARE_HALT 0x00
#define	RETURN_FIRMWARE_REBOOT 0x01
#define RETURN_FIRMWARE_QUICK_REBOOT 0x02
#define RETURN_FIRMWARE_HARD 0x03
#define RETURN_FIRMWARE_FATAL 0x04
#define RETURN_FIRMWARE_ALL 0x05

#define SMC_COMMAND_TRAY_STATE 0x03
#define SMC_TRAY_STATE_ACTIVITY 0x01
#define SMC_TRAY_STATE_CLOSED 0x00
#define SMC_TRAY_STATE_OPEN 0x10
#define SMC_TRAY_STATE_UNLOADING 0x20
#define SMC_TRAY_STATE_OPENING 0x30
#define SMC_TRAY_STATE_NO_MEDIA 0x40
#define SMC_TRAY_STATE_CLOSING 0x50
#define SMC_TRAY_STATE_MEDIA_DETECT 0x60
#define SMC_TRAY_STATE_RESET 0x70

#define PIC_ADDRESS 0x20
#define FAN_MODE 0x05
#define FAN_REGISTER 0x06
#define FAN_READBACK 0x10
#define MB_TEMP 0x0A
#define CPU_TEMP 0x09

#define I2C_IO_BASE 0xC000

#define SMC_SLAVE_ADDRESS 0x20
#define SMC_COMMAND_LED_OVERRIDE 0x07
#define SMC_COMMAND_LED_STATES 0x08
#define SMC_LED_OVERRIDE_USE_REQUESTED_LED_STATES 0x01
#define SMC_LED_STATES_GREEN_STATE0 0x01
#define SMC_LED_STATES_GREEN_STATE1 0x02
#define SMC_LED_STATES_GREEN_STATE2 0x04
#define SMC_LED_STATES_GREEN_STATE3 0x08
#define SMC_LED_STATES_RED_STATE0 0x10
#define SMC_LED_STATES_RED_STATE1 0x20
#define SMC_LED_STATES_RED_STATE2 0x40
#define SMC_LED_STATES_RED_STATE3 0x80

#define EEPROM_VIDEO_FLAGS_WIDESCREEN   0x00010000
#define EEPROM_VIDEO_FLAGS_HDTV_720p    0x00020000
#define EEPROM_VIDEO_FLAGS_HDTV_1080i   0x00040000
#define EEPROM_VIDEO_FLAGS_HDTV_480p    0x00080000
#define EEPROM_VIDEO_FLAGS_LETTERBOX    0x00100000
#define EEPROM_VIDEO_FLAGS_60Hz         0x00400000
#define EEPROM_VIDEO_FLAGS_MASK         0x005F0000

#define EEPROM_AUDIO_FLAGS_STEREO       0x00000000
#define EEPROM_AUDIO_FLAGS_MONO         0x00000001
#define EEPROM_AUDIO_FLAGS_SURROUND     0x00000002
#define EEPROM_AUDIO_FLAGS_ENABLE_AC3   0x00010000
#define EEPROM_AUDIO_FLAGS_ENABLE_DTS   0x00020000
#define EEPROM_AUDIO_FLAGS_MASK         0x00030003

typedef LONG NTSTATUS;

#define STATUS_SUCCESS ((NTSTATUS)0x00000000L) 
#define STATUS_DEVICE_NOT_CONNECTED ((NTSTATUS)0xC000009DL)
#define STATUS_IO_TIMEOUT ((NTSTATUS)0xC00000B5L)
#define STATUS_DEVICE_DOES_NOT_EXIST ((NTSTATUS)0xC00000C0L)
#define STATUS_NOT_FOUND ((NTSTATUS)0xC0000225L)
#define STATUS_INVALID_DEVICE_REQUEST ((NTSTATUS)0xC0000010L)
#define STATUS_UNSUCCESSFUL ((NTSTATUS)0x80000000L)

typedef struct LAUNCH_DATA_HEADER
{
	DWORD   dwLaunchDataType;
	DWORD   dwTitleId;
	char    szLaunchPath[520];
	DWORD   dwFlags;
}
LAUNCH_DATA_HEADER;

typedef struct LAUNCH_DATA_PAGE
{
	LAUNCH_DATA_HEADER  Header;
	UCHAR               Pad[492];
	UCHAR               LaunchData[3072];
}
LAUNCH_DATA_PAGE;

#define PAGE_SIZE 0x1000
#define ROUND_TO_PAGES(Size)  (((ULONG_PTR)(Size) + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1))

typedef struct DRIVER_OBJECT
{
	const int16_t Type;
	const int16_t Size;
	struct _DEVICE_OBJECT *DeviceObject;
	// ...
}
DRIVER_OBJECT;

typedef struct DEVICE_OBJECT
{
	const int16_t Type;
	const uint16_t Size;
	int32_t ReferenceCount;
	DRIVER_OBJECT* DriverObject;
}
DEVICE_OBJECT;

typedef struct STRING {
	uint16_t Length;
	uint16_t MaximumLength;
	char* Buffer;
} STRING;

typedef struct {
    BYTE        abSeed[20];                     // Last random seed
    IN_ADDR     ina;                            // Static IP address (0 for DHCP)
    IN_ADDR     inaMask;                        // Static IP subnet mask
    IN_ADDR     inaGateway;                     // Static gateway IP address
    IN_ADDR     inaDnsPrimary;                  // Static primary DNS server IP address
    IN_ADDR     inaDnsSecondary;                // Static secondary DNS server IP address
    char        achDhcpHostName[64];            // Host name for DHCP ("" means no host name)
    char        achPppUserName[64];             // User name for PPPoE ("" means no PPPoE)
    char        achPppPassword[64];             // User password for PPPoE
    char        achPppServer[64];               // Server name for PPPoE ("" ok)
    BYTE        abReserved[192];                // Reserved (makes structure 492 bytes)
    DWORD       dwSigEnd;                       // XNET_CONFIG_PARAMS_SIGEND
} XXNetConfigParams;

typedef struct   
{   
	DWORD	Data_00;            // Check Block Start   
	DWORD	Data_04;   
	DWORD	Data_08;   
	DWORD	Data_0c;   
	DWORD	Data_10;            // Check Block End   
  
	DWORD	V1_IP;              // 0x14   
	DWORD	V1_Subnetmask;      // 0x18   
	DWORD	V1_Defaultgateway;  // 0x1c   
	DWORD	V1_DNS1;            // 0x20   
	DWORD	V1_DNS2;            // 0x24   

	DWORD	Data_28;            // Check Block Start   
	DWORD	Data_2c;   
	DWORD	Data_30;   
	DWORD	Data_34;   
	DWORD	Data_38;            // Check Block End   

	DWORD	V2_Tag;             // V2 Tag "XBV2"   
 
	DWORD	Flag;				// 0x40   
	DWORD	Data_44;   

	DWORD	V2_IP;              // 0x48   
	DWORD	V2_Subnetmask;      // 0x4c   
	DWORD	V2_Defaultgateway;  // 0x50   
	DWORD	V2_DNS1;            // 0x54   
	DWORD	V2_DNS2;            // 0x58   

	DWORD   Data_xx[0x200-0x5c];

} XNetConfigParams;

#define XDK_NETWORK_CONFIG_MANUAL_IP 0x00000004
#define XDK_NETWORK_CONFIG_MANUAL_DNS 0x00000008

typedef struct {
    DWORD       dwFlags;                        // See XNET_STATUS_* below
    IN_ADDR     ina;                            // IP address
    IN_ADDR     inaMask;                        // IP subnet mask
    IN_ADDR     inaGateway;                     // Gateway IP address
    IN_ADDR     inaDnsPrimary;                  // Primary DNS server IP address
    IN_ADDR     inaDnsSecondary;                // Secondary DNS server IP address
    IN_ADDR     inaDhcpServer;                  // IP address of DHCP server
    char        achPppServer[64][4];            // PPPoE concentrators discovered
} XNetConfigStatus;

//typedef struct XNetConfigStatus
//{
//  DWORD data_00;
//
//  IN_ADDR ip;       // 0x04
//  IN_ADDR subnet;   // 0x08
//  IN_ADDR gateway;  // 0x0c
//
//  DWORD data_10;    /* probably additional gateway */
//  DWORD data_14;
//  DWORD data_18;
//
//  IN_ADDR dns1;     // 0x1c
//  IN_ADDR dns2;     // 0x20
//
//  DWORD data_24;
//  DWORD data_28;
//  DWORD data_2c;
//  DWORD data_30;
//
//  BYTE dhcp;        // 0x34
//  BYTE data_35;
//  DWORD data_36;
//}
//TXNetConfigStatus, *PTXNetConfigStatus;

//https://github.com/brentdc-nz/xbmc4xbox/blob/13cf4fbab8d70b154941a6b91e101bd05cc5b111/xbmc/xbox/Network.cpp#L211

#define XNET_CONFIG_NORMAL 0
#define XNET_CONFIG_PARAMS_SIGEND 'XBCP'

#define XNET_STATUS_PENDING             0x0001  // XNet configuration is not yet complete
#define XNET_STATUS_PPPOE_DISCOVERED    0x0002  // One or more PPPoE concentrators found
#define XNET_STATUS_PPPOE_CONFIGURED    0x0004  // Credentials accepted by PPPoE concentrator
#define XNET_STATUS_PPPOE_REJECTED      0x0008  // Credentials rejected by PPPoE concentrator
#define XNET_STATUS_PPPOE_NORESPONSE    0x0010  // PPPoE concentrator did not respond
#define XNET_STATUS_DHCP_CONFIGURED     0x0020  // DHCP server provided IP configuration
#define XNET_STATUS_DHCP_REJECTED       0x0040  // DHCP server rejected request
#define XNET_STATUS_DHCP_GATEWAY        0x0080  // DHCP server provided default gateway
#define XNET_STATUS_DHCP_DNS            0x0100  // DHCP server provided DNS servers
#define XNET_STATUS_DHCP_NORESPONSE     0x0200  // DHCP server did not respond
#define XNET_STATUS_DNS_CONFIGURED      0x0400  // DNS server successfully resolved test lookup
#define XNET_STATUS_DNS_FAILED          0x0800  // DNS server failed to resolve test lookup
#define XNET_STATUS_DNS_NORESPONSE      0x1000  // DNS server did not respond to test lookup
#define XNET_STATUS_PING_SUCCESSFUL     0x2000  // ICMP echo to Internet test server successful
#define XNET_STATUS_PING_NORESPONSE     0x4000  // ICMP echo to Internet test server did not respond

#define SMC_COMMAND_OVERRIDE_RESET_ON_TRAY_OPEN 0x19
#define SMC_RESET_ON_TRAY_OPEN_NONSECURE_MODE 0x01

typedef struct OBJECT_ATTRIBUTES 
{
    HANDLE RootDirectory;
    STRING*	ObjectName;
    ULONG Attributes;
} OBJECT_ATTRIBUTES;

typedef struct IO_STATUS_BLOCK {
    union {
        NTSTATUS Status;
        PVOID Pointer;
    };
    ULONG_PTR Information;
} IO_STATUS_BLOCK, *PIO_STATUS_BLOCK;

#define FAT_VOLUME_NAME_LENGTH          32
#define FAT_ONLINE_DATA_LENGTH          2048
typedef struct _FAT_VOLUME_METADATA {
    ULONG Signature;
    ULONG SerialNumber;
    ULONG SectorsPerCluster;
    ULONG RootDirFirstCluster;
    WCHAR VolumeName[FAT_VOLUME_NAME_LENGTH];
    UCHAR OnlineData[FAT_ONLINE_DATA_LENGTH];
    // Unused space in the block is filled with 0xFF bytes.
} FAT_VOLUME_METADATA, *PFAT_VOLUME_METADATA;

typedef enum _FSINFOCLASS {
    FileFsVolumeInformation = 1,
    FileFsLabelInformation,
    FileFsSizeInformation,
    FileFsDeviceInformation,
    FileFsAttributeInformation,
    FileFsControlInformation,
    FileFsFullSizeInformation,
    FileFsObjectIdInformation,
    FileFsMaximumInformation
} FS_INFORMATION_CLASS, *PFS_INFORMATION_CLASS;

typedef struct _FILE_FS_SIZE_INFORMATION {
    LARGE_INTEGER TotalAllocationUnits;
    LARGE_INTEGER AvailableAllocationUnits;
    ULONG SectorsPerAllocationUnit;
    ULONG BytesPerSector;
} FILE_FS_SIZE_INFORMATION, *PFILE_FS_SIZE_INFORMATION;

typedef VOID (WINAPI *PIO_APC_ROUTINE) (PVOID ApcContext, IO_STATUS_BLOCK* IoStatusBlock, ULONG Reserved);

#define InitializeObjectAttributes( p, n, a, r ) { \
    (p)->RootDirectory = r;                             \
    (p)->Attributes = a;                                \
    (p)->ObjectName = n;                                \
    }

extern "C" 
{
	extern STRING* XeImageFileName;
	extern STRING* HalDiskModelNumber;
	extern STRING* HalDiskSerialNumber;

	LONG WINAPI IoCreateSymbolicLink(STRING*, STRING*);
	LONG WINAPI IoDeleteSymbolicLink(STRING*);
	LONG WINAPI IoDismountVolumeByName(STRING*);
	LONG WINAPI IoDismountVolume(DEVICE_OBJECT*);
	VOID WINAPI HalReturnToFirmware(unsigned int value);
	LONG WINAPI ExQueryNonVolatileSetting(ULONG ValueIndex, ULONG* Type, void* Value, ULONG ValueLength, ULONG* ResultLength);
	LONG WINAPI ExSaveNonVolatileSetting(ULONG ValueIndex, ULONG Type, void* Value, ULONG ValueLength);
	NTSTATUS WINAPI HalWriteSMBusValue(UCHAR devddress, UCHAR offset, UCHAR writedw, DWORD data);
	NTSTATUS WINAPI HalReadSMBusValue(UCHAR devddress, UCHAR offset, UCHAR readdw, DWORD* pdata);
	NTSTATUS WINAPI HalReadSMCTrayState(ULONG* TrayState, ULONG* EjectCount);

	NTSTATUS WINAPI XNetLoadConfigParams(XNetConfigParams* params);
	NTSTATUS WINAPI XNetSaveConfigParams(const XNetConfigParams* params);
	NTSTATUS WINAPI XNetConfig(const XNetConfigParams* params, DWORD data);
	NTSTATUS WINAPI XNetGetConfigStatus(XNetConfigStatus* status);

	VOID WINAPI KeQuerySystemTime(LPFILETIME CurrentTime);

	NTSTATUS WINAPI NtQueryVolumeInformationFile(HANDLE FileHandle, PIO_STATUS_BLOCK IoStatusBlock, PVOID FsInformation, ULONG Length, FS_INFORMATION_CLASS FsInformationClass);
	NTSTATUS WINAPI NtSetSystemTime(LPFILETIME SystemTime, LPFILETIME PreviousTime);
	NTSTATUS WINAPI NtOpenFile(HANDLE* FileHandle, ACCESS_MASK DesiredAccess, OBJECT_ATTRIBUTES* ObjectAttributes, IO_STATUS_BLOCK* IoStatusBlock, ULONG ShareAccess, ULONG OpenOptions);
	NTSTATUS WINAPI NtReadFile(HANDLE FileHandle, HANDLE Event, PIO_APC_ROUTINE ApcRoutine, VOID* ApcContext, IO_STATUS_BLOCK* IoStatusBlock, VOID* Buffer, ULONG Length, LARGE_INTEGER* ByteOffset);
	NTSTATUS WINAPI NtWriteFile(HANDLE FileHandle, HANDLE Event, PIO_APC_ROUTINE ApcRoutine, VOID* ApcContext, IO_STATUS_BLOCK* IoStatusBlock, VOID* Buffer, ULONG Length, LARGE_INTEGER* ByteOffset);
	NTSTATUS WINAPI NtClose(HANDLE Handle);
	NTSTATUS WINAPI NtDeviceIoControlFile(HANDLE FileHandle, HANDLE Event, PIO_APC_ROUTINE ApcRoutine, PVOID ApcContext, IO_STATUS_BLOCK* IoStatusBlock, ULONG IoControlCode, PVOID InputBuffer, ULONG InputBufferLength, PVOID OutputBuffer, ULONG OutputBufferLength);
	NTSTATUS WINAPI RtlInitAnsiString(STRING* DestinationString, LPCSTR SourceString);

	NTSTATUS WINAPI KeDelayExecutionThread(CHAR WaitMode, BOOLEAN Alertable, PLARGE_INTEGER Interval);
	VOID WINAPI KeStallExecutionProcessor(ULONG usec);
	ULONG WINAPI HalGetInterruptVector(ULONG BusInterruptLevel, UCHAR* Irql);
	UCHAR _fastcall KfRaiseIrql(UCHAR NewIrql);
	VOID _fastcall KfLowerIrql(UCHAR NewIrql);

	NTSTATUS WINAPI MU_CreateDeviceObject(uint32_t port, uint32_t slot, STRING* deviceName);
	VOID WINAPI MU_CloseDeviceObject(uint32_t port, uint32_t slot);
	DEVICE_OBJECT* WINAPI MU_GetExistingDeviceObject(uint32_t port, uint32_t slot);
	BOOL WINAPI XapiFormatFATVolumeEx(STRING* VolumePath, ULONG BytesPerCluster);

	extern LAUNCH_DATA_PAGE* LaunchDataPage;

	VOID WINAPI MmPersistContiguousMemory(PVOID BaseAddress, ULONG NumberOfBytes, BOOLEAN Persist);
	PVOID WINAPI MmAllocateContiguousMemory(ULONG NumberOfBytes);

	UCHAR NTSYSAPI XboxHDKey[0x10];
	VOID WINAPI XcHMAC(PBYTE pbKey, DWORD dwKeyLength, PBYTE pbInput, DWORD dwInputLength, PBYTE pbInput2, DWORD dwInputLength2, PBYTE pbDigest);
}

#define HalReadSMBusByte(SlaveAddress, CommandCode, DataValue) HalReadSMBusValue(SlaveAddress, CommandCode, FALSE, DataValue)
#define HalReadSMBusWord(SlaveAddress, CommandCode, DataValue) HalReadSMBusValue(SlaveAddress, CommandCode, TRUE, DataValue)
#define HalWriteSMBusByte(SlaveAddress, CommandCode, DataValue) HalWriteSMBusValue(SlaveAddress, CommandCode, FALSE, DataValue)
#define HalWriteSMBusWord(SlaveAddress, CommandCode, DataValue) HalWriteSMBusValue(SlaveAddress, CommandCode, TRUE, DataValue)
