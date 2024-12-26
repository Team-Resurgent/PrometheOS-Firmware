#pragma once

#include <xtl.h>

#include "xboxInternals.h"

typedef struct XboxPartitionTableEntry
{
	UCHAR Name[16];
	ULONG Flags;
	ULONG LBAStartLo;
	ULONG LBASizeLo;
	USHORT LBAStartHi;
	USHORT LBASizeHi;
} XboxPartitionTableEntry;

typedef struct XboxPartitionTable
{
	UCHAR	Magic[16];
	UCHAR   FatxMode;
	char	Res0[31];
	XboxPartitionTableEntry	TableEntries[14];
	char	Res1[16];
} XboxPartitionTable;

typedef enum MEDIA_TYPE {
    Unknown,                // Format is unknown
    F5_1Pt2_512,            // 5.25", 1.2MB,  512 bytes/sector
    F3_1Pt44_512,           // 3.5",  1.44MB, 512 bytes/sector
    F3_2Pt88_512,           // 3.5",  2.88MB, 512 bytes/sector
    F3_20Pt8_512,           // 3.5",  20.8MB, 512 bytes/sector
    F3_720_512,             // 3.5",  720KB,  512 bytes/sector
    F5_360_512,             // 5.25", 360KB,  512 bytes/sector
    F5_320_512,             // 5.25", 320KB,  512 bytes/sector
    F5_320_1024,            // 5.25", 320KB,  1024 bytes/sector
    F5_180_512,             // 5.25", 180KB,  512 bytes/sector
    F5_160_512,             // 5.25", 160KB,  512 bytes/sector
    RemovableMedia,         // Removable media other than floppy
    FixedMedia,             // Fixed hard disk media
    F3_120M_512,            // 3.5", 120M Floppy
    F3_640_512,             // 3.5" ,  640KB,  512 bytes/sector
    F5_640_512,             // 5.25",  640KB,  512 bytes/sector
    F5_720_512,             // 5.25",  720KB,  512 bytes/sector
    F3_1Pt2_512,            // 3.5" ,  1.2Mb,  512 bytes/sector
    F3_1Pt23_1024,          // 3.5" ,  1.23Mb, 1024 bytes/sector
    F5_1Pt23_1024,          // 5.25",  1.23MB, 1024 bytes/sector
    F3_128Mb_512,           // 3.5" MO 128Mb   512 bytes/sector
    F3_230Mb_512,           // 3.5" MO 230Mb   512 bytes/sector
    F8_256_128              // 8",     256KB,  128 bytes/sector
} MEDIA_TYPE;

typedef struct DISK_GEOMETRY {
    LARGE_INTEGER Cylinders;
    MEDIA_TYPE MediaType;
    DWORD TracksPerCylinder;
    DWORD SectorsPerTrack;
    DWORD BytesPerSector;
} DISK_GEOMETRY;

#define OBJ_CASE_INSENSITIVE 0x00000040

#define IOCTL_CMD_LBA48_ACCESS 0xcafebabe
#define IOCTL_SUBCMD_GET_INFO 0

#define LBA48_GET_INFO_MAGIC1_IDX 0
#define LBA48_GET_INFO_MAGIC1_VAL 0xcafebabe
#define LBA48_GET_INFO_MAGIC2_IDX 1
#define LBA48_GET_INFO_MAGIC2_VAL 0xbabeface
#define LBA48_GET_INFO_PATCHCODE_VERSION_IDX 2
#define LBA48_GET_INFO_LOWCODE_BASE_IDX  3
#define LBA48_GET_INFO_HIGHCODE_BASE_IDX 4
#define LBA48_GET_INFO_PATCHSEG_SIZE_IDX 5
#define LBA48_GET_INFO_PART_TABLE_OFS_IDX 6

#define CTL_CODE( DeviceType, Function, Method, Access ) (                 \
    ((DeviceType) << 16) | ((Access) << 14) | ((Function) << 2) | (Method) \
)

#define FILE_DEVICE_DISK                0x00000007
#define FILE_SYNCHRONOUS_IO_ALERT               0x00000010
#define FILE_NO_INTERMEDIATE_BUFFERING          0x00000008

#define FILE_ANY_ACCESS                 0
//#define FILE_READ_ACCESS          ( 0x0001 )    // file & pipe
//#define FILE_WRITE_ACCESS         ( 0x0002 )    // file & pipe

#define METHOD_BUFFERED                 0

