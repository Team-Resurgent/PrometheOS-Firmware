#include "harddrive.h"

#include "stringUtility.h"
#include "utils.h"
#include "driveManager.h"
#include "xboxInternals.h"

bool harddrive::formatDrive(int harddrive)
{
	XboxPartitionTable table;
	ULONGLONG totalSectors = 0;
	DWORD version = 0;

	NTSTATUS status = harddrive::readLbaInfo(harddrive, &table, &totalSectors, &version);
	if (status != STATUS_SUCCESS)
	{
		utils::debugPrint("Read LBA info failed. Status: %08X\n", status);
		return false;
	}

	driveManager::unmountAllDrives();

	harddrive::initXboxPartitionTable(harddrive, totalSectors, &table);
	status = harddrive::writePartitionTable(harddrive, &table);
	if (status != STATUS_SUCCESS)
	{
		utils::debugPrint("Partition write failed.\n");
		driveManager::mountAllDrives();
		return false;
	}

	for (int i = 0; i < 14; i++)
	{
		if (table.TableEntries[i].Flags == harddrive::partitionTableEntryFlagNotInUse)
		{
			continue;
		}

		ULONGLONG lbaSize = harddrive::getPartitionLBASize(&table.TableEntries[i]);
		ULONG clusterSize = harddrive::calculateClusterSize(lbaSize);
		ULONG clusterSizeBytes = clusterSize << 10;

		char* partition = stringUtility::formatString("\\Device\\Harddisk%i\\Partition%i", harddrive, i + 1);
		STRING partitionString;
		RtlInitAnsiString(&partitionString, partition);
		BOOL success = XapiFormatFATVolumeEx(&partitionString, clusterSizeBytes);

		// XapiFormatFATVolumeEx will format the partition with
		// an incorrect SectorsPerCluster for large partitions, let's fix it
		if(success && i >= 5) {
			success = largePartitionFixup(harddrive, &table.TableEntries[i], clusterSizeBytes);
		}

		if (success == FALSE)
		{
			utils::debugPrint("Partition '%s' format failed.\n", partition);
			free(partition);
			driveManager::mountAllDrives();
			return false;
		}
		free(partition);
	}

	driveManager::mountAllDrives();
	return true;
}

ULONG harddrive::calculateClusterSize(ULONGLONG lbaSize)
{
	ULONG clusterSize = 16;
	ULONGLONG compare = 0x20000000;
	while (lbaSize > compare)
	{
		compare = compare * 2;
		clusterSize = clusterSize * 2;
	}
	return clusterSize;
}

void harddrive::setPartitionName(const char* name, XboxPartitionTableEntry* entry)
{
	memset(entry->Name, 0x20, 16);
	strncpy((char*)entry->Name, name, strlen(name));
	entry->Flags = partitionTableEntryFlagInUse;
}

void harddrive::setPartitionLBAStart(ULONGLONG start, XboxPartitionTableEntry* entry)
{
	entry->LBAStartLo = (ULONG)(start & 0x00000000ffffffff);
	entry->LBAStartHi = (USHORT)((start & 0x0000ffff00000000) >> 32);
}

void harddrive::setPartitionLBASize(ULONGLONG size, XboxPartitionTableEntry* entry)
{
	entry->LBASizeLo = (ULONG)(size & 0x00000000ffffffff);
	entry->LBASizeHi = (USHORT)((size & 0x0000ffff00000000) >> 32);
}

ULONGLONG harddrive::getPartitionLBASize(XboxPartitionTableEntry* entry)
{
	return (((ULONGLONG)entry->LBASizeHi) << 32) | ((ULONGLONG)entry->LBASizeLo);
}

