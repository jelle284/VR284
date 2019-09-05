#pragma once

#define PIPE_NAME_W L"\\\\.\\pipe\\Pipe"
#define PIPE_NAME "\\\\.\\pipe\\Pipe"

enum DeviceTag_t {
	DEVICE_TAG_HMD,
	DEVICE_TAG_RIGHT_HAND_CONTROLLER,
	DEVICE_TAG_LEFT_HAND_CONTROLLER,
	DEVICE_COUNT
};

enum ButtonTag_t {
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

/* Data from IMU devices */
struct DataPacket_t {
	float acc_gyro_mag[9];
};

struct imu_packet_t {
	int16_t ax, ay, az;
	int16_t gx, gy, gz;
	int16_t mx, my, mz;
};

struct adc_packet_t {
	int16_t a0, a1, a2, a3;
};

/* Button information */
struct ButtonState_t {
	float axis[ANALOG_COUNT];
	bool ButtonState[BUTTON_COUNT];
};

/* Pose information*/
struct Pose_t {
	float
		pos[3],
		vel[3],
		ang_vel[3],
		q[4];
};

/* Message struct for driver communication. */
struct PoseMessage_t {
	Pose_t pose;
	DeviceTag_t tag;
	ButtonState_t buttons;
};

struct HmdQuaternionf_t
{
  float w, x, y, z;
};

struct VRInitMessage {
	bool deviceStatus[DEVICE_COUNT];
	float IPD, k1, k2, zW, zH;
};

struct DistortionSettings_t {
	float IPD, k1, k2, zW, zH;
};

// union
enum udesc_t {
	e_distort, 
	e_poseupdate
};

union Udata {
	DistortionSettings_t Distort;
	PoseMessage_t Pose;
};

struct uMessage_t {
	udesc_t descriptor;
	Udata data;
};