#pragma once
#include <WinSock2.h>
#pragma comment(lib,"ws2_32.lib") //Winsock Library
#include <string>

#include "common.h"
#include "driverlog.h"


class tracking_client
{
private:
	struct sockaddr_in server;
	int s, slen, packetNum;
	WSADATA wsa;
	timeval timeout;
	fd_set fds_master;

	/* Worker thread */
	void UpdateThread();
	bool b_running; // thread is running
	std::thread *pThread;

	/* Convert pose to openVR api. */
	vr::DriverPose_t GetPoseFromUDP(PoseMessage Pose);
public:

	/* ID of tracked devices */
	vr::TrackedDeviceIndex_t HMD, RHController, LHController;

	tracking_client();
	~tracking_client();
	
	/*Ask server for list of connected devices*/
	std::string GetDeviceList();

	void Start();
	void Stop();
};

