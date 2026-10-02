#include "sdcard.h"
#include "lpcWindow.h"

#include <cstring>

#define SDCARD_MAPPER_ID 3

namespace {
void convertSlashesToForward(char* path) {
	if (path == NULL) {
		return;
	}
	for (char* p = path; *p != 0; p++) {
		if (*p == '\\') {
			*p = '/';
		}
	}
}

void convertSlashesToBackslash(char* path) {
	if (path == NULL) {
		return;
	}
	for (char* p = path; *p != 0; p++) {
		if (*p == '/') {
			*p = '\\';
		}
	}
}

void writePayloadPath(uint8_t payloadType, const char* path) {
	char* lpcPath = strdup(path);
	if (lpcPath == NULL) {
		return;
	}
	convertSlashesToForward(lpcPath);

	lpcWindow::sendCustomCommandData(SDCARD_COMMAND_SET_PAYLOAD_TYPE, payloadType);
	volatile uint8_t* lpcMemMap = (volatile uint8_t*)LPC_MEMORY_BASE;
	strcpy((char*)lpcMemMap, lpcPath);
	free(lpcPath);
}
} // namespace

void sdcard::init() {
	lpcWindow::sendCommandData(LPC_MAPPER_SHORTCUT_ENABLE, 0);
	lpcWindow::sendCommandData(LPC_MEM_WIN_CMD_CUR_WIN_ID, SDCARD_MAPPER_ID);
	close();
}

void sdcard::wait_ready(uint8_t param)
{
	while (true)
	{
		unsigned char ready = lpcWindow::receiveCustomCommandData(param);
		if (ready == 1)
		{
			break;
		}
		Sleep(2);
	}
}

void sdcard::open()
{
	lpcWindow::sendCommandData(LPC_MEM_WIN_CMD_CUR_WIN_ID, SDCARD_MAPPER_ID);
	lpcWindow::sendCommandData(LPC_MEM_WIN_CMD_WIN_TYPE, LPC_MEM_WIN_TYPE_SDCARD);
	lpcWindow::sendLong(0xff000000);
	lpcWindow::sendCommandData(LPC_MEM_WIN_CMD_WIN_BASE_ADDR, 0);
	lpcWindow::sendLong(0x01000000);
	lpcWindow::sendCommandData(LPC_MEM_WIN_CMD_WIN_LENGTH, 0);
	lpcWindow::sendCommandData(LPC_MEM_WIN_CMD_WIN_FLAGS, LPC_MEM_WIN_FLAG_READ_ENABLED | LPC_MEM_WIN_FLAG_WRITE_ENABLED);
	lpcWindow::sendCommandData(LPC_MEM_WIN_CMD_ENABLE, 1);
}

void sdcard::close()
{
	lpcWindow::sendCommandData(LPC_MEM_WIN_CMD_WIN_FLAGS, 0);
	lpcWindow::sendCommandData(LPC_MEM_WIN_CMD_ENABLE, 0);
}

bool sdcard::remount()
{
	lpcWindow::sendCustomCommandData(SDCARD_COMMAND_REQUEST_REMOUNT, 0);
	wait_ready(SDCARD_COMMAND_RESPONSE_REMOUNT_READY);
	uint8_t result = lpcWindow::receiveCustomCommandData(SDCARD_COMMAND_RESPONSE_REMOUNT_RESULT);
	return result == SDCARD_FILE_RESULT_OK;
}

uint8_t sdcard::get_file_list(uint32_t offset)
{
	lpcWindow::sendLong(offset);
	lpcWindow::sendCustomCommandData(SDCARD_COMMAND_REQUEST_FILE_LIST, 0);
	wait_ready(SDCARD_COMMAND_RESPONSE_FILE_LIST_READY);

	uint8_t result = lpcWindow::receiveCustomCommandData(SDCARD_COMMAND_RESPONSE_FILE_LIST_RESULT);
	if (result != SDCARD_FILE_RESULT_OK)
	{
		return 0;
	}

	uint8_t count = lpcWindow::receiveCustomCommandData(SDCARD_COMMAND_RESPONSE_FILE_LIST_COUNT);
	return count;
}

bool sdcard::get_file_info_from_index(uint8_t index)
{
	lpcWindow::sendCustomCommandData(SDCARD_COMMAND_REQUEST_FILE_INFO_FROM_INDEX, index);
	wait_ready(SDCARD_COMMAND_RESPONSE_FILE_INFO_READY);

	uint8_t result = lpcWindow::receiveCustomCommandData(SDCARD_COMMAND_RESPONSE_FILE_INFO_RESULT);
	return result != SDCARD_FILE_RESULT_OK;
}

