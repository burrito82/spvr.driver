/*
 * Copyright (c) 2016
 *  Somebody
 */
#include "HmdDriver.h"

#include "Logger.h"

#include <cstring>
#include <fstream>
#include <memory>
#include <string>

namespace smartvr
{

HmdDriver::HmdDriver(vr::IServerDriverHost *pServerDriverHost, Logger *pDriverLog):
    m_pServerDriverHost{pServerDriverHost},
    m_pDriverLog{pDriverLog},
    m_uObjectId{vr::k_unTrackedDeviceIndexInvalid},
    m_sSerialNumber("SVR0815"),
    m_sModelNumber("smartvr.driver"),
    m_fIPD{0.0635f},
    m_fDisplayFrequency{90.0f},
    m_fSecondsFromVsyncToPhotons{0.1f},
    m_iWindowX{},
    m_iWindowY{},
    m_iWindowWidth{1280},
    m_iWindowHeight{720},
    m_iRenderWidth{640},
    m_iRenderHeight{360}
{
}

vr::EVRInitError HmdDriver::Activate(std::uint32_t uObjectId)
{
    if (m_pDriverLog)
    {
        m_pDriverLog->Log(std::string{"HmdDriver::Activate("} + std::to_string(uObjectId) + ")\n");
    }
    m_uObjectId = uObjectId;
    return vr::VRInitError_None;
}

void HmdDriver::Deactivate()
{
    if (m_pDriverLog)
    {
        m_pDriverLog->Log("HmdDriver::Deactivate()\n");
    }
    m_uObjectId = vr::k_unTrackedDeviceIndexInvalid;
}

void HmdDriver::PowerOff()
{
    Deactivate();
}

void *HmdDriver::GetComponent(char const *pchComponentNameAndVersion)
{
    if (!pchComponentNameAndVersion)
    {
        return nullptr;
    }
    if (m_pDriverLog)
    {
        m_pDriverLog->Log(std::string{"HmdDriver::GetComponent("} + pchComponentNameAndVersion + ")\n");
    }
    if (std::string{pchComponentNameAndVersion} == vr::IVRDisplayComponent_Version)
    {
        return static_cast<vr::IVRDisplayComponent *>(this);
    }

    return nullptr;
}

void HmdDriver::DebugRequest(char const *pchRequest, char *pchResponseBuffer, std::uint32_t uResponseBufferSize)
{
    if (uResponseBufferSize >= 1)
    {
        pchResponseBuffer[0] = 0;
    }
}

vr::DriverPose_t HmdDriver::GetPose()
{
    vr::DriverPose_t pose{};
    pose.poseIsValid = true;
    pose.result = vr::TrackingResult_Running_OK;
    pose.deviceIsConnected = true;

    pose.qWorldFromDriverRotation = {1.0, 0.0, 0.0, 0.0};
    pose.qDriverFromHeadRotation = {1.0, 0.0, 0.0, 0.0};

    static int iCounter = 0;
    if (false)
    {
        double w = 1.0 - (static_cast<double>(iCounter) / 100.0);
        double x = (static_cast<double>(iCounter) / 100.0);
        double d = 1.0 / (w + x);
        w *= d;
        x *= d;
        pose.qDriverFromHeadRotation.w = w;
        pose.qDriverFromHeadRotation.x = x;
        ++iCounter;
    }

    if (m_pDriverLog)
    {
        m_pDriverLog->Log("HmdDriver::GetPose()\n");
    }

    return pose;
}

bool HmdDriver::GetBoolTrackedDeviceProperty(vr::ETrackedDeviceProperty prop, vr::ETrackedPropertyError *pError)
{
    *pError = vr::TrackedProp_Success;
    bool bRetVal = false;

    switch (prop)
    {
    case vr::Prop_IsOnDesktop_Bool:                 /* FALLTHROUGH */
        // avoid "not fullscreen" warnings from vrmonitor
    case vr::Prop_ContainsProximitySensor_Bool:     /* FALLTHROUGH */
    case vr::Prop_BlockServerShutdown_Bool:         /* FALLTHROUGH */
    case vr::Prop_HasCamera_Bool:
    {
        break;
    }
    default:
    {
        *pError = vr::TrackedProp_ValueNotProvidedByDevice;
    }
    }

    if (m_pDriverLog)
    {
        m_pDriverLog->Log(std::string{"HmdDriver::GetBoolTrackedDeviceProperty("} +std::to_string(prop) + ", ...) => false\n");
    }

    return bRetVal;
}

float HmdDriver::GetFloatTrackedDeviceProperty(vr::ETrackedDeviceProperty prop, vr::ETrackedPropertyError *pError)
{
    vr::ETrackedPropertyError error = vr::TrackedProp_Success;
    float fRetVal = 0.0f;

    switch (prop)
    {
    case vr::Prop_UserIpdMeters_Float:
    {
        fRetVal = m_fIPD;
        break;
    }
    case vr::Prop_UserHeadToEyeDepthMeters_Float:
    {
        fRetVal = 0.0f;
        break;
    }
    case vr::Prop_DisplayFrequency_Float:
    {
        fRetVal = m_fDisplayFrequency;
        break;
    }
    case vr::Prop_SecondsFromVsyncToPhotons_Float:
    {
        fRetVal = m_fSecondsFromVsyncToPhotons;
        break;
    }
    default:
        error = vr::TrackedProp_ValueNotProvidedByDevice;
    }

    if (pError)
    {
        *pError = error;
    }

    if (m_pDriverLog)
    {
        m_pDriverLog->Log(std::string{"HmdDriver::GetFloatTrackedDeviceProperty("} +std::to_string(prop) + ", ...) => " + std::to_string(fRetVal) + "\n");
    }

    return fRetVal;
}

std::int32_t HmdDriver::GetInt32TrackedDeviceProperty(vr::ETrackedDeviceProperty prop, vr::ETrackedPropertyError *pError)
{
    std::int32_t iRetVal = 0;
    vr::ETrackedPropertyError error = vr::TrackedProp_Success;

    switch (prop)
    {
    case vr::Prop_DeviceClass_Int32:
    {
        iRetVal = vr::TrackedDeviceClass_HMD;
        break;
    }
    case vr::Prop_EdidVendorID_Int32:
    {
        iRetVal = 1234;
        break;
    }
    default:
    {
        error = vr::TrackedProp_UnknownProperty;
    }
    }

    if (pError)
    {
        *pError = error;
    }

    if (m_pDriverLog)
    {
        m_pDriverLog->Log(std::string{"HmdDriver::GetInt32TrackedDeviceProperty("} + std::to_string(prop) + ", ...) => " + std::to_string(iRetVal) + "\n");
    }

    return iRetVal;
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
    default:
    {
        *pError = vr::TrackedProp_ValueNotProvidedByDevice;
    }
    }

