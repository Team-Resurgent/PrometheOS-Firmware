#include "lpcWindow.h"

#include <conio.h>

#define LPC_MEMORY_BASE 0xFF000000u

void lpcWindow::sendLong(unsigned int data)
{
	_outpd(0x2888, data);
}

unsigned int lpcWindow::readLong()
{
	return _inpd(0x2888);
}

void lpcWindow::sendCommandData(unsigned char cmd, unsigned char data)
{
	_outpw(0x288C, (data << 8) | cmd);
}

void lpcWindow::sendCustomCommandData(unsigned char cmd, unsigned char data)
{
	sendCommandData(LPC_MEM_WIN_CMD_WIN_CTRL_CMD, cmd);
	sendCommandData(LPC_MEM_WIN_CMD_WIN_CTRL, data);
}

unsigned char lpcWindow::receiveCommandData(unsigned char cmd)
{
	_outp(0x288C, cmd);
	return _inp(0x288C);
}

unsigned char lpcWindow::receiveCustomCommandData(unsigned char cmd, unsigned char data)
{
	sendCommandData(LPC_MEM_WIN_CMD_WIN_CTRL_CMD, cmd);
	sendCommandData(LPC_MEM_WIN_CMD_WIN_CTRL_DATA, data);
	return receiveCommandData(LPC_MEM_WIN_CMD_WIN_CTRL);
}