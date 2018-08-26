#include "hmd.h"

CHeadMountDisplayDevice::CHeadMountDisplayDevice(){
	InitDriverLog(vr::VRDriverLog());

	char buf[1024];
	DriverLog("CHeadMountDisplayDevice Construction.\n");
	//get parameters from settings file.
	m_unObjectId = vr::k_unTrackedDeviceIndexInvalid;
	m_ulPropertyContainer = vr::k_ulInvalidPropertyContainer;
	
	m_flIPD = vr::VRSettings()->GetFloat( k_pch_SteamVR_Section, k_pch_SteamVR_IPD_Float );
	vr::VRSettings()->GetString( k_pch_Sample_Section, k_pch_Sample_SerialNumber_String, buf, sizeof( buf ) );
	m_sSerialNumber = buf;

	vr::VRSettings()->GetString( k_pch_Sample_Section, k_pch_Sample_ModelNumber_String, buf, sizeof( buf ) );
	m_sModelNumber = buf;

	m_nWindowX = vr::VRSettings()->GetInt32( k_pch_Sample_Section, k_pch_Sample_WindowX_Int32 );
	m_nWindowY = vr::VRSettings()->GetInt32( k_pch_Sample_Section, k_pch_Sample_WindowY_Int32 );
	m_nWindowWidth = vr::VRSettings()->GetInt32( k_pch_Sample_Section, k_pch_Sample_WindowWidth_Int32 );
	m_nWindowHeight = vr::VRSettings()->GetInt32( k_pch_Sample_Section, k_pch_Sample_WindowHeight_Int32 );
	m_nRenderWidth = vr::VRSettings()->GetInt32( k_pch_Sample_Section, k_pch_Sample_RenderWidth_Int32 );
	m_nRenderHeight = vr::VRSettings()->GetInt32( k_pch_Sample_Section, k_pch_Sample_RenderHeight_Int32 );
	m_flSecondsFromVsyncToPhotons = vr::VRSettings()->GetFloat( k_pch_Sample_Section, k_pch_Sample_SecondsFromVsyncToPhotons_Float );
	m_flDisplayFrequency = vr::VRSettings()->GetFloat( k_pch_Sample_Section, k_pch_Sample_DisplayFrequency_Float );
    m_bTopCamera = vr::VRSettings()->GetBool( k_pch_Sample_Section, k_pch_Sample_TopCamera );
    m_bCameraHeight = vr::VRSettings()->GetFloat( k_pch_Sample_Section, k_pch_Sample_CameraHeight );

	m_fDistortionK1 = 0.92f;
	m_fDistortionK2 = 0.96f;
	m_fZoomWidth = 0.8f;
	m_fZoomHeight = 1.0f;
	
	//init m_pose struct
	memset( &m_Pose, 0, sizeof( m_Pose ) );
	m_Pose.willDriftInYaw = true;
	m_Pose.shouldApplyHeadModel = false;
	m_Pose.deviceIsConnected = true;
	m_Pose.poseIsValid = true;
	m_Pose.result = vr::ETrackingResult::TrackingResult_Running_OK;
	m_Pose.qRotation = HmdQuaternion_Init( 1, 0, 0, 0 );
	m_Pose.qWorldFromDriverRotation = HmdQuaternion_Init( 1, 0, 0, 0 );
	m_Pose.qDriverFromHeadRotation = HmdQuaternion_Init( 1, 0, 0, 0 );
	m_Pose.poseTimeOffset = -0.016f;
	m_Pose.vecDriverFromHeadTranslation[0] = 0.0f;
	m_Pose.vecDriverFromHeadTranslation[1] = 0.0f;
	m_Pose.vecDriverFromHeadTranslation[2] = 0.0f;
	m_Pose.vecWorldFromDriverTranslation[0] = 0.0f;
	m_Pose.vecWorldFromDriverTranslation[1] = 0.0f;
	m_Pose.vecWorldFromDriverTranslation[2] = 0.0f;

	//create report hmd pose thread
	m_tReportPoseThread = std::thread(&CHeadMountDisplayDevice::ReportPoseThread ,this);
	m_bReportPoseThreadState = true;
}

CHeadMountDisplayDevice::~CHeadMountDisplayDevice(){
}