bool sdcard::get_file_info_from_path()
{
	lpcWindow::sendCustomCommandData(SDCARD_COMMAND_REQUEST_FILE_INFO_FROM_PATH, 0);
	wait_ready(SDCARD_COMMAND_RESPONSE_FILE_INFO_READY);

	uint8_t result = lpcWindow::receiveCustomCommandData(SDCARD_COMMAND_RESPONSE_FILE_INFO_RESULT);
	return result != SDCARD_FILE_RESULT_OK;
}

uint8_t sdcard::get_file_flags()
{
	uint8_t file_flags = lpcWindow::receiveCustomCommandData(SDCARD_COMMAND_GET_FILE_INFO_FLAGS);
	return file_flags;	
}

bool sdcard::is_file_read_only(uint8_t file_flags)
{
	return (file_flags & AM_RDO) != 0;
}

bool sdcard::is_file_hidden(uint8_t file_flags)
{
	return (file_flags & AM_HID) != 0;
}

bool sdcard::is_file_system(uint8_t file_flags)
{
	return (file_flags & AM_SYS) != 0;
}

bool sdcard::is_file_directory(uint8_t file_flags)
{
	return (file_flags & AM_DIR) != 0;
}

bool sdcard::is_file_archive(uint8_t file_flags)
{
	return (file_flags & AM_ARC) != 0;
}

bool sdcard::is_file_visible(uint8_t file_flags)
{
	return !is_file_hidden(file_flags) && !is_file_system(file_flags);
}

uint32_t sdcard::get_file_size()
{
	lpcWindow::sendCustomCommandData(SDCARD_COMMAND_GET_FILE_INFO_SIZE, 0);
	uint32_t result = lpcWindow::readLong();
	return result;
}

char* sdcard::get_file_name()
{
	volatile unsigned char* lpcMemMap = (volatile unsigned char*)LPC_MEMORY_BASE;
	lpcWindow::sendCustomCommandData(SDCARD_COMMAND_SET_PAYLOAD_TYPE, SDCARD_PAYLOAD_TYPE_FILE_NAME);
	return strdup((char*)lpcMemMap);
}

uint32_t sdcard::get_file_modified()
{
	lpcWindow::sendCustomCommandData(SDCARD_COMMAND_GET_FILE_INFO_MODIFIED, 0);
	uint32_t result = lpcWindow::readLong();
	return result;
}

uint32_t sdcard::get_file_created()
{
	lpcWindow::sendCustomCommandData(SDCARD_COMMAND_GET_FILE_INFO_CREATED, 0);
	uint32_t result = lpcWindow::readLong();
	return result;
}

bool sdcard::open_file_from_index(uint8_t index)
{
	lpcWindow::sendCustomCommandData(SDCARD_COMMAND_REQUEST_OPEN_FILE_FROM_INDEX, index);
	wait_ready(SDCARD_COMMAND_RESPONSE_OPEN_FILE_READY);

	uint8_t result = lpcWindow::receiveCustomCommandData(SDCARD_COMMAND_RESPONSE_OPEN_FILE_RESULT);
	return result == SDCARD_FILE_RESULT_OK;
}

bool sdcard::open_file_from_path(const char* path)
{
	set_path(path);

	lpcWindow::sendCustomCommandData(SDCARD_COMMAND_REQUEST_OPEN_FILE_FROM_PATH, 0);
	wait_ready(SDCARD_COMMAND_RESPONSE_OPEN_FILE_READY);

	uint8_t result = lpcWindow::receiveCustomCommandData(SDCARD_COMMAND_RESPONSE_OPEN_FILE_RESULT);
	return result == SDCARD_FILE_RESULT_OK;
}

bool sdcard::get_volume_space()
{
	lpcWindow::sendCustomCommandData(SDCARD_COMMAND_REQUEST_VOLUME_SPACE, 0);
	wait_ready(SDCARD_COMMAND_RESPONSE_VOLUME_SPACE_READY);

	uint8_t result = lpcWindow::receiveCustomCommandData(SDCARD_COMMAND_RESPONSE_VOLUME_SPACE_RESULT);
	return result == SDCARD_FILE_RESULT_OK;
}

uint64_t sdcard::get_volume_total()
{
	lpcWindow::sendCustomCommandData(SDCARD_COMMAND_GET_VOLUME_TOTAL, SDCARD_VOLUME_SPACE_DWORD_HIGH);
	uint64_t result = lpcWindow::readLong() << 32;
	lpcWindow::sendCustomCommandData(SDCARD_COMMAND_GET_VOLUME_TOTAL, SDCARD_VOLUME_SPACE_DWORD_LOW);
	result |= lpcWindow::readLong();
	return result;
}

