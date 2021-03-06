#ifndef VR284_HMD_H_
#define VR284_HMD_H_
#include "common.h"
#include <thread>

/**
	CHeadMountDisplayDevice implement  ITrackedDeviceServerDriver and IVRDisplayComponent.
*/
class CHeadMountDisplayDevice : public vr::ITrackedDeviceServerDriver, public vr::IVRDisplayComponent {
public:
	/**
		constructor.
	*/
	CHeadMountDisplayDevice();

	/**
		destructor.
	*/
	~CHeadMountDisplayDevice();

	//implement ITrackedDeviceServerDriver

	/** This is called before an HMD is returned to the application. It will always be
	* called before any display or tracking methods. Memory and processor use by the
	* ITrackedDeviceServerDriver object should be kept to a minimum until it is activated.
	* The pose listener is guaranteed to be valid until Deactivate is called, but
	* should not be used after that point. */
	virtual EVRInitError Activate(uint32_t unObjectId) override;
	/** This is called when The VR system is switching from this Hmd being the active display
	* to another Hmd being the active display. The driver should clean whatever memory
	* and thread use it can when it is deactivated */
	virtual void Deactivate() override;
	/** Handles a request from the system to put this device into standby mode. What that means is defined per-device. */
	virtual void EnterStandby() override;
	/** Requests a component interface of the driver for device-specific functionality. The driver should return NULL
	* if the requested interface or version is not supported. */
	virtual void *GetComponent(const char *pchComponentNameAndVersion) override;
	/** A VR Client has made this debug request of the driver. The set of valid requests is entirely
	* up to the driver and the client to figure out, as is the format of the response. Responses that
	* exceed the length of the supplied buffer should be truncated and null terminated */
	virtual void DebugRequest(const char *pchRequest, char *pchResponseBuffer, uint32_t unResponseBufferSize) override;
	/**
		return track device pose.
	*/
	virtual DriverPose_t GetPose() override;

	//implement IVRDisplayComponent

	/** Size and position that the window needs to be on the VR display. */
	virtual void GetWindowBounds(int32_t *pnX, int32_t *pnY, uint32_t *pnWidth, uint32_t *pnHeight) override;
	/** Returns true if the display is extending the desktop. */
	virtual bool IsDisplayOnDesktop() override;
	/** Returns true if the display is real and not a fictional display. */
	virtual bool IsDisplayRealDisplay() override;
	/** Suggested size for the intermediate render target that the distortion pulls from. */
	virtual void GetRecommendedRenderTargetSize(uint32_t *pnWidth, uint32_t *pnHeight) override;
	/** Gets the viewport in the frame buffer to draw the output of the distortion into */
	virtual void GetEyeOutputViewport(EVREye eEye, uint32_t *pnX, uint32_t *pnY, uint32_t *pnWidth, uint32_t *pnHeight) override;
	/** The components necessary to build your own projection matrix in case your
		* application is doing something fancy like infinite Z */
	virtual void GetProjectionRaw(EVREye eEye, float *pfLeft, float *pfRight, float *pfTop, float *pfBottom) override;
	/** Returns the result of the distortion function for the specified eye and input UVs. UVs go from 0,0 in
		* the upper left of that eye's viewport and 1,1 in the lower right of that eye's viewport. */
	virtual DistortionCoordinates_t ComputeDistortion(EVREye eEye, float fU, float fV) override;

	//other functions
	std::string GetSerialNumber();
	const uint32_t GetUniqueObjectId();

	/* Updates pose */
	void ReportPoseButton(PoseMessage_t &Pose);

	void ChangeDistortion(DistortionSettings_t distort);

private:
	vr::TrackedDeviceIndex_t m_unObjectId;//< unique id ,set by vrserver through Activate function
	vr::PropertyContainerHandle_t m_ulPropertyContainer;//< use to set/get property

	std::string m_sSerialNumber;		//< device's serial number.
	std::string m_sModelNumber;			//< device's model number.

	int32_t m_nWindowX;					//< window x position
	int32_t m_nWindowY;					//< window y position
	int32_t m_nWindowWidth;				//< window's width resolution
	int32_t m_nWindowHeight;			//< windows height resolution
	int32_t m_nRenderWidth;				//< render width range 
	int32_t m_nRenderHeight;			//< render height range
	float m_flSecondsFromVsyncToPhotons;//< vsync
	float m_flDisplayFrequency;			//< display frequency
	float m_flIPD;						//< IPD

	/* Barrel distortion parameters. */
	std::array<float, 3> m_fDistortionK1, m_fDistortionK2; // RGB component
	float m_fZoomWidth;					//<width zoom scale 
	float m_fZoomHeight;				//<height zoom scale

	vr::DriverPose_t m_Pose;			//HMD pose.

};
#endif