EVRInitError CHeadMountDisplayDevice::Activate(uint32_t unObjectId){
	DriverLog("HMD: Activate.\n");
	m_unObjectId = unObjectId;
	m_ulPropertyContainer = vr::VRProperties()->TrackedDeviceToPropertyContainer( m_unObjectId );


	vr::VRProperties()->SetStringProperty( m_ulPropertyContainer, Prop_ModelNumber_String, m_sModelNumber.c_str() );
	vr::VRProperties()->SetStringProperty( m_ulPropertyContainer, Prop_RenderModelName_String, m_sModelNumber.c_str() );
	vr::VRProperties()->SetFloatProperty( m_ulPropertyContainer, Prop_UserIpdMeters_Float, m_flIPD );
	vr::VRProperties()->SetFloatProperty( m_ulPropertyContainer, Prop_UserHeadToEyeDepthMeters_Float, 0.f );
	vr::VRProperties()->SetFloatProperty( m_ulPropertyContainer, Prop_DisplayFrequency_Float, m_flDisplayFrequency );
	vr::VRProperties()->SetFloatProperty( m_ulPropertyContainer, Prop_SecondsFromVsyncToPhotons_Float, m_flSecondsFromVsyncToPhotons );
	// return a constant that's not 0 (invalid) or 1 (reserved for Oculus)
	vr::VRProperties()->SetUint64Property( m_ulPropertyContainer, Prop_CurrentUniverseId_Uint64, 2 );

	// avoid "not fullscreen" warnings from vrmonitor
	vr::VRProperties()->SetBoolProperty( m_ulPropertyContainer, Prop_IsOnDesktop_Bool, false );

	// Icons can be configured in code or automatically configured by an external file "drivername\resources\driver.vrresources".
	// Icon properties NOT configured in code (post Activate) are then auto-configured by the optional presence of a driver's "drivername\resources\driver.vrresources".
	// In this manner a driver can configure their icons in a flexible data driven fashion by using an external file.
	//
	// The structure of the driver.vrresources file allows a driver to specialize their icons based on their HW.
	// Keys matching the value in "Prop_ModelNumber_String" are considered first, since the driver may have model specific icons.
	// An absence of a matching "Prop_ModelNumber_String" then considers the ETrackedDeviceClass ("HMD", "Controller", "GenericTracker", "TrackingReference")
	// since the driver may have specialized icons based on those device class names.
	//
	// An absence of either then falls back to the "system.vrresources" where generic device class icons are then supplied.
	//
	// Please refer to "bin\drivers\sample\resources\driver.vrresources" which contains this sample configuration.
	//
	// "Alias" is a reserved key and specifies chaining to another json block.
	//
	// In this sample configuration file (overly complex FOR EXAMPLE PURPOSES ONLY)....
	//
	// "Model-v2.0" chains through the alias to "Model-v1.0" which chains through the alias to "Model-v Defaults".
	//
	// Keys NOT found in "Model-v2.0" would then chase through the "Alias" to be resolved in "Model-v1.0" and either resolve their or continue through the alias.
	// Thus "Prop_NamedIconPathDeviceAlertLow_String" in each model's block represent a specialization specific for that "model".
	// Keys in "Model-v Defaults" are an example of mapping to the same states, and here all map to "Prop_NamedIconPathDeviceOff_String".
	//
	bool bSetupIconUsingExternalResourceFile = false;
	if ( !bSetupIconUsingExternalResourceFile )
	{
		// Setup properties directly in code.
		// Path values are of the form {drivername}\icons\some_icon_filename.png
		vr::VRProperties()->SetStringProperty( m_ulPropertyContainer, vr::Prop_NamedIconPathDeviceOff_String, "{VR284}/icons/headset_sample_status_off.png" );
		vr::VRProperties()->SetStringProperty( m_ulPropertyContainer, vr::Prop_NamedIconPathDeviceSearching_String, "{VR284}/icons/headset_sample_status_searching.gif" );
		vr::VRProperties()->SetStringProperty( m_ulPropertyContainer, vr::Prop_NamedIconPathDeviceSearchingAlert_String, "{VR284}/icons/headset_sample_status_searching_alert.gif" );
		vr::VRProperties()->SetStringProperty( m_ulPropertyContainer, vr::Prop_NamedIconPathDeviceReady_String, "{VR284}/icons/headset_sample_status_ready.png" );
		vr::VRProperties()->SetStringProperty( m_ulPropertyContainer, vr::Prop_NamedIconPathDeviceReadyAlert_String, "{VR284}/icons/headset_sample_status_ready_alert.png" );
		vr::VRProperties()->SetStringProperty( m_ulPropertyContainer, vr::Prop_NamedIconPathDeviceNotReady_String, "{VR284}/icons/headset_sample_status_error.png" );
		vr::VRProperties()->SetStringProperty( m_ulPropertyContainer, vr::Prop_NamedIconPathDeviceStandby_String, "{VR284}/icons/headset_sample_status_standby.png" );
		vr::VRProperties()->SetStringProperty( m_ulPropertyContainer, vr::Prop_NamedIconPathDeviceAlertLow_String, "{VR284}/icons/headset_sample_status_ready_low.png" );
	}
	return VRInitError_None;
}

void CHeadMountDisplayDevice::Deactivate(){
	DriverLog("CHeadMountDisplayDevice::Deactive: enter\n");
	m_unObjectId = vr::k_unTrackedDeviceIndexInvalid;
	m_bReportPoseThreadState = false;
}

void CHeadMountDisplayDevice::EnterStandby(){

}

void *CHeadMountDisplayDevice::GetComponent(const char *pchComponentNameAndVersion){
	if ( !_stricmp( pchComponentNameAndVersion, vr::IVRDisplayComponent_Version ) )
	{
		return (vr::IVRDisplayComponent*)this;
	}

	// override this to add a component to a driver
	return NULL;
}

void CHeadMountDisplayDevice::DebugRequest(const char *pchRequest,char *pchResponseBuffer,uint32_t unResponseBufferSize){

}

