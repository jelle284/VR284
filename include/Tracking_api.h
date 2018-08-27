#pragma once
#ifndef VR284_TRACKING_API_H_
#define VR284_TRACKING_API_H_

enum DeviceTag_t {
	DEVICE_TAG_HMD,
	DEVICE_TAG_RIGHT_HAND_CONTROLLER,
	DEVICE_TAG_LEFT_HAND_CONTROLLER
};

/* Message struct for sending and recieving pose information. */
struct PoseMessage {
	float
		quat_w, quat_x, quat_y, quat_z,
		pos_x, pos_y, pos_z,
		vel_x, vel_y, vel_z,
		ang_vel_x, ang_vel_y, ang_vel_z;
	bool TriggerBtn;
	DeviceTag_t tag;
	int packetNum;
};

/* Data from IMU devices and buttons*/
struct DataPacket_t {
	float quat[4], acc[3], gyro[3];
	bool TriggerBtn;
};
#endif