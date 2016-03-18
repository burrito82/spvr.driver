/*
 * Copyright (c) 2016
 *  Somebody
 */
#include "HmdDriver.h"

#include <cstring>
#include <fstream>
#include <memory>
#include <string>

namespace smartvr
{

namespace keys
{

static const char *const Section = "smartvr";
static const char *const SerialNumber_String = "serialNumber";
static const char *const ModelNumber_String = "modelNumber";
static const char *const WindowX_Int32 = "windowX";
static const char *const WindowY_Int32 = "windowY";
static const char *const WindowWidth_Int32 = "windowWidth";
static const char *const WindowHeight_Int32 = "windowHeight";
static const char *const RenderWidth_Int32 = "renderWidth";
static const char *const RenderHeight_Int32 = "renderHeight";
static const char *const SecondsFromVsyncToPhotons_Float = "secondsFromVsyncToPhotons";
static const char *const DisplayFrequency_Float = "displayFrequency";

} // namespace keys

HmdDriver::HmdDriver(vr::IServerDriverHost *pServerDriverHost) :
    m_pServerDriverHost{pServerDriverHost}
{

}

vr::EVRInitError HmdDriver::Activate(std::uint32_t unObjectId)
{
    return vr::VRInitError_None;
}

void HmdDriver::Deactivate()
{
}

void HmdDriver::PowerOff()
{
}

void *HmdDriver::GetComponent(char const *pchComponentNameAndVersion)
{
    if (std::string{pchComponentNameAndVersion} == vr::IVRDisplayComponent_Version)
    {
        return static_cast<vr::IVRDisplayComponent *>(this);
    }

    return nullptr;
}

void HmdDriver::DebugRequest(char const *pchRequest, char *pchResponseBuffer, std::uint32_t unResponseBufferSize)
{
    if (unResponseBufferSize >= 1)
    {
        pchResponseBuffer[0] = 0;
    }
}

vr::DriverPose_t HmdDriver::GetPose()
{
    vr::DriverPose_t pose = {0};
    pose.poseIsValid = true;
    pose.result = vr::TrackingResult_Running_OK;
    pose.deviceIsConnected = true;

    pose.qWorldFromDriverRotation = {1.0, 0.0, 0.0, 0.0};
    pose.qDriverFromHeadRotation = {1.0, 0.0, 0.0, 0.0};

    return pose;
}

bool HmdDriver::GetBoolTrackedDeviceProperty(vr::ETrackedDeviceProperty prop, vr::ETrackedPropertyError *pError)
{
    *pError = vr::TrackedProp_Success;

    switch (prop)
    {
    case vr::Prop_IsOnDesktop_Bool:
    {
        // avoid "not fullscreen" warnings from vrmonitor
        return false;
    }
    }

    *pError = vr::TrackedProp_ValueNotProvidedByDevice;
    return false;
}

float HmdDriver::GetFloatTrackedDeviceProperty(vr::ETrackedDeviceProperty prop, vr::ETrackedPropertyError *pError)
{
    vr::ETrackedPropertyError error = vr::TrackedProp_ValueNotProvidedByDevice;
    float fRetVal = 0;

    switch (prop)
    {
    case vr::Prop_UserIpdMeters_Float:
    {
        fRetVal = m_fIPD;
        error = vr::TrackedProp_Success;
        break;
    }
    case vr::Prop_UserHeadToEyeDepthMeters_Float:
    {
        error = vr::TrackedProp_Success;
        fRetVal = 0.f;
        break;
    }
    case vr::Prop_DisplayFrequency_Float:
    {
        error = vr::TrackedProp_Success;
        fRetVal = m_fDisplayFrequency;
        break;
    }
    case vr::Prop_SecondsFromVsyncToPhotons_Float:
    {
        error = vr::TrackedProp_Success;
        fRetVal = m_fSecondsFromVsyncToPhotons;
        break;
    }
    }

    if (pError)
    {
        *pError = error;
    }

    return fRetVal;
}

std::int32_t HmdDriver::GetInt32TrackedDeviceProperty(vr::ETrackedDeviceProperty prop, vr::ETrackedPropertyError *pError)
{
    int32_t nRetVal = 0;
    vr::ETrackedPropertyError error = vr::TrackedProp_UnknownProperty;

    switch (prop)
    {
    case vr::Prop_DeviceClass_Int32:
    {
        nRetVal = vr::TrackedDeviceClass_HMD;
        error = vr::TrackedProp_Success;
        break;
    }
    }

    if (pError)
    {
        *pError = error;
    }

    return nRetVal;
}

std::uint64_t HmdDriver::GetUint64TrackedDeviceProperty(vr::ETrackedDeviceProperty prop, vr::ETrackedPropertyError *pError)
{
    *pError = vr::TrackedProp_Success;

    switch (prop)
    {
    case vr::Prop_CurrentUniverseId_Uint64:
    {
        // return a constant that's not 0 (invalid) or 1 (reserved for Oculus)
        return 2;
    }
    }

    *pError = vr::TrackedProp_ValueNotProvidedByDevice;
    return 0;
}

vr::HmdMatrix34_t HmdDriver::GetMatrix34TrackedDeviceProperty(vr::ETrackedDeviceProperty prop, vr::ETrackedPropertyError *pError)
{
    *pError = vr::TrackedProp_ValueNotProvidedByDevice;
    vr::HmdMatrix34_t const matIdentity
    {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f
    };
    return matIdentity;
}

std::uint32_t HmdDriver::GetStringTrackedDeviceProperty(vr::ETrackedDeviceProperty prop, char *pchValue, std::uint32_t unBufferSize, vr::ETrackedPropertyError *pError)
{
    std::string sValue = GetStringTrackedDeviceProperty(prop, pError);
    if (*pError == vr::TrackedProp_Success)
    {
        if (sValue.size() + 1 > unBufferSize)
        {
            *pError = vr::TrackedProp_BufferTooSmall;
        }
        else
        {
            strcpy_s(pchValue, unBufferSize, sValue.c_str());
        }
        return (uint32_t)sValue.size() + 1;
    }
    return 0;
}

std::string HmdDriver::GetStringTrackedDeviceProperty(vr::ETrackedDeviceProperty prop, vr::ETrackedPropertyError *pError)
{
    *pError = vr::TrackedProp_ValueNotProvidedByDevice;
    std::string sRetVal;

    switch (prop)
    {
    case vr::Prop_ModelNumber_String:
    sRetVal = m_sModelNumber;
    *pError = vr::TrackedProp_Success;
    break;

    case vr::Prop_SerialNumber_String:
    sRetVal = m_sSerialNumber;
    *pError = vr::TrackedProp_Success;
    break;
    }

    return sRetVal;
}

void HmdDriver::RunFrame()
{
    // In a real driver, this should happen from some pose tracking thread.
    // The RunFrame interval is unspecified and can be very irregular if some other
    // driver blocks it for some periodic task.
    if (m_uObjectId != vr::k_unTrackedDeviceIndexInvalid)
    {
        m_pServerDriverHost->TrackedDevicePoseUpdated(m_uObjectId, GetPose());
    }
}

void HmdDriver::GetWindowBounds(std::int32_t *pnX, std::int32_t *pnY, std::uint32_t *pnWidth, std::uint32_t *pnHeight)
{
}

bool HmdDriver::IsDisplayOnDesktop()
{
    return true;
}

bool HmdDriver::IsDisplayRealDisplay()
{
    return false;
}

void HmdDriver::GetRecommendedRenderTargetSize(std::uint32_t *pnWidth, std::uint32_t *pnHeight)
{
}

void HmdDriver::GetEyeOutputViewport(vr::EVREye eEye, std::uint32_t *pnX, std::uint32_t *pnY, std::uint32_t *pnWidth, std::uint32_t *pnHeight)
{
}

void HmdDriver::GetProjectionRaw(vr::EVREye eEye, float *pfLeft, float *pfRight, float *pfTop, float *pfBottom)
{
}

vr::DistortionCoordinates_t HmdDriver::ComputeDistortion(vr::EVREye eEye, float fU, float fV)
{
    vr::DistortionCoordinates_t oDistortion{};
    oDistortion.rfBlue[0] = fU;
    oDistortion.rfBlue[1] = fV;
    oDistortion.rfGreen[0] = fU;
    oDistortion.rfGreen[1] = fV;
    oDistortion.rfRed[0] = fU;
    oDistortion.rfRed[1] = fV;
    return oDistortion;
}

void HmdDriver::CreateSwapTextureSet(std::uint32_t unPid, std::uint32_t unFormat, std::uint32_t unWidth, std::uint32_t unHeight, void *(*pSharedTextureHandles)[2])
{
}

void HmdDriver::DestroySwapTextureSet(void *pSharedTextureHandle)
{
}

void HmdDriver::DestroyAllSwapTextureSets(std::uint32_t unPid)
{
}

void HmdDriver::SubmitLayer(void *pSharedTextureHandles[2], vr::VRTextureBounds_t const (&bounds)[2], vr::HmdMatrix34_t const *pPose)
{
}

void HmdDriver::Present(void *hSyncTexture)
{
}

} // namespace smartvr
