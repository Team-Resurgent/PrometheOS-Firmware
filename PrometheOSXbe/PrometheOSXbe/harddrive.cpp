#include "harddrive.h"

#include "xboxInternals.h"

unsigned int harddrive::readLbaInfo(XboxPartitionTable *p_table, XboxPartitionTable **p_table_add, DWORD *total_sectors, DWORD *version)
{
	STRING a_file;
	OBJECT_ATTRIBUTES obj_attr;
	IO_STATUS_BLOCK io_stat_block;
	HANDLE handle;
	unsigned int stat;
	unsigned int lba48_ioctl_cmd_in_buf[100];
	unsigned int lba48_ioctl_cmd_out_buf[100];
	unsigned int geom_ioctl_cmd_in_buf[100];
	unsigned int partition_table_addr;
	DISK_GEOMETRY DiskGeometry;

	memset(p_table, 0, sizeof(XboxPartitionTable));

	RtlInitAnsiString(&a_file, "\\Device\\Harddisk0\\partition0");
	obj_attr.RootDirectory = 0;
	obj_attr.ObjectName = &a_file;
	obj_attr.Attributes = OBJ_CASE_INSENSITIVE;

	stat = NtOpenFile(&handle, (GENERIC_READ|0x00100000),
		&obj_attr, &io_stat_block, (FILE_SHARE_READ|FILE_SHARE_WRITE), 0x10);

	if (stat != STATUS_SUCCESS) {
		return stat;
	}

//////////////////// geom ////////////////////
		memset(geom_ioctl_cmd_in_buf, 0, sizeof(geom_ioctl_cmd_in_buf));
		geom_ioctl_cmd_in_buf[0] = IOCTL_SUBCMD_GET_INFO;

		stat = NtDeviceIoControlFile(handle, 0, 0, 0, &io_stat_block,
			IOCTL_DISK_GET_DRIVE_GEOMETRY,
			geom_ioctl_cmd_in_buf, sizeof(geom_ioctl_cmd_in_buf),
			&DiskGeometry, sizeof(DiskGeometry));
		if (stat == STATUS_SUCCESS)
		{
			*total_sectors = DiskGeometry.Cylinders.LowPart;
		}
		else
			*total_sectors = 0;

//////////////////// lba48 ////////////////////
		if (stat == STATUS_SUCCESS)
		{
			memset(lba48_ioctl_cmd_in_buf, 0, sizeof(lba48_ioctl_cmd_in_buf));
			lba48_ioctl_cmd_in_buf[0] = IOCTL_SUBCMD_GET_INFO;

			memset(lba48_ioctl_cmd_out_buf, 0, sizeof(lba48_ioctl_cmd_out_buf));

			stat = NtDeviceIoControlFile(handle, 0, 0, 0, &io_stat_block,
				IOCTL_CMD_LBA48_ACCESS,
				lba48_ioctl_cmd_in_buf, sizeof(lba48_ioctl_cmd_in_buf),
				lba48_ioctl_cmd_out_buf, sizeof(lba48_ioctl_cmd_out_buf));
		}
//////////////////////////////////////////////

	NtClose(handle);
	if (stat != STATUS_SUCCESS) {
		return stat;
	}

	if ((lba48_ioctl_cmd_out_buf[LBA48_GET_INFO_MAGIC1_IDX] != LBA48_GET_INFO_MAGIC1_VAL) ||
		(lba48_ioctl_cmd_out_buf[LBA48_GET_INFO_MAGIC2_IDX] != LBA48_GET_INFO_MAGIC2_VAL)) {

			return STATUS_UNSUCCESSFUL;
		}

		// this way is compatible with Paul's lba48 patched kernels
		partition_table_addr = lba48_ioctl_cmd_out_buf[LBA48_GET_INFO_LOWCODE_BASE_IDX];
		partition_table_addr += lba48_ioctl_cmd_out_buf[LBA48_GET_INFO_PART_TABLE_OFS_IDX];

		*version = lba48_ioctl_cmd_out_buf[LBA48_GET_INFO_PATCHCODE_VERSION_IDX];

		if (*version == 2)
			partition_table_addr += 0x200;

		*p_table_add = (XboxPartitionTable *) partition_table_addr;


		memcpy(p_table, (void *) partition_table_addr, sizeof(XboxPartitionTable));

		return STATUS_SUCCESS;
}

NTSTATUS harddrive::writePartitionTable(XboxPartitionTable *p_table)
{
	HANDLE hDisk;
	NTSTATUS status;
	STRING a_path;
	OBJECT_ATTRIBUTES ObjectAttributes;
	IO_STATUS_BLOCK IoStatusBlock;
	BYTE PartSector[XBOX_HD_SECTOR_SIZE];
	LARGE_INTEGER RWOffset;


	RtlInitAnsiString(&a_path, "\\Device\\Harddisk0\\partition0");
	InitializeObjectAttributes(&ObjectAttributes, &a_path, OBJ_CASE_INSENSITIVE, NULL);

	RWOffset.QuadPart = 0;
	memset(PartSector, 0, sizeof(PartSector));
	if(p_table)
		memcpy(PartSector, p_table, sizeof(XboxPartitionTable));

	status = NtOpenFile(&hDisk, SYNCHRONIZE | GENERIC_ALL, &ObjectAttributes, &IoStatusBlock, 0, FILE_SYNCHRONOUS_IO_ALERT);
	if (status >= 0)
	{
		status = NtWriteFile(hDisk, NULL, NULL, NULL, &IoStatusBlock, (PVOID) PartSector, 512, &RWOffset);
	}
	NtClose(hDisk);
	return status;

}