#define IOCTL_DISK_BASE                 FILE_DEVICE_DISK
#define IOCTL_DISK_GET_DRIVE_GEOMETRY   CTL_CODE(IOCTL_DISK_BASE, 0x0000, METHOD_BUFFERED, FILE_ANY_ACCESS)
//#define IOCTL_DISK_GET_PARTITION_INFO   CTL_CODE(IOCTL_DISK_BASE, 0x0001, METHOD_BUFFERED, FILE_READ_ACCESS)
//#define IOCTL_DISK_SET_PARTITION_INFO   CTL_CODE(IOCTL_DISK_BASE, 0x0002, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
//#define IOCTL_DISK_GET_DRIVE_LAYOUT     CTL_CODE(IOCTL_DISK_BASE, 0x0003, METHOD_BUFFERED, FILE_READ_ACCESS)
//#define IOCTL_DISK_SET_DRIVE_LAYOUT     CTL_CODE(IOCTL_DISK_BASE, 0x0004, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
//#define IOCTL_DISK_VERIFY               CTL_CODE(IOCTL_DISK_BASE, 0x0005, METHOD_BUFFERED, FILE_ANY_ACCESS)
//#define IOCTL_DISK_FORMAT_TRACKS        CTL_CODE(IOCTL_DISK_BASE, 0x0006, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
//#define IOCTL_DISK_REASSIGN_BLOCKS      CTL_CODE(IOCTL_DISK_BASE, 0x0007, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
//#define IOCTL_DISK_PERFORMANCE          CTL_CODE(IOCTL_DISK_BASE, 0x0008, METHOD_BUFFERED, FILE_ANY_ACCESS)
//#define IOCTL_DISK_IS_WRITABLE          CTL_CODE(IOCTL_DISK_BASE, 0x0009, METHOD_BUFFERED, FILE_ANY_ACCESS)
//#define IOCTL_DISK_LOGGING              CTL_CODE(IOCTL_DISK_BASE, 0x000a, METHOD_BUFFERED, FILE_ANY_ACCESS)
//#define IOCTL_DISK_FORMAT_TRACKS_EX     CTL_CODE(IOCTL_DISK_BASE, 0x000b, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
//#define IOCTL_DISK_HISTOGRAM_STRUCTURE  CTL_CODE(IOCTL_DISK_BASE, 0x000c, METHOD_BUFFERED, FILE_ANY_ACCESS)
//#define IOCTL_DISK_HISTOGRAM_DATA       CTL_CODE(IOCTL_DISK_BASE, 0x000d, METHOD_BUFFERED, FILE_ANY_ACCESS)
//#define IOCTL_DISK_HISTOGRAM_RESET      CTL_CODE(IOCTL_DISK_BASE, 0x000e, METHOD_BUFFERED, FILE_ANY_ACCESS)
//#define IOCTL_DISK_REQUEST_STRUCTURE    CTL_CODE(IOCTL_DISK_BASE, 0x000f, METHOD_BUFFERED, FILE_ANY_ACCESS)
//#define IOCTL_DISK_REQUEST_DATA         CTL_CODE(IOCTL_DISK_BASE, 0x0010, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define PARTITION_XBOX_CACHE_X_LBA_SIZE  0x177000
#define PARTITION_XBOX_CACHE_X_LBA_START 0x400
#define PARTITION_XBOX_CACHE_Y_LBA_SIZE  0x177000
#define PARTITION_XBOX_CACHE_Y_LBA_START (PARTITION_XBOX_CACHE_X_LBA_START + PARTITION_XBOX_CACHE_X_LBA_SIZE)
#define PARTITION_XBOX_CACHE_Z_LBA_SIZE  0x177000
#define PARTITION_XBOX_CACHE_Z_LBA_START (PARTITION_XBOX_CACHE_Y_LBA_START + PARTITION_XBOX_CACHE_Y_LBA_SIZE)
#define PARTITION_XBOX_SHELL_C_LBA_SIZE  0xfa000
#define PARTITION_XBOX_SHELL_C_LBA_START  (PARTITION_XBOX_CACHE_Z_LBA_START + PARTITION_XBOX_CACHE_Z_LBA_SIZE)
#define PARTITION_XBOX_DATA_E_LBA_SIZE  0x9896b0
#define PARTITION_XBOX_DATA_E_LBA_START (PARTITION_XBOX_SHELL_C_LBA_START + PARTITION_XBOX_SHELL_C_LBA_SIZE)
#define PARTITION_XBOX_DATA_F_LBA_START  (PARTITION_XBOX_DATA_E_LBA_START + PARTITION_XBOX_DATA_E_LBA_SIZE)

#define PARTITION_XBOX_NAME_DATA_E "XBOX DATA E"
#define PARTITION_XBOX_NAME_SHELL_C "XBOX SHELL C"
#define PARTITION_XBOX_NAME_CACHE_X "XBOX CACHE X"
#define PARTITION_XBOX_NAME_CACHE_Y "XBOX CACHE Y"
#define PARTITION_XBOX_NAME_CACHE_Z "XBOX CACHE Z"
#define PARTITION_XBOX_NAME_DRIVE_F "DRIVE F"

class harddrive
{
public:

	enum partitionTableEntryFlag
	{ 
		partitionTableEntryFlagNotInUse = 0x00000000,
		partitionTableEntryFlagInUse = 0x80000000,
	}; 

	enum fatxMode
	{
		fatxModeCerbiosPrimary = 1,
		fatxModeCerbiosSecondary = 2,
		fatxModeStellar = 3,
	};

	static bool largePartitionFixup(ULONG harddrive, XboxPartitionTableEntry* entry, ULONG clusterSize);
	static bool formatDrive(int harddrive);
	static ULONG calculateClusterSize(ULONGLONG lbaSize);
	static void setPartitionName(const char* name, XboxPartitionTableEntry* entry);
	static void setPartitionLBAStart(ULONGLONG start, XboxPartitionTableEntry* entry);
	static void setPartitionLBASize(ULONGLONG size, XboxPartitionTableEntry* entry);
	static ULONGLONG getPartitionLBASize(XboxPartitionTableEntry* entry);
	static void initXboxPartitionTable(int harddrive, ULONGLONG totalSectors, XboxPartitionTable* partitionTable);
	static NTSTATUS readLbaInfo(int harddrive, XboxPartitionTable *p_table, ULONGLONG *total_sectors, DWORD *version);
	static NTSTATUS writePartitionTable(int harddrive, XboxPartitionTable *p_table);
};
