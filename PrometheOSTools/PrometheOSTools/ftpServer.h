#pragma once

#include "socketUtility.h"
#include "fileSystem.h"
#include <cstdio>
#include <xtl.h>
#include <winsockx.h>

class ftpServer
{
public:

	typedef enum _ReceiveStatus {
		ReceiveStatus_OK = 1,
		ReceiveStatus_Network_Error,
		ReceiveStatus_Timeout,
		ReceiveStatus_Invalid_Data,
		ReceiveStatus_Insufficient_Buffer
	} ReceiveStatus;

	static bool WINAPI connectionThread(uint64_t pParam);

	static bool WINAPI listenThread(LPVOID lParam);

	static bool init();
	static void close();
	static bool socketSendString(uint64_t s, const char *psz);
	static ReceiveStatus socketReceiveString(uint64_t s, char *psz, uint32_t dwMaxChars, uint32_t* pdwCharsReceived);
	static ReceiveStatus socketReceiveLetter(uint64_t s, char* pch, uint32_t dwMaxChars, uint32_t* pdwCharsReceived);
	static ReceiveStatus socketReceiveData(uint64_t s, char *psz, uint32_t dwBytesToRead, uint32_t* pdwBytesRead);
	static uint64_t establishDataConnection(sockaddr_in* psaiData, uint64_t* psPasv);
	static bool receiveSocketFile(uint64_t sCmd, uint64_t sData, uint32_t fileHandle);
	static bool sendSocketFile(uint64_t sCmd, uint64_t sData, uint32_t fileHandle, uint32_t* pdwAbortFlag);
};
