#pragma once
#include <WinSock2.h>
#pragma comment(lib,"ws2_32.lib") //Winsock Library
#include <string>

#include "common.h"
#include "driverlog.h"

/* Tracking client class. Recieves tracking information and updates tracked devices. */
class tracking_client
{
private:
	struct sockaddr_in server;
	int s, slen, packetNum;
	WSADATA wsa;
	timeval timeout;
	fd_set fds_master;

public:
	tracking_client();
	~tracking_client();
	
	/*Ask server for list of connected devices*/
	std::string GetDeviceList();

	/* Recieve a pose message from socket. Timeout 1 sec */
	PoseMessage_t GetPoseFromUDP();
};

