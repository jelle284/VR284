#ifndef VR284_TRACKING_CLIENT_H_
#define VR284_TRACKING_CLIENT_H_
#include "tracking_client.h"


tracking_client::tracking_client()
{
	InitDriverLog(vr::VRDriverLog());
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		DriverLog("WSA failure \n");
		return;
	}

	//create socket
	if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR)
	{
		DriverLog("Socket failure \n");
		return;
	}

	//setup address structure
	memset((char *)&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(4210);
	server.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	slen = sizeof(server);

	// Setup fd_set structure
	FD_ZERO(&fds_master);
	FD_SET(s, &fds_master);

	timeout.tv_sec = 0;
	timeout.tv_usec = 500* 1000;

	packetNum = 0;
}


tracking_client::~tracking_client()
{
	closesocket(s);
	WSACleanup();
}

std::string tracking_client::GetDeviceList()
{
	DriverLog("Asking server for device list.\n");
	char buffer[64];
	sendto(s, "Add Devices", 12, 0, (struct sockaddr*)&server, slen);
	fd_set fds_copy = fds_master;
	int sockcount = select(s+1, &fds_copy, NULL, NULL, &timeout);
	if (sockcount > 0) {
		int size = recvfrom(s, buffer, sizeof(buffer), 0, (struct sockaddr*)&server, &slen);
		buffer[size] = 0;
		DriverLog("Message from server: %s\n", buffer);
	}
	return std::string(buffer);
}

PoseMessage tracking_client::GetPoseFromUDP()
{
	PoseMessage Pose;
	fd_set fds_copy = fds_master;
	int sockcount = select(s + 1, &fds_copy, NULL, NULL, &timeout);
	if (sockcount > 0) {
		// if data is available read
		recvfrom(s, (char*)&Pose, sizeof(Pose), 0, (struct sockaddr*)&server, &slen);
		// TODO: check size
		packetNum++;
	}
	return Pose;
}

#endif