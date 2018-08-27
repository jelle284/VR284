#include "hand_controller.h"

CHandControllerDevice::CHandControllerDevice(string serial_number,ETrackedControllerRole controller_role){
	InitDriverLog(vr::VRDriverLog());

	m_sSerialNumber = serial_number;

	//init m_pose struct
	memset( &m_Pose, 0, sizeof( m_Pose ) );
	m_Pose.willDriftInYaw = true;
	m_Pose.shouldApplyHeadModel = false;
	m_Pose.result = ETrackingResult::TrackingResult_Running_OK;
	m_Pose.qRotation = HmdQuaternion_Init( 1, 0, 0, 0 );
	m_Pose.qWorldFromDriverRotation = HmdQuaternion_Init( 1, 0, 0, 0 );
	m_Pose.qDriverFromHeadRotation = HmdQuaternion_Init( 1,0, 0, 0);
	m_Pose.poseTimeOffset = -0.016f;	
	m_Pose.deviceIsConnected = true;
	m_Pose.poseIsValid = true;
	m_Pose.vecDriverFromHeadTranslation[0] = 0.0f;
	m_Pose.vecDriverFromHeadTranslation[1] = 0.0f;
	m_Pose.vecDriverFromHeadTranslation[2] = 0.0f;
	m_Pose.vecWorldFromDriverTranslation[0] = 0.0f;
	m_Pose.vecWorldFromDriverTranslation[1] = 0.0f;
	m_Pose.vecWorldFromDriverTranslation[2] = 0.0f;

	switch (controller_role) {
		case TrackedControllerRole_LeftHand:
			DriverLog("Left hand constructor\n");
			m_eHandController = LEFT_HAND_CONTROLLER;
			m_cControllerRole = 'L';
			break;
		case TrackedControllerRole_RightHand:
			DriverLog("Right hand constructor\n");
			m_eHandController = RIGHT_HAND_CONTROLLER;
			m_cControllerRole = 'R';
			break;
	}
	m_fHmdXPositionOffset = vr::VRSettings()->GetFloat( k_pch_Sample_Section, k_pch_Sample_HmdXPositionOffset_Float );
	m_fHmdYPositionOffset = vr::VRSettings()->GetFloat( k_pch_Sample_Section, k_pch_Sample_HmdYPositionOffset_Float );
	m_fHmdZPositionOffset = vr::VRSettings()->GetFloat( k_pch_Sample_Section, k_pch_Sample_HmdZPositionOffset_Float );
    m_bTopCamera = vr::VRSettings()->GetBool( k_pch_Sample_Section, k_pch_Sample_TopCamera );
    m_bCameraHeight = vr::VRSettings()->GetFloat( k_pch_Sample_Section, k_pch_Sample_CameraHeight );
	m_nReportPoseInterval = std::chrono::milliseconds( POSE_REPORT_INTERVAL );
	
	//init m_ControllerState,m_PropertyContainerHandle
	memset( &m_ControllerState, 0, sizeof( m_ControllerState ) );
	m_PropertyContainerHandle = vr::k_ulInvalidPropertyContainer;
	
}

CHandControllerDevice::~CHandControllerDevice(){
}

