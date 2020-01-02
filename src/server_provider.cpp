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


	m_pHeadMountDisplay = new CHeadMountDisplayDevice();
	vr::VRServerDriverHost()->TrackedDeviceAdded(
		m_pHeadMountDisplay->GetSerialNumber().c_str(), vr::TrackedDeviceClass_HMD, m_pHeadMountDisplay
	);
	/**/
	//create controller device.
	m_pHandController[LEFT_HAND_CONTROLLER] = new CHandControllerDevice("Left Controller", TrackedControllerRole_LeftHand);
	vr::VRServerDriverHost()->TrackedDeviceAdded(
		m_pHandController[0]->GetSerialNumber(), vr::TrackedDeviceClass_Controller, m_pHandController[0]
	);
	m_bControllerState[LEFT_HAND_CONTROLLER] = true;
	
	//create controller device.
	m_pHandController[RIGHT_HAND_CONTROLLER] = new CHandControllerDevice("Right Controller", TrackedControllerRole_RightHand);
	vr::VRServerDriverHost()->TrackedDeviceAdded(
		m_pHandController[1]->GetSerialNumber(), vr::TrackedDeviceClass_Controller, m_pHandController[1]
	);
	m_bControllerState[RIGHT_HAND_CONTROLLER] = true;
	
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
	DriverLog("Pose update thread entry\n.");
	b_running = true;
	Sleep(1 * 1000);

	HANDLE hPipe;
	DWORD dwRead;
	uMessage_t uMes;

	hPipe = CreateNamedPipe(PIPE_NAME,
		PIPE_ACCESS_DUPLEX,
		PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,   // FILE_FLAG_FIRST_PIPE_INSTANCE is not needed but forces CreateNamedPipe(..) to fail if the pipe already exists...
		1,
		1024 * 16,
		1024 * 16,
		PIPE_NOWAIT,
		NULL);

	while (b_running)
	{
		if (ConnectNamedPipe(hPipe, NULL) != FALSE)   // wait for someone to connect to the pipe
		{
			while (ReadFile(hPipe, (char*)&uMes, sizeof(uMes), &dwRead, NULL) != FALSE) {
				switch (uMes.descriptor) {
				case e_poseupdate:
					switch (uMes.data.Pose.tag) {
					case DEVICE_TAG_HMD:
						m_pHeadMountDisplay->ReportPoseButton(uMes.data.Pose);
						break;
					case DEVICE_TAG_RIGHT_HAND_CONTROLLER:
						m_pHandController[RIGHT_HAND_CONTROLLER]->ReportPoseButton(uMes.data.Pose);
						break;
					case DEVICE_TAG_LEFT_HAND_CONTROLLER:
						m_pHandController[LEFT_HAND_CONTROLLER]->ReportPoseButton(uMes.data.Pose);
						break;
					}
					break;
				case e_distort:
					m_pHeadMountDisplay->ChangeDistortion(uMes.data.Distort);
					break;
				}
			}
		}
		DisconnectNamedPipe(hPipe);
	}

	DriverLog("Pose update thread exit\n.");
}