void harddrive::initXboxPartitionTable(int harddrive, ULONGLONG totalSectors, XboxPartitionTable* partitionTable)
{
	LONGLONG fPartLbaSize = totalSectors - PARTITION_XBOX_DATA_F_LBA_START;
	memset(partitionTable, 0, sizeof(XboxPartitionTable));
	strncpy((char*)partitionTable->Magic, "****PARTINFO****", 16);
	partitionTable->FatxMode = harddrive == 0 ? 1 : 2;

	setPartitionName(PARTITION_XBOX_NAME_DATA_E, &partitionTable->TableEntries[0]);
	setPartitionLBAStart(PARTITION_XBOX_DATA_E_LBA_START, &partitionTable->TableEntries[0]);
	setPartitionLBASize(PARTITION_XBOX_DATA_E_LBA_SIZE, &partitionTable->TableEntries[0]);

	setPartitionName(PARTITION_XBOX_NAME_SHELL_C, &partitionTable->TableEntries[1]);
	setPartitionLBAStart(PARTITION_XBOX_SHELL_C_LBA_START, &partitionTable->TableEntries[1]);
	setPartitionLBASize(PARTITION_XBOX_SHELL_C_LBA_SIZE, &partitionTable->TableEntries[1]);

	setPartitionName(PARTITION_XBOX_NAME_CACHE_X, &partitionTable->TableEntries[2]);
	setPartitionLBAStart(PARTITION_XBOX_CACHE_X_LBA_START, &partitionTable->TableEntries[2]);
	setPartitionLBASize(PARTITION_XBOX_CACHE_X_LBA_SIZE, &partitionTable->TableEntries[2]);

	setPartitionName(PARTITION_XBOX_NAME_CACHE_Y, &partitionTable->TableEntries[3]);
	setPartitionLBAStart(PARTITION_XBOX_CACHE_Y_LBA_START, &partitionTable->TableEntries[3]);
	setPartitionLBASize(PARTITION_XBOX_CACHE_Y_LBA_SIZE, &partitionTable->TableEntries[3]);

	setPartitionName(PARTITION_XBOX_NAME_CACHE_Z, &partitionTable->TableEntries[4]);
	setPartitionLBAStart(PARTITION_XBOX_CACHE_Z_LBA_START, &partitionTable->TableEntries[4]);
	setPartitionLBASize(PARTITION_XBOX_CACHE_Z_LBA_SIZE, &partitionTable->TableEntries[4]);

	if(fPartLbaSize > 0) {
		setPartitionName(PARTITION_XBOX_NAME_DRIVE_F, &partitionTable->TableEntries[5]);
		setPartitionLBAStart(PARTITION_XBOX_DATA_F_LBA_START, &partitionTable->TableEntries[5]);
		setPartitionLBASize(fPartLbaSize - 1, &partitionTable->TableEntries[5]);
	}
}