    if (m_pDriverLog)
    {
        m_pDriverLog->Log("HmdDriver::GetUint64TrackedDeviceProperty(...)\n");
    }

    return 0;
}

vr::HmdMatrix34_t HmdDriver::GetMatrix34TrackedDeviceProperty(vr::ETrackedDeviceProperty prop, vr::ETrackedPropertyError *pError)
{
    if (m_pDriverLog)
    {
        m_pDriverLog->Log(std::string{"HmdDriver::GetMatrix34TrackedDeviceProperty("} + std::to_string(prop) + ")\n");
    }
    //*pError = vr::TrackedProp_ValueNotProvidedByDevice;
    *pError = vr::TrackedProp_Success;
    vr::HmdMatrix34_t matIdentity
    {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f
    };
    static int iCounter = 0;
    {
        auto w = 1.0f - (static_cast<float>(iCounter) / 100.0f);
        auto x = (static_cast<float>(iCounter) / 100.0f);
        auto d = 1.0f / (w + x);
        w *= d;
        x *= d;
        matIdentity.m[0][0] = w;
        matIdentity.m[0][2] = x;
        matIdentity.m[2][0] = w;
        matIdentity.m[2][2] = x;
        ++iCounter;
    }
    return matIdentity;
}

std::uint32_t HmdDriver::GetStringTrackedDeviceProperty(vr::ETrackedDeviceProperty prop, char *pchValue, std::uint32_t uBufferSize, vr::ETrackedPropertyError *pError)
{
    vr::ETrackedPropertyError eError{};
    if (!pError)
    {
        pError = &eError;
    }
    std::string const strValue = GetStringTrackedDeviceProperty(prop, *pError);
    if (*pError == vr::TrackedProp_Success)
    {
        if (strValue.size() + 1 > uBufferSize)
        {
            *pError = vr::TrackedProp_BufferTooSmall;
        }
        else
        {
            strcpy_s(pchValue, uBufferSize, strValue.c_str());
        }
        return static_cast<std::uint32_t>(strValue.size()) + 1;
    }
    return 0;
}

