/*
 * Copyright (c) 2016
 *  Somebody
 */
#include "HmdDriver.h"

#include "Context.h"
#include "ControlInterface.h"
#include "Logger.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/quaternion.hpp"

#include <chrono>
#include <cstring>
#include <fstream>
#include <memory>
#include <string>

namespace spvr
{

HmdDriver::HmdDriver(vr::IServerDriverHost *pServerDriverHost, Logger *pDriverLog):
    m_pServerDriverHost{pServerDriverHost},
    m_pDriverLog{pDriverLog},
    m_uObjectId{vr::k_unTrackedDeviceIndexInvalid},
    m_sSerialNumber("SPVR0815"),
    m_sModelNumber("SmartPhoneVR Driver 0x0000"),
    m_fIPD{0.0635f},
    m_fDisplayFrequency{90.0f},
    m_fSecondsFromVsyncToPhotons{0.0111111f},
    m_iWindowX{},
    m_iWindowY{},
    m_iWindowWidth{1280},
    m_iWindowHeight{720},
    m_iRenderWidth{640},
    m_iRenderHeight{360},
    m_oPoseUpdateThread{}
{
    m_fDisplayFrequency = 60.0f;
    auto pSettings = pServerDriverHost->GetSettings(vr::IVRSettings_Version);
    m_fIPD = pSettings->GetFloat(vr::k_pch_SteamVR_Section, vr::k_pch_SteamVR_IPD_Float, 0.063f);
    /*m_iWindowWidth = 2160;
    m_iWindowHeight = 1200;*/
    m_iRenderWidth = 1512;
    m_iRenderHeight = 1680;
}

char const *HmdDriver::GetSerialNumber() const
{
    return m_sSerialNumber.c_str();
}

char const *HmdDriver::GetModelNumber() const
{
    return m_sModelNumber.c_str();
}

vr::EVRInitError HmdDriver::Activate(std::uint32_t uObjectId)
{
    if (m_pDriverLog)
    {
        m_pDriverLog->Log(std::string{"HmdDriver::Activate("} + std::to_string(uObjectId) + ")\n");
    }
    m_uObjectId = uObjectId;
    /*m_oPoseUpdateThread = std::thread{
        [this]()
        {
            volatile auto uObjectId = m_uObjectId;
            vr::DriverPose_t pose;
            while (uObjectId != vr::k_unTrackedDeviceIndexInvalid)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds{1});
                pose = GetPose();
                m_pServerDriverHost->TrackedDevicePoseUpdated(uObjectId, pose);
                uObjectId = m_uObjectId;
            }
        }
    };*/
    return vr::VRInitError_None;
}

void HmdDriver::Deactivate()
{
    if (m_pDriverLog)
    {
        m_pDriverLog->Log("HmdDriver::Deactivate()\n");
    }
    m_uObjectId = vr::k_unTrackedDeviceIndexInvalid;
    if (m_oPoseUpdateThread.joinable())
    {
        m_oPoseUpdateThread.join();
    }
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
    pose.poseTimeOffset = 0.15;
    pose.poseIsValid = true;
    pose.result = vr::TrackingResult_Running_OK;
    pose.deviceIsConnected = true;
    pose.willDriftInYaw = true;

    pose.qWorldFromDriverRotation = vr::HmdQuaternion_t{1.0, 0.0, 0.0, 0.0};
    pose.qDriverFromHeadRotation = vr::HmdQuaternion_t{1.0, 0.0, 0.0, 0.0};

    glm::quat const qRotation = Context::GetInstance().GetControlInterface().GetRotation();

    pose.qRotation.w = qRotation.w;
    pose.qRotation.x = qRotation.x;
    pose.qRotation.y = qRotation.y;
    pose.qRotation.z = qRotation.z;

    if (m_pDriverLog)
    {
        m_pDriverLog->Log("HmdDriver::GetPose()\n");
    }

    return pose;
}

bool HmdDriver::GetBoolTrackedDeviceProperty(vr::ETrackedDeviceProperty prop, vr::ETrackedPropertyError *pError)
{
    vr::ETrackedPropertyError error = vr::TrackedProp_Success;
    bool bRetVal = false;

    switch (prop)
    {
    case vr::Prop_IsOnDesktop_Bool:
    {
        //bRetVal = IsDisplayOnDesktop();
        // avoid "not fullscreen" warnings from vrmonitor
        bRetVal = false;
        break;
    }
    default:
    {
        error = vr::TrackedProp_ValueNotProvidedByDevice;
    }
    }

    if (pError)
    {
        *pError = error;
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
    default:
    {
        error = vr::TrackedProp_ValueNotProvidedByDevice;
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
        return 3;
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
    *pError = vr::TrackedProp_ValueNotProvidedByDevice;
    vr::HmdMatrix34_t matIdentity{};

    matIdentity.m[0][0] = 1.f;
    matIdentity.m[0][1] = 0.f;
    matIdentity.m[0][2] = 0.f;
    matIdentity.m[0][3] = 0.f;

    matIdentity.m[1][0] = 0.f;
    matIdentity.m[1][1] = 1.f;
    matIdentity.m[1][2] = 0.f;
    matIdentity.m[1][3] = 0.f;

    matIdentity.m[2][0] = 0.f;
    matIdentity.m[2][1] = 0.f;
    matIdentity.m[2][2] = 1.f;
    matIdentity.m[2][3] = 0.f;

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
    case vr::Prop_RenderModelName_String:
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
    m_pDriverLog->Log("HmdDriver::RunFrame()\n");
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
    *puWidth = static_cast<std::uint32_t>(m_iWindowWidth);
    *puHeight = static_cast<std::uint32_t>(m_iWindowHeight);
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
        m_pDriverLog->Log("HmdDriver::IsDisplayRealDisplay()\n");
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

void HmdDriver::CreateSwapTextureSet(std::uint32_t unPid, std::uint32_t unFormat, std::uint32_t unWidth, std::uint32_t unHeight, void *(*pSharedTextureHandles)[2])
{
    if (m_pDriverLog)
    {
        m_pDriverLog->Log("HmdDriver::CreateSwapTextureSet(...)\n");
    }
}

void HmdDriver::DestroySwapTextureSet(void *pSharedTextureHandle)
{
    if (m_pDriverLog)
    {
        m_pDriverLog->Log("HmdDriver::DestroySwapTextureSet(...)\n");
    }
}

void HmdDriver::DestroyAllSwapTextureSets(std::uint32_t unPid)
{
    if (m_pDriverLog)
    {
        m_pDriverLog->Log("HmdDriver::DestroyAllSwapTextureSets(...)\n");
    }
}

void HmdDriver::SubmitLayer(void *pSharedTextureHandles[2], vr::VRTextureBounds_t const (&bounds)[2], vr::HmdMatrix34_t const *pPose)
{
    if (m_pDriverLog)
    {
        m_pDriverLog->Log("HmdDriver::SubmitLayer(...)\n");
    }
}

void HmdDriver::Present(void *hSyncTexture)
{
    if (m_pDriverLog)
    {
        m_pDriverLog->Log("HmdDriver::Present(...)\n");
    }
}

} // namespace spvr
