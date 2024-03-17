#include "network.h"
#include "context.h"
#include "socketUtility.h"
#include "ftpServer.h"
#include "xboxInternals.h"

void network::init()
{
	if ((XNetGetEthernetLinkStatus() & XNET_ETHERNET_LINK_ACTIVE) > 0 && context::getNetworkInitialized() == false)
	{
		XNetStartupParams xnsp;
		memset(&xnsp, 0, sizeof(xnsp));
		xnsp.cfgSizeOfStruct = sizeof(XNetStartupParams);
		xnsp.cfgFlags = XNET_STARTUP_BYPASS_SECURITY;

		xnsp.cfgPrivatePoolSizeInPages = 64;
		xnsp.cfgEnetReceiveQueueLength = 16;
		xnsp.cfgIpFragMaxSimultaneous = 16;
		xnsp.cfgIpFragMaxPacketDiv256 = 32;
		xnsp.cfgSockMaxSockets = 64;

		xnsp.cfgSockDefaultRecvBufsizeInK = RECV_SOCKET_BUFFER_SIZE_IN_K;
		xnsp.cfgSockDefaultSendBufsizeInK = SEND_SOCKET_BUFFER_SIZE_IN_K;

		XNetStartup(&xnsp);

		WSADATA wsaData;
		const int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (result != 0)
		{
			utils::debugPrint("Error: Socket startup failed: %i", result);
		}

		ftpServer::init();

		context::setNetworkInitialized(true);
	}
}

void network::restart()
{
	context::setNetworkInitialized(false);
	ftpServer::close();
	WSACleanup();
	XNetCleanup();
	init();
}

bool network::isReady()
{
	if (context::getNetworkInitialized() == false)
	{
		return false;
	}
	XNADDR addr;
	memset(&addr, 0, sizeof(addr));
	DWORD dwState = XNetGetTitleXnAddr(&addr);
	return dwState != XNET_GET_XNADDR_PENDING;
}

uint32_t network::getAddress()
{
	XNetConfigStatus status;
	memset(&status, 0, sizeof(status));
	XNetGetConfigStatus(&status);
	return status.ina.S_un.S_addr;
}

uint32_t network::getSubnet()
{
	XNetConfigStatus status;
	memset(&status, 0, sizeof(status));
	XNetGetConfigStatus(&status);
	return status.inaMask.S_un.S_addr;
}

uint32_t network::getGateway()
{
	XNetConfigStatus status;
	memset(&status, 0, sizeof(status));
	XNetGetConfigStatus(&status);
	return status.inaGateway.S_un.S_addr;
}

uint32_t network::getPrimaryDns()
{
	XNetConfigStatus status;
	memset(&status, 0, sizeof(status));
	XNetGetConfigStatus(&status);
	return status.inaDnsSecondary.S_un.S_addr;
}

uint32_t network::getSecondaryDns()
{
	XNetConfigStatus status;
	memset(&status, 0, sizeof(status));
	XNetGetConfigStatus(&status);
	return status.inaDnsSecondary.S_un.S_addr;
}