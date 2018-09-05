#include "server_provider.h"

CServerProvider::CServerProvider(){
	m_bControllerState[LEFT_HAND_CONTROLLER] = false;
	m_bControllerState[RIGHT_HAND_CONTROLLER] = false;
	m_pThis = this;
}

CServerProvider::~CServerProvider(){

}

CServerProvider *CServerProvider::m_pThis = NULL;
vr::EVRInitError CServerProvider::Init( IVRDriverContext *pDriverContext ){
	//init modules contains:VRServerDriverHost,VRSettings,VRProperties,VRDriverLog.
	VR_INIT_SERVER_DRIVER_CONTEXT( pDriverContext );

	pTracker = new tracking_client();
	// Get list of devices connected
	std::string devices = pTracker->GetDeviceList();

	if (devices.find("HMD") != devices.npos) {
		//create HMD device
		m_pHeadMountDisplay = new CHeadMountDisplayDevice();
		vr::VRServerDriverHost()->TrackedDeviceAdded(m_pHeadMountDisplay->GetSerialNumber().c_str(), vr::TrackedDeviceClass_HMD, m_pHeadMountDisplay);
	}

	if (devices.find("Left Hand Controller") != devices.npos) {
		//create controller device.
		m_pHandController[LEFT_HAND_CONTROLLER] = new CHandControllerDevice("Left Controller", TrackedControllerRole_LeftHand);
		vr::VRServerDriverHost()->TrackedDeviceAdded(m_pHandController[0]->GetSerialNumber(), vr::TrackedDeviceClass_Controller, m_pHandController[0]);
		m_bControllerState[LEFT_HAND_CONTROLLER] = true;
	}

	if (devices.find("Right Hand Controller") != devices.npos) {
		//create controller device.
		m_pHandController[RIGHT_HAND_CONTROLLER] = new CHandControllerDevice("Right Controller", TrackedControllerRole_RightHand);
		vr::VRServerDriverHost()->TrackedDeviceAdded(m_pHandController[1]->GetSerialNumber(), vr::TrackedDeviceClass_Controller, m_pHandController[1]);
		m_bControllerState[RIGHT_HAND_CONTROLLER] = true;
	}

	/* Create pose update thread */
	pThread = new std::thread(&CServerProvider::PoseUpdateThread, this);

	return VRInitError_None;
}

void CServerProvider::Cleanup(){
	b_running = false;
	pThread->join();
	CleanupDriverLog();
}

const char * const *CServerProvider::GetInterfaceVersions(){
	return k_InterfaceVersions;
}

void CServerProvider::RunFrame(){
}

bool CServerProvider::ShouldBlockStandbyMode(){
	return false;
}

void CServerProvider::EnterStandby(){

}

void CServerProvider::LeaveStandby(){

}

void CServerProvider::PoseUpdateThread()
{
	bool distortion_on = false;
	DriverLog("Pose update thread entry\n.");
	b_running = true;
	Sleep(1 * 1000);
	while (b_running) {
		PoseMessage_t pose = pTracker->GetPoseFromUDP();
			switch (pose.tag) {

			case DEVICE_TAG_HMD:
				m_pHeadMountDisplay->ReportPoseButton(pose);
				break;

			case DEVICE_TAG_RIGHT_HAND_CONTROLLER:
				m_pHandController[RIGHT_HAND_CONTROLLER]->ReportPoseButton(pose);
				break;

			case DEVICE_TAG_LEFT_HAND_CONTROLLER:
				m_pHandController[LEFT_HAND_CONTROLLER]->ReportPoseButton(pose);
				break;

			default:
				break;
			}

			// Toggle distortion on/off
			if ((0x01 & GetAsyncKeyState('T')) != 0) {
				distortion_on = !distortion_on;
				DriverLog((distortion_on ? "Distortion on.\n" : "Distortion off.\n"));
			}

			if (distortion_on) {
				m_pHeadMountDisplay->ChangeDistortion();
			}
		}

	DriverLog("Pose update thread exit\n.");
}