uint64_t sdcard::get_volume_free()
{
	lpcWindow::sendCustomCommandData(SDCARD_COMMAND_GET_VOLUME_FREE, SDCARD_VOLUME_SPACE_DWORD_HIGH);
	uint64_t result = lpcWindow::readLong() << 32;
	lpcWindow::sendCustomCommandData(SDCARD_COMMAND_GET_VOLUME_FREE, SDCARD_VOLUME_SPACE_DWORD_LOW);
	result |= lpcWindow::readLong();
	return result;
}

bool sdcard::create_directory(const char* path)
{
	set_path(path);

	lpcWindow::sendCustomCommandData(SDCARD_COMMAND_REQUEST_CREATE_DIR_FROM_PATH, 0);
	wait_ready(SDCARD_COMMAND_RESPONSE_PATH_CREATE_READY);

	uint8_t result = lpcWindow::receiveCustomCommandData(SDCARD_COMMAND_RESPONSE_PATH_CREATE_RESULT);
	return result == SDCARD_FILE_RESULT_OK;
}

bool sdcard::create_file(const char* path, uint32_t size)
{
	set_path(path);

	lpcWindow::sendLong(size);
	lpcWindow::sendCustomCommandData(SDCARD_COMMAND_REQUEST_CREATE_FILE_FROM_PATH, 0);
	wait_ready(SDCARD_COMMAND_RESPONSE_PATH_CREATE_READY);

	uint8_t result = lpcWindow::receiveCustomCommandData(SDCARD_COMMAND_RESPONSE_PATH_CREATE_RESULT);
	return result == SDCARD_FILE_RESULT_OK;
}

bool sdcard::delete_path(const char* path)
{
	set_path(path);

	lpcWindow::sendCustomCommandData(SDCARD_COMMAND_REQUEST_DELETE_FROM_PATH, 0);
	wait_ready(SDCARD_COMMAND_RESPONSE_PATH_DELETE_READY);

	uint8_t result = lpcWindow::receiveCustomCommandData(SDCARD_COMMAND_RESPONSE_PATH_DELETE_RESULT);
	return result == SDCARD_FILE_RESULT_OK;
}

bool sdcard::rename_path(const char* sourcePath, const char* destPath)
{
	writePayloadPath(SDCARD_PAYLOAD_TYPE_PATH, sourcePath);
	writePayloadPath(SDCARD_PAYLOAD_TYPE_PATH_DST, destPath);

	lpcWindow::sendCustomCommandData(SDCARD_COMMAND_REQUEST_RENAME_FROM_PATH, 0);
	wait_ready(SDCARD_COMMAND_RESPONSE_PATH_RENAME_READY);

	uint8_t result = lpcWindow::receiveCustomCommandData(SDCARD_COMMAND_RESPONSE_PATH_RENAME_RESULT);
	return result == SDCARD_FILE_RESULT_OK;
}

void sdcard::map_bios_to_mem()
{
	lpcWindow::sendCustomCommandData(SDCARD_COMMAND_SET_PAYLOAD_TYPE, SDCARD_PAYLOAD_TYPE_FILE_SD_BIOS);
	lpcWindow::sendCommandData(LPC_MAPPER_SHORTCUT_ENABLE, 1);
}

bool sdcard::write_chunk(uint32_t chunk_index, uint8_t* chunk_data, uint16_t chunk_length)
{
	lpcWindow::sendLong(chunk_length);
	lpcWindow::sendCustomCommandData(SDCARD_COMMAND_SET_FILE_WRITE_CHUNK_SIZE, 0);

	volatile unsigned char* lpcMemMap = (volatile unsigned char*)LPC_MEMORY_BASE;
	lpcWindow::sendCustomCommandData(SDCARD_COMMAND_SET_PAYLOAD_TYPE, SDCARD_PAYLOAD_TYPE_FILE_SD_CHUNK);
	memcpy((void*)lpcMemMap, chunk_data, chunk_length);

	lpcWindow::sendLong(chunk_index);
	lpcWindow::sendCustomCommandData(SDCARD_COMMAND_REQUEST_WRITE_FILE_CHUNK, 0);
	wait_ready(SDCARD_COMMAND_RESPONSE_FILE_WRITE_READY);

	uint8_t result = lpcWindow::receiveCustomCommandData(SDCARD_COMMAND_RESPONSE_FILE_WRITE_RESULT);
	return result == SDCARD_FILE_RESULT_OK;
}