std::string HmdDriver::GetStringTrackedDeviceProperty(vr::ETrackedDeviceProperty prop, vr::ETrackedPropertyError &rError)
{
    std::string strRetVal{};
    rError = vr::TrackedProp_Success;

    switch (prop)
    {
    case vr::Prop_ModelNumber_String:
    {
        strRetVal = m_sModelNumber;
        break;
    }
    case vr::Prop_SerialNumber_String:
    {
        strRetVal = m_sSerialNumber;
        break;
    }
    default:
    {
        rError = vr::TrackedProp_ValueNotProvidedByDevice;
    }
    }

    if (m_pDriverLog)
    {
        m_pDriverLog->Log(std::string{"HmdDriver::GetStringTrackedDeviceProperty("} +std::to_string(prop) + ", ...) => \"" + strRetVal + "\"\n");
    }

    return strRetVal;
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
    else
    {
        if (m_pDriverLog)
        {
            m_pDriverLog->Log("HmdDriver::RunFrame(), but m_uObjectId is invalid!\n");
        }
    }
}

void HmdDriver::GetWindowBounds(std::int32_t *piX, std::int32_t *piY, std::uint32_t *puWidth, std::uint32_t *puHeight)
{
    if (m_pDriverLog)
    {
        m_pDriverLog->Log("HmdDriver::GetWindowBounds()\n");
    }
    *piX = m_iWindowX;
    *piY = m_iWindowY;
    *puWidth = m_iWindowWidth;
    *puHeight = m_iWindowHeight;
}

bool HmdDriver::IsDisplayOnDesktop()
{
    if (m_pDriverLog)
    {
        m_pDriverLog->Log("HmdDriver::IsDisplayOnDesktop()\n");
    }
    return true;
}

bool HmdDriver::IsDisplayRealDisplay()
{
    if (m_pDriverLog)
    {
        m_pDriverLog->Log("HmdDriver::IsDisplayOnDesktop()\n");
    }
    return false;
}

void HmdDriver::GetRecommendedRenderTargetSize(std::uint32_t *puWidth, std::uint32_t *puHeight)
{
    if (m_pDriverLog)
    {
        m_pDriverLog->Log("HmdDriver::GetRecommendedRenderTargetSize(...)\n");
    }
    *puWidth = m_iRenderWidth;
    *puHeight = m_iRenderHeight;
}

void HmdDriver::GetEyeOutputViewport(vr::EVREye eEye, std::uint32_t *puX, std::uint32_t *puY, std::uint32_t *puWidth, std::uint32_t *puHeight)
{
    if (m_pDriverLog)
    {
        m_pDriverLog->Log("HmdDriver::GetEyeOutputViewport(...)\n");
    }
    *puY = 0;
    *puWidth = m_iWindowWidth / 2;
    *puHeight = m_iWindowHeight;

    if (eEye == vr::Eye_Left)
    {
        *puX = 0;
    }
    else
    {
        *puX = m_iWindowWidth / 2;
    }
}

void HmdDriver::GetProjectionRaw(vr::EVREye eEye, float *pfLeft, float *pfRight, float *pfTop, float *pfBottom)
{
    if (m_pDriverLog)
    {
        m_pDriverLog->Log("HmdDriver::GetProjectionRaw(...)\n");
    }
    *pfLeft = -1.0;
    *pfRight = 1.0;
    *pfTop = -1.0;
    *pfBottom = 1.0;
}

vr::DistortionCoordinates_t HmdDriver::ComputeDistortion(vr::EVREye eEye, float fU, float fV)
{
    if (m_pDriverLog)
    {
        m_pDriverLog->Log(std::string{"HmdDriver::ComputeDistortion("} + std::to_string(eEye) + ", "
                          + std::to_string(fU) + ", " + std::to_string(fV) + ")\n");
    }
    vr::DistortionCoordinates_t oDistortion{};
    oDistortion.rfBlue[0] = fU;
    oDistortion.rfBlue[1] = fV;
    oDistortion.rfGreen[0] = fU;
    oDistortion.rfGreen[1] = fV;
    oDistortion.rfRed[0] = fU;
    oDistortion.rfRed[1] = fV;
    return oDistortion;
}

/*void HmdDriver::CreateSwapTextureSet(std::uint32_t unPid, std::uint32_t unFormat, std::uint32_t unWidth, std::uint32_t unHeight, void *(*pSharedTextureHandles)[2])
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
}*/

} // namespace smartvr
