// 
#ifndef VR284_HAND_CONTROLLER_H_
#define VR284_HAND_CONTROLLER_H_
#include "common.h"
#include "driverlog.h"
#include <string>
#include <thread>

typedef void (vr::IVRServerDriverHost::*ButtonUpdate)(uint32_t unWhichDevice, vr::EVRButtonId eButtonId, double eventTimeOffset);
/**
	CHandControllerDevice implement  ITrackedDeviceServerDriver and IVRControllerComponent.
*/
class CHandControllerDevice : public ITrackedDeviceServerDriver {
public:
	/**
		constructor.
		@param[in] serial_number handle controller's serial number.
		@param[in] controller_role which hand controller(left or right).
	*/
	CHandControllerDevice(string serial_number,ETrackedControllerRole controller_role);
	/**
		destructor
	*/
	~CHandControllerDevice();
	
	//implement ITrackedDeviceServerDriver
	/** This is called before an HMD is returned to the application. It will always be
	* called before any display or tracking methods. Memory and processor use by the
	* ITrackedDeviceServerDriver object should be kept to a minimum until it is activated.
	* The pose listener is guaranteed to be valid until Deactivate is called, but
	* should not be used after that point. */	
	EVRInitError Activate( uint32_t unObjectId ) override;
	/** This is called when The VR system is switching from this Hmd being the active display
	* to another Hmd being the active display. The driver should clean whatever memory
	* and thread use it can when it is deactivated */	
	void Deactivate() override;
	/** Handles a request from the system to put this device into standby mode. What that means is defined per-device. */
	void EnterStandby() override;
	/** Requests a component interface of the driver for device-specific functionality. The driver should return NULL
	* if the requested interface or version is not supported. */	
	void *GetComponent( const char *pchComponentNameAndVersion ) override;
	/** A VR Client has made this debug request of the driver. The set of valid requests is entirely
	* up to the driver and the client to figure out, as is the format of the response. Responses that
	* exceed the length of the supplied buffer should be truncated and null terminated */	
	void DebugRequest( const char *pchRequest, char *pchResponseBuffer, uint32_t unResponseBufferSize ) override;
	/**
		return track device pose.
	*/		
	DriverPose_t GetPose() override;	
	
	//implement IVRControllerComponent	

	/** Gets the current state of a controller. */
	//VRControllerState_t GetControllerState( ) override;
	/** Returns a uint64 property. If the property is not available this function will return 0. */
	//bool TriggerHapticPulse( uint32_t unAxisId, uint16_t usPulseDurationMicroseconds ) override;
	
	//custom define function
	const char *GetSerialNumber();
	/**
		@return return unique object id which set by Activate function.
	*/
	const uint32_t GetUniqueObjectId();

private:
	string m_sSerialNumber;					//< serial number
	uint32_t m_nUniqueObjectId;				//< unique object id which set by active function
	vr::DriverPose_t m_Pose;					//< hand controller's pose
	EHandController m_eHandController;		//< indicate left or right hand
	vr::VRControllerState_t m_ControllerState;//< controller state
	vr::PropertyContainerHandle_t m_PropertyContainerHandle;//< set/get property
	char m_cControllerRole; 				//< 'L' or 'R' controller.
	std::chrono::milliseconds m_nReportPoseInterval;			//< report controller's pose interval.

	float m_fHmdXPositionOffset;	//< six dof tracking module X position offset.
	float m_fHmdYPositionOffset;	//< six dof tracking module Y position offset.
	float m_fHmdZPositionOffset;	//< six dof tracking module Z position offset.		
    bool m_bTopCamera;
    double m_bCameraHeight;

	// vr284
	vr::VRInputComponentHandle_t btn_menu, btn_trigger;
};

#endif