uint8_t* sdcard::read_chunk(uint32_t chunk_index, uint32_t* chunk_length)
{
	lpcWindow::sendLong(chunk_index);
    uint8_t result = lpcWindow::receiveCustomCommandData(SDCARD_COMMAND_FILE_READ_CHUNK);
	*chunk_length = lpcWindow::readLong();
	if (result != SDCARD_FILE_RESULT_OK)
	{
		return NULL;
	}

	uint8_t* chunk_data = (uint8_t*)malloc(*chunk_length);
	if (chunk_data == NULL) {
		return NULL;
	}
	lpcWindow::sendCustomCommandData(SDCARD_COMMAND_SET_PAYLOAD_TYPE, SDCARD_PAYLOAD_TYPE_FILE_SD_CHUNK);
	volatile uint8_t* lpcMemMap = (volatile uint8_t*)LPC_MEMORY_BASE;
	for (uint32_t i = 0; i < *chunk_length; i++) {
		chunk_data[i] = lpcMemMap[i];
	}
	return chunk_data;
}

bool sdcard::flash_chunk(uint32_t chunk_index, uint32_t flash_offset)
{
	lpcWindow::sendLong(chunk_index);
    uint8_t result = lpcWindow::receiveCustomCommandData(SDCARD_COMMAND_FILE_READ_CHUNK);
	if (result != SDCARD_FILE_RESULT_OK)
	{
		return false;
	}

	lpcWindow::sendLong(flash_offset);
	lpcWindow::sendCustomCommandData(SDCARD_COMMAND_REQUEST_FLASH_CHUNK, 0);
	wait_ready(SDCARD_COMMAND_RESPONSE_FLASH_CHUNK_READY);

	result = lpcWindow::receiveCustomCommandData(SDCARD_COMMAND_RESPONSE_FLASH_CHUNK_RESULT);
	return result == SDCARD_FILE_RESULT_OK;
}

void sdcard::set_cwd(const char* path)
{
	char* lpcPath = strdup(path);
	if (lpcPath == NULL) {
		return;
	}
	convertSlashesToForward(lpcPath);

	lpcWindow::sendCustomCommandData(SDCARD_COMMAND_SET_PAYLOAD_TYPE, SDCARD_PAYLOAD_TYPE_CWD);
	volatile uint8_t* lpcMemMap = (volatile uint8_t*)LPC_MEMORY_BASE;
	strcpy((char*)lpcMemMap, lpcPath);
	free(lpcPath);
}

char* sdcard::get_cwd()
{
	lpcWindow::sendCustomCommandData(SDCARD_COMMAND_SET_PAYLOAD_TYPE, SDCARD_PAYLOAD_TYPE_CWD);
	volatile uint8_t* lpcMemMap = (volatile uint8_t*)LPC_MEMORY_BASE;
	char* result = strdup((char*)lpcMemMap);
	convertSlashesToBackslash(result);
	return result;
}

void sdcard::set_path(const char* path)
{
	writePayloadPath(SDCARD_PAYLOAD_TYPE_PATH, path);
}

char* sdcard::get_path()
{
	lpcWindow::sendCustomCommandData(SDCARD_COMMAND_SET_PAYLOAD_TYPE, SDCARD_PAYLOAD_TYPE_PATH);
	volatile uint8_t* lpcMemMap = (volatile uint8_t*)LPC_MEMORY_BASE;
	char* result = strdup((char*)lpcMemMap);
	convertSlashesToBackslash(result);
	return result;
}

bool sdcard::cwd(uint8_t index)
{
    uint8_t result = lpcWindow::receiveCustomCommandData(SDCARD_COMMAND_SET_CWD_FROM_INDEX);
	return result == SDCARD_FILE_RESULT_OK;
}

bool sdcard::cwd_root()
{
    uint8_t result = lpcWindow::receiveCustomCommandData(SDCARD_COMMAND_CWD_ROOT);
	return result == SDCARD_FILE_RESULT_OK;
}

bool sdcard::cwd_parent()
{
    uint8_t result = lpcWindow::receiveCustomCommandData(SDCARD_COMMAND_CWD_PARENT, 0);
	return result == SDCARD_FILE_RESULT_OK;
}

bool sdcard::close_file()
{
	uint8_t result = lpcWindow::receiveCustomCommandData(SDCARD_COMMAND_CLOSE_FILE);
	return result == SDCARD_FILE_RESULT_OK;
}