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
	char buffer[64];
	sendto(s, "Add Devices", 12, 0, (struct sockaddr*)&server, slen);
	fd_set fds_copy = fds_master;
	int sockcount = select(s+1, &fds_copy, NULL, NULL, &timeout);
	if (sockcount > 0) {
		recvfrom(s, buffer, sizeof(buffer), 0, (struct sockaddr*)&server, &slen);
		DriverLog("Message from server: %s\n", buffer);
	}
	return std::string(buffer);
}

void tracking_client::UpdateThread()
{
	DriverLog("UDP worker thread entry\n.");
	b_running = true;
	Sleep(1 * 1000);
	while (b_running) {
		PoseMessage pose = { 0 };

		fd_set fds_copy = fds_master;
		int sockcount = select(s + 1, &fds_copy, NULL, NULL, &timeout);
		if (sockcount > 0) {
			// if data is available read
			recvfrom(s, (char*)&pose, sizeof(pose), 0, (struct sockaddr*)&server, &slen);
			packetNum++;
			// update device
			switch (pose.tag) {

			case DEVICE_TAG_HMD:
				if (HMD != vr::k_unTrackedDeviceIndexInvalid)
				{
					vr::VRServerDriverHost()->TrackedDevicePoseUpdated(HMD, GetPoseFromUDP(pose), sizeof(DriverPose_t));
				}
				break;

			case DEVICE_TAG_RIGHT_HAND_CONTROLLER:
				if (RHController != vr::k_unTrackedDeviceIndexInvalid)
				{
					vr::VRServerDriverHost()->TrackedDevicePoseUpdated(RHController, GetPoseFromUDP(pose), sizeof(DriverPose_t));
				}
				//vr::VRDriverInput()->UpdateBooleanComponent(btn_menu, (0x8000 & GetAsyncKeyState('A')) != 0, 0);
				//vr::VRDriverInput()->UpdateBooleanComponent(btn_trigger, Pose.TriggerBtn, 0);
				break;

			case DEVICE_TAG_LEFT_HAND_CONTROLLER:
				if (LHController != vr::k_unTrackedDeviceIndexInvalid)
				{
					vr::VRServerDriverHost()->TrackedDevicePoseUpdated(LHController, GetPoseFromUDP(pose), sizeof(DriverPose_t));
				}
				//vr::VRDriverInput()->UpdateBooleanComponent(btn_menu, (0x8000 & GetAsyncKeyState('A')) != 0, 0);
				//vr::VRDriverInput()->UpdateBooleanComponent(btn_trigger, Pose.TriggerBtn, 0);
				break;

			default:
				break;
			}
		}
	}
	DriverLog("UDP worker thread exit\n.");
}

void tracking_client::Start()
{
	pThread = new std::thread(&tracking_client::UpdateThread, this);
}

void tracking_client::Stop()
{
	b_running = false;
	pThread->join();
}

vr::DriverPose_t tracking_client::GetPoseFromUDP(PoseMessage Pose)
{
	vr::DriverPose_t DriverPose;

	//init m_pose struct
	memset(&DriverPose, 0, sizeof(DriverPose));
	DriverPose.willDriftInYaw = true;
	DriverPose.shouldApplyHeadModel = false;
	DriverPose.deviceIsConnected = true;
	DriverPose.poseIsValid = true;
	DriverPose.result = vr::ETrackingResult::TrackingResult_Running_OK;
	DriverPose.qRotation = HmdQuaternion_Init(1, 0, 0, 0);
	DriverPose.qWorldFromDriverRotation = HmdQuaternion_Init(1, 0, 0, 0);
	DriverPose.qDriverFromHeadRotation = HmdQuaternion_Init(1, 0, 0, 0);
	DriverPose.poseTimeOffset = -0.016f;
	DriverPose.vecDriverFromHeadTranslation[0] = 0.0f;
	DriverPose.vecDriverFromHeadTranslation[1] = 0.0f;
	DriverPose.vecDriverFromHeadTranslation[2] = 0.0f;
	DriverPose.vecWorldFromDriverTranslation[0] = 0.0f;
	DriverPose.vecWorldFromDriverTranslation[1] = 0.0f;
	DriverPose.vecWorldFromDriverTranslation[2] = 0.0f;

	DriverPose.vecPosition[0] = Pose.pos_x;
	DriverPose.vecPosition[1] = Pose.pos_y;
	DriverPose.vecPosition[2] = Pose.pos_z;

	DriverPose.vecVelocity[0] = Pose.vel_x;
	DriverPose.vecVelocity[1] = Pose.vel_y;
	DriverPose.vecVelocity[2] = Pose.vel_z;

	DriverPose.qRotation.w = Pose.quat_w;
	DriverPose.qRotation.x = Pose.quat_x;
	DriverPose.qRotation.y = Pose.quat_y;
	DriverPose.qRotation.z = Pose.quat_z;

	return DriverPose;
}

#endif