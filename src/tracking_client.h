#pragma once
#include <WinSock2.h>
#pragma comment(lib,"ws2_32.lib") //Winsock Library
#include <string>
#include <mutex>
#include "driverlog.h"


struct ClientPoseMessage {
	float
		quat_w, quat_x, quat_y, quat_z,
		pos_x, pos_y, pos_z,
		vel_x, vel_y, vel_z,
		ang_vel_x, ang_vel_y, ang_vel_z;
	bool TriggerBtn;
	int packetNum;
};

struct ClientRequest {
	char message[32];
	int packetNum;
};

class tracking_client
{
private:
	struct sockaddr_in server;
	int s, slen, packetNum;
	WSADATA wsa;
	timeval timeout;
	fd_set fds_master;
	std::mutex mtx;
public:
	tracking_client();
	~tracking_client();
	
	/*Ask server for list of connected devices*/
	std::string GetDeviceList(); 

	/*Update pose for a specific device*/
	ClientPoseMessage UpdatePose(std::string tag, bool &Success); 
};

