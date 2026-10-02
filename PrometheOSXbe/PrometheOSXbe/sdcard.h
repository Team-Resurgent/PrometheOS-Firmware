#pragma once

#include "xboxinternals.h"

#define SDCARD_DISK_SECTOR_SIZE 512

#define SDCARD_MAX_ENTRIES 8
#define SDCARD_QUEUE_BUFFER_LEN 8
#define SDCARD_PATH_MAX 280

#define SDCARD_FILE_CHUNK_SIZE_SHIFT 12
#define SDCARD_FILE_CHUNK_SIZE (1 << SDCARD_FILE_CHUNK_SIZE_SHIFT)

#define SDCARD_CWD_RESULT_OK 0
#define SDCARD_CWD_RESULT_BAD_INDEX 1
#define SDCARD_CWD_RESULT_NOT_DIRECTORY 2
#define SDCARD_CWD_RESULT_PATH_TOO_LONG 3
#define SDCARD_CWD_RESULT_AT_ROOT 4

#define SDCARD_FILE_RESULT_IDLE 0 
#define SDCARD_FILE_RESULT_OK 1
#define SDCARD_FILE_RESULT_ERROR 2
#define SDCARD_FILE_RESULT_INVALID_INDEX 3
#define SDCARD_FILE_RESULT_IS_DIRECTORY 4

#define SDCARD_COMMAND_NONE 0

#define SDCARD_COMMAND_REQUEST_REMOUNT 1
#define SDCARD_COMMAND_RESPONSE_REMOUNT_READY 2
#define SDCARD_COMMAND_RESPONSE_REMOUNT_RESULT 3

#define SDCARD_COMMAND_REQUEST_FILE_LIST 4 // set long value for offset
#define SDCARD_COMMAND_RESPONSE_FILE_LIST_COUNT 5 
#define SDCARD_COMMAND_RESPONSE_FILE_LIST_READY 6
#define SDCARD_COMMAND_RESPONSE_FILE_LIST_RESULT 7

#define SDCARD_COMMAND_REQUEST_OPEN_FILE_FROM_INDEX 8 // data = index of list
#define SDCARD_COMMAND_REQUEST_OPEN_FILE_FROM_PATH 9
#define SDCARD_COMMAND_RESPONSE_OPEN_FILE_READY 10
#define SDCARD_COMMAND_RESPONSE_OPEN_FILE_RESULT 11

#define SDCARD_COMMAND_REQUEST_FLASH_CHUNK 12 // flashes loaded chunk at current_long offset
#define SDCARD_COMMAND_RESPONSE_FLASH_CHUNK_READY 13
#define SDCARD_COMMAND_RESPONSE_FLASH_CHUNK_RESULT 14

#define SDCARD_COMMAND_CLOSE_FILE 15

#define SDCARD_COMMAND_FILE_READ_CHUNK 16 // peek: current_long_val = chunk index; returns bytes read in long

#define SDCARD_COMMAND_CWD_ROOT 17
#define SDCARD_COMMAND_CWD_PARENT 18
#define SDCARD_COMMAND_SET_CWD_FROM_INDEX 19

#define SDCARD_COMMAND_REQUEST_FILE_INFO_FROM_INDEX 20
#define SDCARD_COMMAND_REQUEST_FILE_INFO_FROM_PATH 21
#define SDCARD_COMMAND_RESPONSE_FILE_INFO_READY 22
#define SDCARD_COMMAND_RESPONSE_FILE_INFO_RESULT 23
#define SDCARD_COMMAND_GET_FILE_INFO_SIZE 24
#define SDCARD_COMMAND_GET_FILE_INFO_FLAGS 25
#define SDCARD_COMMAND_GET_FILE_INFO_MODIFIED 26
#define SDCARD_COMMAND_GET_FILE_INFO_CREATED 27

#define SDCARD_COMMAND_REQUEST_VOLUME_SPACE 28
#define SDCARD_COMMAND_RESPONSE_VOLUME_SPACE_READY 29
#define SDCARD_COMMAND_RESPONSE_VOLUME_SPACE_RESULT 30
#define SDCARD_COMMAND_GET_VOLUME_FREE 31
#define SDCARD_COMMAND_GET_VOLUME_TOTAL 32

#define SDCARD_COMMAND_REQUEST_DELETE_FROM_PATH 33
#define SDCARD_COMMAND_RESPONSE_PATH_DELETE_READY 34
#define SDCARD_COMMAND_RESPONSE_PATH_DELETE_RESULT 35

