#ifndef VR284_COMMON_H_
#define VR284_COMMON_H_

#define EPSILON 0.000001f

#include <openvr_driver.h>
#include "driverlog.h"
#include <thread>
#include <chrono>
#define _WINSOCKAPI_
#include <windows.h>

#include "tracking_client.h"

//namespace
using namespace std;
using namespace vr;


inline HmdQuaternion_t HmdQuaternion_Init( double w, double x, double y, double z )
{
	HmdQuaternion_t quat;
	quat.w = w;
	quat.x = x;
	quat.y = y;
	quat.z = z;
	return quat;
}

inline void HmdMatrix_SetIdentity( HmdMatrix34_t *pMatrix )
{
	pMatrix->m[0][0] = 1.f;
	pMatrix->m[0][1] = 0.f;
	pMatrix->m[0][2] = 0.f;
	pMatrix->m[0][3] = 0.f;
	pMatrix->m[1][0] = 0.f;
	pMatrix->m[1][1] = 1.f;
	pMatrix->m[1][2] = 0.f;
	pMatrix->m[1][3] = 0.f;
	pMatrix->m[2][0] = 0.f;
	pMatrix->m[2][1] = 0.f;
	pMatrix->m[2][2] = 1.f;
	pMatrix->m[2][3] = 0.f;
}

typedef enum HAND_CONTROLLER{
    LEFT_HAND_CONTROLLER = 0,
	RIGHT_HAND_CONTROLLER,
	HAND_CONTROLLER_COUNT
}EHandController;


#define POSE_REPORT_INTERVAL 16

/** 
	keys for use with the settings API
*/
static const char * const k_pch_Sample_Section = "driver_VR284";
static const char * const k_pch_Sample_SerialNumber_String = "serialNumber";
static const char * const k_pch_Sample_ModelNumber_String = "modelNumber";
static const char * const k_pch_Sample_WindowX_Int32 = "windowX";
static const char * const k_pch_Sample_WindowY_Int32 = "windowY";
static const char * const k_pch_Sample_WindowWidth_Int32 = "windowWidth";
static const char * const k_pch_Sample_WindowHeight_Int32 = "windowHeight";
static const char * const k_pch_Sample_RenderWidth_Int32 = "renderWidth";
static const char * const k_pch_Sample_RenderHeight_Int32 = "renderHeight";
static const char * const k_pch_Sample_SecondsFromVsyncToPhotons_Float = "secondsFromVsyncToPhotons";
static const char * const k_pch_Sample_DisplayFrequency_Float = "displayFrequency";
static const char * const k_pch_Sample_HmdXPositionOffset_Float = "hmd_x_position_offset";
static const char * const k_pch_Sample_HmdYPositionOffset_Float = "hmd_y_position_offset";
static const char * const k_pch_Sample_HmdZPositionOffset_Float = "hmd_z_position_offset";
static const char * const k_pch_Sample_TopCamera = "topCamera";
static const char * const k_pch_Sample_CameraHeight = "cameraHeight";

#endif
