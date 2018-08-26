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

	// Setup timeval variable
	timeout.tv_sec = 0;
	timeout.tv_usec = 50*1000;

	// Setup fd_set structure
	FD_ZERO(&fds_master);
	FD_SET(s, &fds_master);

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
	ClientRequest request = { 0 };
	strcpy_s(request.message, "Add Devices");
	request.packetNum = -1;
	char buffer[64];
	sendto(s, (char*)&request, sizeof(request), 0, (struct sockaddr*)&server, slen);
	fd_set fds_copy = fds_master;
	int sockcount = select(s+1, &fds_copy, NULL, NULL, &timeout);
	if (sockcount > 0) {
		recvfrom(s, buffer, sizeof(buffer), 0, (struct sockaddr*)&server, &slen);
		DriverLog("Message from server: %s\n", buffer);
	}
	return std::string(buffer);
}

ClientPoseMessage tracking_client::UpdatePose(std::string tag, bool &Success)
{
	mtx.lock();
	ClientPoseMessage pose = { 0 };
	ClientRequest request = { 0 };
	strcpy_s(request.message, tag.c_str());
	request.packetNum = packetNum;
	fd_set fds_copy = fds_master;
	sendto(s, (char*)&request, sizeof(request), 0, (struct sockaddr*)&server, slen);
	int sockcount = select(s + 1, &fds_copy, NULL, NULL, &timeout);
	if (sockcount > 0) {
		recvfrom(s, (char*)&pose, sizeof(pose), 0, (struct sockaddr*)&server, &slen);
	}
	if (pose.packetNum == packetNum) {
		Success = true;
		packetNum++;
		if (packetNum > 999) packetNum = 0;
	}
	else {
		Success = false;
		DriverLog("Packet failure. Packet num: %i, recieved %i.\n", packetNum, pose.packetNum);
		packetNum = 0;
	}
	mtx.unlock();
	return pose;
}