#define SDCARD_COMMAND_REQUEST_CREATE_DIR_FROM_PATH 36
#define SDCARD_COMMAND_REQUEST_CREATE_FILE_FROM_PATH 37 // empty file; size grows via write chunks
#define SDCARD_COMMAND_RESPONSE_PATH_CREATE_READY 38
#define SDCARD_COMMAND_RESPONSE_PATH_CREATE_RESULT 39

#define SDCARD_COMMAND_SET_FILE_WRITE_CHUNK_SIZE 40 // current_long_val = byte count
#define SDCARD_COMMAND_REQUEST_WRITE_FILE_CHUNK 41 // current_long_val = chunk index
#define SDCARD_COMMAND_RESPONSE_FILE_WRITE_READY 42
#define SDCARD_COMMAND_RESPONSE_FILE_WRITE_RESULT 43
#define SDCARD_COMMAND_REQUEST_RENAME_FROM_PATH 44 // path = source, path_dst = destination
#define SDCARD_COMMAND_GET_DISK_SECTOR_COUNT 45 // set: data 0/1 = low/high dword of sector count
#define SDCARD_COMMAND_REQUEST_DISK_READ_SECTOR 46
#define SDCARD_COMMAND_RESPONSE_DISK_READ_READY 47
#define SDCARD_COMMAND_RESPONSE_DISK_READ_RESULT 48
#define SDCARD_COMMAND_REQUEST_DISK_WRITE_SECTOR 49
#define SDCARD_COMMAND_RESPONSE_DISK_WRITE_READY 50
#define SDCARD_COMMAND_RESPONSE_DISK_WRITE_RESULT 51

#define SDCARD_COMMAND_SET_PAYLOAD_TYPE 52

#define SDCARD_COMMAND_RESPONSE_PATH_RENAME_READY 53
#define SDCARD_COMMAND_RESPONSE_PATH_RENAME_RESULT 54

#define SDCARD_VOLUME_SPACE_DWORD_LOW 0
#define SDCARD_VOLUME_SPACE_DWORD_HIGH 1

#define SDCARD_DISK_SECTOR_DWORD_LOW 0
#define SDCARD_DISK_SECTOR_DWORD_HIGH 1

#define SDCARD_PAYLOAD_TYPE_NONE 0
#define SDCARD_PAYLOAD_TYPE_FILE_NAME 1
#define SDCARD_PAYLOAD_TYPE_FILE_SD_CHUNK 2
#define SDCARD_PAYLOAD_TYPE_FILE_SD_BIOS 3
#define SDCARD_PAYLOAD_TYPE_CWD 4
#define SDCARD_PAYLOAD_TYPE_PATH 5
#define SDCARD_PAYLOAD_TYPE_DISK_SECTOR 6
#define SDCARD_PAYLOAD_TYPE_PATH_DST 7

#define	AM_RDO	0x01	/* Read only */
#define	AM_HID	0x02	/* Hidden */
#define	AM_SYS	0x04	/* System */
#define AM_DIR	0x10	/* Directory */
#define AM_ARC	0x20	/* Archive */

class sdcard
{
public:
	static void init();
	static void wait_ready(uint8_t param);
	static void open();
	static void close();
	static bool remount();
	static uint8_t get_file_list(uint32_t offset);
	static bool get_file_info_from_index(uint8_t index);
	static bool get_file_info_from_path();
	static uint8_t get_file_flags();
	static bool is_file_read_only(uint8_t file_flags);
	static bool is_file_hidden(uint8_t file_flags);
	static bool is_file_system(uint8_t file_flags);
	static bool is_file_directory(uint8_t file_flags);
	static bool is_file_archive(uint8_t file_flags);
	static bool is_file_visible(uint8_t file_flags);
	static uint32_t get_file_size();
	static char* get_file_name();
	static uint32_t get_file_modified();
	static uint32_t get_file_created();
	static bool open_file_from_index(uint8_t index);
	static bool open_file_from_path(const char* path);
	static bool get_volume_space();
	static uint64_t get_volume_total();
	static uint64_t get_volume_free();
	static bool create_directory(const char* path);
	static bool create_file(const char* path, uint32_t size);
	static bool delete_path(const char* path);
	static bool rename_path(const char* sourcePath, const char* destPath);
	static void map_bios_to_mem();
	static bool write_chunk(uint32_t chunk_index, uint8_t* data, uint16_t chunk_length);
	static uint8_t* read_chunk(uint32_t chunk_index, uint32_t* chunk_length);
	static bool flash_chunk(uint32_t chunk_index, uint32_t flash_offset);
	static void set_cwd(const char* path);
	static char* get_cwd();
	static void set_path(const char* path);
	static char* get_path();
	static bool cwd(uint8_t index);
	static bool cwd_root();
	static bool cwd_parent();
	static bool close_file();
};
