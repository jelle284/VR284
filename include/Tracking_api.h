#pragma once
#ifndef VR284_TRACKING_API_H_
#define VR284_TRACKING_API_H_

enum DeviceTag_t {
	DEVICE_TAG_HMD,
	DEVICE_TAG_RIGHT_HAND_CONTROLLER,
	DEVICE_TAG_LEFT_HAND_CONTROLLER,
	DEVICE_COUNT
};

enum ButtonTag_t {
	BUTTON_TAG_TRIGGER,
	BUTTON_TAG_SMENU,
	BUTTON_TAG_AMENU,
	BUTTON_TAG_GRIP,
	BUTTON_TAG_TPAD,
	BUTTON_COUNT
};

enum AnalogTag_t {
	ANALOG_TAG_X,
	ANALOG_TAG_Y,
	ANALOG_TAG_TRIGGER,
	ANALOG_COUNT
};

/* Message struct for sending and recieving pose information. */
struct PoseMessage_t {
	float
		quat_w, quat_x, quat_y, quat_z,
		pos_x, pos_y, pos_z,
		vel_x, vel_y, vel_z,
		ang_vel_x, ang_vel_y, ang_vel_z,
		axis[ANALOG_COUNT];
	bool ButtonState[BUTTON_COUNT];
	DeviceTag_t tag;
	int packetNum;
};

/* Data from IMU devices and buttons*/
struct DataPacket_t {
	float quat[4], acc[3], gyro[3];
	bool TriggerBtn; // TODO: multiple buttons - flash mcu
};

#endif