NTSTATUS harddrive::readLbaInfo(int harddrive, XboxPartitionTable *p_table, ULONGLONG *total_sectors, DWORD *version)
{
	STRING a_file;
	OBJECT_ATTRIBUTES obj_attr;
	IO_STATUS_BLOCK io_stat_block;
	HANDLE handle;
	NTSTATUS stat;
	unsigned int lba48_ioctl_cmd_in_buf[100];
	unsigned int lba48_ioctl_cmd_out_buf[100];
	unsigned int geom_ioctl_cmd_in_buf[100];
	unsigned int partition_table_addr;
	DISK_GEOMETRY DiskGeometry;

	memset(p_table, 0, sizeof(XboxPartitionTable));

	RtlInitAnsiString(&a_file, harddrive == 0 ? "\\Device\\Harddisk0\\partition0" : "\\Device\\Harddisk1\\partition0");
	obj_attr.RootDirectory = 0;
	obj_attr.ObjectName = &a_file;
	obj_attr.Attributes = OBJ_CASE_INSENSITIVE;

	stat = NtOpenFile(&handle, (GENERIC_READ|0x00100000), &obj_attr, &io_stat_block, (FILE_SHARE_READ|FILE_SHARE_WRITE), 0x10);
	if (stat != STATUS_SUCCESS) 
	{
		return stat;
	}

	memset(geom_ioctl_cmd_in_buf, 0, sizeof(geom_ioctl_cmd_in_buf));
	geom_ioctl_cmd_in_buf[0] = IOCTL_SUBCMD_GET_INFO;

	stat = NtDeviceIoControlFile(handle, 0, 0, 0, &io_stat_block, IOCTL_DISK_GET_DRIVE_GEOMETRY, geom_ioctl_cmd_in_buf, sizeof(geom_ioctl_cmd_in_buf), &DiskGeometry, sizeof(DiskGeometry));
	if (stat == STATUS_SUCCESS)
	{
		*total_sectors = DiskGeometry.Cylinders.QuadPart;
	}
	else
	{
		*total_sectors = 0;
	}

	if (stat == STATUS_SUCCESS)
	{
		memset(lba48_ioctl_cmd_in_buf, 0, sizeof(lba48_ioctl_cmd_in_buf));
		lba48_ioctl_cmd_in_buf[0] = IOCTL_SUBCMD_GET_INFO;

		memset(lba48_ioctl_cmd_out_buf, 0, sizeof(lba48_ioctl_cmd_out_buf));

		stat = NtDeviceIoControlFile(handle, 0, 0, 0, &io_stat_block, IOCTL_CMD_LBA48_ACCESS, lba48_ioctl_cmd_in_buf, sizeof(lba48_ioctl_cmd_in_buf), lba48_ioctl_cmd_out_buf, sizeof(lba48_ioctl_cmd_out_buf));
	}

	NtClose(handle);
	if (stat != STATUS_SUCCESS) 
	{
		return stat;
	}

	if ((lba48_ioctl_cmd_out_buf[LBA48_GET_INFO_MAGIC1_IDX] != LBA48_GET_INFO_MAGIC1_VAL) || (lba48_ioctl_cmd_out_buf[LBA48_GET_INFO_MAGIC2_IDX] != LBA48_GET_INFO_MAGIC2_VAL)) 
	{
		return STATUS_UNSUCCESSFUL;
	}

	partition_table_addr = lba48_ioctl_cmd_out_buf[LBA48_GET_INFO_LOWCODE_BASE_IDX];
	partition_table_addr += lba48_ioctl_cmd_out_buf[LBA48_GET_INFO_PART_TABLE_OFS_IDX];

	*version = lba48_ioctl_cmd_out_buf[LBA48_GET_INFO_PATCHCODE_VERSION_IDX];

	if (*version == 2)
	{
		partition_table_addr += 0x200;
	}

	memcpy(p_table, (void *)partition_table_addr, sizeof(XboxPartitionTable));

	return STATUS_SUCCESS;
}

NTSTATUS harddrive::writePartitionTable(int harddrive, XboxPartitionTable *p_table)
{
	HANDLE hDisk = NULL;
	NTSTATUS status = 0;
	STRING a_path;
	OBJECT_ATTRIBUTES ObjectAttributes;
	IO_STATUS_BLOCK IoStatusBlock;
	LARGE_INTEGER RWOffset;
	FILE_FS_SIZE_INFORMATION fsi;
	ULONG PartTableSize = sizeof(XboxPartitionTable);

	memset(&fsi, 0, sizeof(fsi));
	RtlInitAnsiString(&a_path, harddrive == 0 ? "\\Device\\Harddisk0\\partition0" : "\\Device\\Harddisk1\\partition0");
	InitializeObjectAttributes(&ObjectAttributes, &a_path, OBJ_CASE_INSENSITIVE, NULL);

	status = NtOpenFile(&hDisk, SYNCHRONIZE | GENERIC_READ | GENERIC_WRITE, &ObjectAttributes, &IoStatusBlock, (FILE_SHARE_READ|FILE_SHARE_WRITE), FILE_SYNCHRONOUS_IO_ALERT);
	if(status < 0) {
		goto CleanupAndReturn;
	}

	RWOffset.QuadPart = 0;
	status = NtWriteFile(hDisk, NULL, NULL, NULL, &IoStatusBlock, (PVOID)p_table, PartTableSize, &RWOffset);
	if(status < 0) {
		goto CleanupAndReturn;
	}

	status = NtQueryVolumeInformationFile(hDisk, &IoStatusBlock, &fsi, sizeof(fsi), FileFsSizeInformation);
	if(status < 0) {
		goto CleanupAndReturn;
	}

	// Write a backup of the partition table at the end of the drive.
	// We removed a sector from the last partition in initXboxPartitionTable()
	//  in order to accommodate the backup.
	RWOffset.QuadPart = (fsi.TotalAllocationUnits.QuadPart * (LONGLONG)(fsi.BytesPerSector * fsi.SectorsPerAllocationUnit)) - PartTableSize;
	status = NtWriteFile(hDisk, NULL, NULL, NULL, &IoStatusBlock, (PVOID)p_table, PartTableSize, &RWOffset);
	if(status < 0) {
		goto CleanupAndReturn;
	}

CleanupAndReturn:
	NtClose(hDisk);
	return status;
}

