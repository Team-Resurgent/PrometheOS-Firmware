#pragma once

#define LPC_MEM_WIN_VERSION                           0x01
#define NUM_LPC_MEM_WINDOWS                           4
#define LPC_MEM_WIN_IO_BASE                           0x2888
#define LPC_MEM_WIN_LONG_DATA_PORT_BASE               (LPC_MEM_WIN_IO_BASE + 0)
#define LPC_MEM_WIN_CMD_PORT                          (LPC_MEM_WIN_IO_BASE + 4)
#define LPC_MEM_WIN_DATA_PORT                         (LPC_MEM_WIN_IO_BASE + 5)
#define LPC_MEM_WIN_VERSION_PORT                      (LPC_MEM_WIN_IO_BASE + 7)

#define LPC_MEM_WIN_CMD_ENABLE                        0
#define LPC_MEM_WIN_CMD_CUR_WIN_ID                    1
#define LPC_MEM_WIN_CMD_WIN_BASE_ADDR                 2
#define LPC_MEM_WIN_CMD_WIN_LENGTH                    3
#define LPC_MEM_WIN_CMD_WIN_TYPE                      4
#define LPC_MEM_WIN_CMD_WIN_FLAGS                     5
#define LPC_MEM_WIN_CMD_WIN_CTRL                      6
#define LPC_MEM_WIN_CMD_WIN_CTRL_CMD                  7
#define LPC_MEM_WIN_CMD_WIN_CTRL_DATA                 8
#define LPC_MAPPER_SHORTCUT_ENABLE                    9

#define LPC_MEM_WIN_TYPE_NONE                         0
#define LPC_MEM_WIN_TYPE_PSRAM                        1
#define LPC_MEM_WIN_TYPE_INTERNAL_RAM                 2
#define LPC_MEM_WIN_TYPE_RNG                          3
#define LPC_MEM_WIN_TYPE_ECHO                         4
#define LPC_MEM_WIN_TYPE_HEAP                         5
#define LPC_MEM_WIN_TYPE_SDCARD                       6
#define LPC_MEM_WIN_TYPE_EXPANSION                    7

#define LPC_MEM_WIN_FLAG_WRITE_ENABLED                (1 << 0)
#define LPC_MEM_WIN_FLAG_READ_ENABLED                 (1 << 1)

#define LPC_MEMORY_BASE 0xFF000000u

class lpcWindow {
  public:
	static void sendLong(unsigned int data);
	static unsigned int readLong();
	static void sendCommandData(unsigned char cmd, unsigned char data);
	static void sendCustomCommandData(unsigned char cmd, unsigned char data);
	static unsigned char receiveCommandData(unsigned char cmd);
	static unsigned char receiveCustomCommandData(unsigned char cmd, unsigned char data = 0);
};