DriverPose_t CHeadMountDisplayDevice::GetPose(){
	
	m_Pose.vecPosition[0] = CPose.pos_x;
	m_Pose.vecPosition[1] = CPose.pos_y;
	m_Pose.vecPosition[2] = CPose.pos_z;

	m_Pose.vecVelocity[0] = CPose.vel_x;
	m_Pose.vecVelocity[1] = CPose.vel_y;
	m_Pose.vecVelocity[2] = CPose.vel_z;

	m_Pose.qRotation.w = CPose.quat_w;
	m_Pose.qRotation.x = CPose.quat_x;
	m_Pose.qRotation.y = CPose.quat_y;
	m_Pose.qRotation.z = CPose.quat_z;

	//m_Pose.vecAngularVelocity[0] = CPose.ang_vel_x;
	//m_Pose.vecAngularVelocity[1] = CPose.ang_vel_y;
	//m_Pose.vecAngularVelocity[2] = CPose.ang_vel_z;

	return m_Pose;
}

void CHeadMountDisplayDevice::GetWindowBounds( int32_t *pnX, int32_t *pnY, uint32_t *pnWidth, uint32_t *pnHeight ) 
{
	*pnX = m_nWindowX;
	*pnY = m_nWindowY;
	*pnWidth = m_nWindowWidth;
	*pnHeight = m_nWindowHeight;
}

bool CHeadMountDisplayDevice::IsDisplayOnDesktop() 
{
	return true;
}
bool CHeadMountDisplayDevice::IsDisplayRealDisplay() 
{
	return true;
}

void CHeadMountDisplayDevice::GetRecommendedRenderTargetSize( uint32_t *pnWidth, uint32_t *pnHeight ) 
{
	*pnWidth = m_nRenderWidth;
	*pnHeight = m_nRenderHeight;
}

void CHeadMountDisplayDevice::GetEyeOutputViewport( EVREye eEye, uint32_t *pnX, uint32_t *pnY, uint32_t *pnWidth, uint32_t *pnHeight ) 
{
	*pnY = 0;
	*pnWidth = m_nWindowWidth / 2;
	*pnHeight = m_nWindowHeight;
	
	if ( eEye == Eye_Left )
	{
		*pnX = 0;
	}
	else
	{
		*pnX = m_nWindowWidth / 2;
	}
}

void CHeadMountDisplayDevice::GetProjectionRaw( EVREye eEye, float *pfLeft, float *pfRight, float *pfTop, float *pfBottom ) 
{
	*pfLeft = -1.0;
	*pfRight = 1.0;
	*pfTop = -1.0;
	*pfBottom = 1.0;	
}

DistortionCoordinates_t CHeadMountDisplayDevice::ComputeDistortion( EVREye eEye, float fU, float fV ) 
{
	DistortionCoordinates_t coordinates;
	//add distortion for lens
	float hX;
	float hY;
	double rr;
	double r2;
	double theta;	

	//The formula is derived from this video:https://www.youtube.com/watch?v=B7qrgrrHry0&feature=youtu.be
    //and Distortion correction algorithm for Wikipedia:https://en.wikipedia.org/wiki/Distortion_(optics)#Software_correction
	rr = sqrt((fU - 0.5f)*(fU - 0.5f) + (fV - 0.5f)*(fV - 0.5f));
	r2 = rr * (1 + m_fDistortionK1*(rr*rr) + m_fDistortionK2*(rr*rr*rr*rr));
	theta = atan2(fU-0.5f, fV-0.5f);
	hX = sin(theta)*r2*m_fZoomWidth;
	hY = cos(theta)*r2*m_fZoomHeight;
	
	coordinates.rfBlue[0] = hX + 0.5f;
	coordinates.rfBlue[1] = hY + 0.5f;
	coordinates.rfGreen[0] = hX + 0.5f;
	coordinates.rfGreen[1] = hY + 0.5f;
	coordinates.rfRed[0] = hX + 0.5f;
	coordinates.rfRed[1] = hY + 0.5f;

	return coordinates;
}

std::string CHeadMountDisplayDevice::GetSerialNumber(){
	return m_sSerialNumber;
}

void CHeadMountDisplayDevice::ReportPoseThread(){
	Sleep(1*1000);
	auto retryInterval = std::chrono::milliseconds( REPLY_INTERVAL_MS );
	auto pollDeadline = std::chrono::steady_clock::now();
	
	while(m_bReportPoseThreadState){
		if ( m_unObjectId != vr::k_unTrackedDeviceIndexInvalid )
		{
			CPose = pTrackingClient->UpdatePose("HMD", m_Pose.poseIsValid);
			vr::VRServerDriverHost()->TrackedDevicePoseUpdated( m_unObjectId, GetPose(), sizeof( DriverPose_t ) );
		}

		pollDeadline += retryInterval;
		std::this_thread::sleep_until( pollDeadline );
	}
}

vr::DriverPose_t CHeadMountDisplayDevice::GetMemberPose(){
	return m_Pose;
}

void CHeadMountDisplayDevice::LinkToTrackingServer(tracking_client* pClient) {
	pTrackingClient = pClient;
}