EVRInitError CHandControllerDevice::Activate( uint32_t unObjectId ){
	DriverLog("Hand Controller: Activate.\n");
	uint64_t u64_parameter;
	m_nUniqueObjectId = unObjectId;
	m_PropertyContainerHandle = vr::VRProperties()->TrackedDeviceToPropertyContainer(m_nUniqueObjectId);

	//set controller's display to htc.
	vr::VRProperties()->SetStringProperty(m_PropertyContainerHandle, vr::Prop_ModelNumber_String, "ViveMV");
	vr::VRProperties()->SetStringProperty(m_PropertyContainerHandle, vr::Prop_ManufacturerName_String, "HTC");
	vr::VRProperties()->SetStringProperty(m_PropertyContainerHandle, vr::Prop_RenderModelName_String, "vr_controller_vive_1_5");
	//set general properties
	vr::VRProperties()->SetStringProperty( m_PropertyContainerHandle, Prop_TrackingSystemName_String, "VR284 Controller" );
	vr::VRProperties()->SetInt32Property( m_PropertyContainerHandle, Prop_DeviceClass_Int32, TrackedDeviceClass_Controller);
	vr::VRProperties()->SetStringProperty(m_PropertyContainerHandle, Prop_SerialNumber_String, m_sSerialNumber.c_str());

	// Set icons,you can select yourself icon.
	switch (m_eHandController)
	{
	case LEFT_HAND_CONTROLLER:
		vr::VRProperties()->SetStringProperty(m_PropertyContainerHandle, vr::Prop_NamedIconPathDeviceOff_String, "{VR284}/icons/left_hand_controller_icon.png");
		vr::VRProperties()->SetStringProperty(m_PropertyContainerHandle, vr::Prop_NamedIconPathDeviceSearching_String, "{VR284}/icons/left_hand_controller_icon.png");
		vr::VRProperties()->SetStringProperty(m_PropertyContainerHandle, vr::Prop_NamedIconPathDeviceSearchingAlert_String, "{VR284}/icons/left_hand_controller_icon.png");
		vr::VRProperties()->SetStringProperty(m_PropertyContainerHandle, vr::Prop_NamedIconPathDeviceReady_String, "{VR284}/icons/left_hand_controller_icon.png");
		vr::VRProperties()->SetStringProperty(m_PropertyContainerHandle, vr::Prop_NamedIconPathDeviceReadyAlert_String, "{VR284}/icons/left_hand_controller_icon.png");
		vr::VRProperties()->SetStringProperty(m_PropertyContainerHandle, vr::Prop_NamedIconPathDeviceNotReady_String, "{VR284}/icons/left_hand_controller_icon.png");
		vr::VRProperties()->SetStringProperty(m_PropertyContainerHandle, vr::Prop_NamedIconPathDeviceStandby_String, "{VR284}/icons/left_hand_controller_icon.png");
		vr::VRProperties()->SetStringProperty(m_PropertyContainerHandle, vr::Prop_NamedIconPathDeviceAlertLow_String, "{VR284}/icons/left_hand_controller_icon.png");
		break;
	case RIGHT_HAND_CONTROLLER:
		vr::VRProperties()->SetStringProperty(m_PropertyContainerHandle, vr::Prop_NamedIconPathDeviceOff_String, "{VR284}/icons/right_hand_controller_icon.png");
		vr::VRProperties()->SetStringProperty(m_PropertyContainerHandle, vr::Prop_NamedIconPathDeviceSearching_String, "{VR284}/icons/right_hand_controller_icon.png");
		vr::VRProperties()->SetStringProperty(m_PropertyContainerHandle, vr::Prop_NamedIconPathDeviceSearchingAlert_String, "{VR284}/icons/right_hand_controller_icon.png");
		vr::VRProperties()->SetStringProperty(m_PropertyContainerHandle, vr::Prop_NamedIconPathDeviceReady_String, "{VR284}/icons/right_hand_controller_icon.png");
		vr::VRProperties()->SetStringProperty(m_PropertyContainerHandle, vr::Prop_NamedIconPathDeviceReadyAlert_String, "{VR284}/icons/right_hand_controller_icon.png");
		vr::VRProperties()->SetStringProperty(m_PropertyContainerHandle, vr::Prop_NamedIconPathDeviceNotReady_String, "{VR284}/icons/right_hand_controller_icon.png");
		vr::VRProperties()->SetStringProperty(m_PropertyContainerHandle, vr::Prop_NamedIconPathDeviceStandby_String, "{VR284}/icons/right_hand_controller_icon.png");
		vr::VRProperties()->SetStringProperty(m_PropertyContainerHandle, vr::Prop_NamedIconPathDeviceAlertLow_String, "{VR284}/icons/right_hand_controller_icon.png");
		break;
	default:
		break;
	}
	
	// use htc vive buttons
	vr::VRProperties()->SetStringProperty(m_PropertyContainerHandle, Prop_InputProfilePath_String, "{htc}/input/vive_controller_profile.json");
	vr::VRDriverInput()->CreateBooleanComponent(m_PropertyContainerHandle, "/input/application_menu/click", &btn_menu);
	vr::VRDriverInput()->CreateBooleanComponent(m_PropertyContainerHandle, "/input/trigger/click", &btn_trigger);

	return vr::VRInitError_None;
}

void CHandControllerDevice::Deactivate(){
	DriverLog("Hand Controller: Deactivate.\n");
	m_nUniqueObjectId = k_unTrackedDeviceIndexInvalid;
}
void CHandControllerDevice::EnterStandby(){

}
void *CHandControllerDevice::GetComponent( const char *pchComponentNameAndVersion ){
	void *p = NULL;
	return p;
}
void CHandControllerDevice::DebugRequest( const char *pchRequest, char *pchResponseBuffer, uint32_t unResponseBufferSize ){

}

DriverPose_t CHandControllerDevice::GetPose(){
	//get controller pose.
	return m_Pose;
}

const char *CHandControllerDevice::GetSerialNumber(){
	return m_sSerialNumber.c_str();
}

const uint32_t CHandControllerDevice::GetUniqueObjectId(){
    return m_nUniqueObjectId;
}