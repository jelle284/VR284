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

	// Initiate tracking client and get list of devices connected
	tracking_client* pTrackingClient = new tracking_client();
	std::string devices = pTrackingClient->GetDeviceList();

	if (devices.find("HMD") != devices.npos) {
		//create HMD device
		m_pHeadMountDisplay = new CHeadMountDisplayDevice();
		m_pHeadMountDisplay->LinkToTrackingServer(pTrackingClient);
		vr::VRServerDriverHost()->TrackedDeviceAdded(m_pHeadMountDisplay->GetSerialNumber().c_str(), vr::TrackedDeviceClass_HMD, m_pHeadMountDisplay);
	}

	if (devices.find("Left Hand Controller") != devices.npos) {
		//create controller device.
		m_pHandController[LEFT_HAND_CONTROLLER] = new CHandControllerDevice("Left Controller", TrackedControllerRole_LeftHand);
		m_pHandController[LEFT_HAND_CONTROLLER]->LinkToTrackingServer(pTrackingClient);
		vr::VRServerDriverHost()->TrackedDeviceAdded(m_pHandController[0]->GetSerialNumber(), vr::TrackedDeviceClass_Controller, m_pHandController[0]);
	}

	if (devices.find("Right Hand Controller") != devices.npos) {
		//create controller device.
		m_pHandController[RIGHT_HAND_CONTROLLER] = new CHandControllerDevice("Right Controller", TrackedControllerRole_RightHand);
		m_pHandController[RIGHT_HAND_CONTROLLER]->LinkToTrackingServer(pTrackingClient);
		vr::VRServerDriverHost()->TrackedDeviceAdded(m_pHandController[1]->GetSerialNumber(), vr::TrackedDeviceClass_Controller, m_pHandController[1]);
	}
	return VRInitError_None;
}

void CServerProvider::Cleanup(){
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