bool harddrive::largePartitionFixup(ULONG harddrive, XboxPartitionTableEntry* entry, ULONG clusterSize) {
	NTSTATUS status;
	OBJECT_ATTRIBUTES ObjectAttributes;
	IO_STATUS_BLOCK IoStatusBlock;
	DISK_GEOMETRY DiskGeometry;
	ULONG SectorShift;
	LARGE_INTEGER ByteOffset;
	LARGE_INTEGER Offset;
	ULONG ReadLength = ROUND_TO_PAGES(sizeof(FAT_VOLUME_METADATA));
	HANDLE VolumeHandle = NULL;
	PFAT_VOLUME_METADATA VolumeMetadata = NULL;
	bool ret = false;

	Offset.QuadPart = 0;

	char* partition = stringUtility::formatString("\\Device\\Harddisk%i\\Partition0", harddrive);
	STRING partitionString;
	RtlInitAnsiString(&partitionString, partition);

	InitializeObjectAttributes(&ObjectAttributes, &partitionString,	OBJ_CASE_INSENSITIVE, NULL);

	if(status = NtOpenFile(&VolumeHandle, SYNCHRONIZE | GENERIC_READ |
		GENERIC_WRITE, &ObjectAttributes, &IoStatusBlock, (FILE_SHARE_READ|FILE_SHARE_WRITE),
		FILE_SYNCHRONOUS_IO_ALERT | FILE_NO_INTERMEDIATE_BUFFERING)) {
			goto CleanupAndReturn;
	}

	if(status = NtDeviceIoControlFile(VolumeHandle, NULL, NULL, NULL, &IoStatusBlock,
		IOCTL_DISK_GET_DRIVE_GEOMETRY, NULL, 0, &DiskGeometry,
		sizeof(DiskGeometry))) {
			goto CleanupAndReturn;
	}

	SectorShift = utils::FindFirstSetRightMember(DiskGeometry.BytesPerSector);
	Offset.HighPart = entry->LBAStartHi;
	Offset.LowPart = entry->LBAStartLo;
	Offset.QuadPart <<= SectorShift;

	VolumeMetadata = (PFAT_VOLUME_METADATA)malloc(ReadLength);

	ByteOffset = Offset;
	if(status = NtReadFile(VolumeHandle, NULL, NULL, NULL, &IoStatusBlock,
		VolumeMetadata, ReadLength, &ByteOffset)) {
			goto CleanupAndReturn;
	}

    if(memcmp(&VolumeMetadata->Signature, "FATX", 4) == 0 && VolumeMetadata->SectorsPerCluster == 0) {
		VolumeMetadata->SectorsPerCluster = (ULONG)(clusterSize >> SectorShift);

		ByteOffset = Offset;
		if(status = NtWriteFile(VolumeHandle, NULL, NULL, NULL, &IoStatusBlock,
			VolumeMetadata, ReadLength, &ByteOffset)) {
				goto CleanupAndReturn;
		}
	}

    ret = true;

CleanupAndReturn:
	NtClose(VolumeHandle);
	free(VolumeMetadata);
	free(partition);
	return ret;
}
