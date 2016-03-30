/*
 * Copyright (c) 2016
 *  Somebody
 */
#include "ClientProvider.h"

#include "Context.h"
#include "ControlInterface.h"
#include "Logger.h"

#include <cstring>
#include <fstream>
#include <memory>
#include <string>

#include "openvr_driver.h"

namespace spvr
{

SmartClient::SmartClient():
m_pLogger{}
{

}

/** initializes the driver. This will be called before any other methods are called,
* except BIsHmdPresent(). BIsHmdPresent is called outside of the Init/Cleanup pair.
* If Init returns anything other than VRInitError_None the driver DLL will be unloaded.
*
* pDriverHost will never be NULL, and will always be a pointer to a IClientDriverHost interface
*
* pchUserDriverConfigDir - The absolute path of the directory where the driver should store user
*	config files.
* pchDriverInstallDir - The absolute path of the root directory for the driver.
*/
vr::EVRInitError SmartClient::Init(vr::IDriverLog *pDriverLog, vr::IClientDriverHost *pDriverHost, char const *pchUserDriverConfigDir, char const *pchDriverInstallDir)
{
    Context *pContext = nullptr;
    try
    {
        pContext = &Context::GetInstance();
    }
    catch (...)
    {
        return vr::VRInitError_Init_HmdNotFound;
    }
    m_pLogger = &pContext->GetLogger();
    m_pLogger->AddDriverLog(pDriverLog);

    if (!pchUserDriverConfigDir)
    {
        pchUserDriverConfigDir = "nullptr";
    }
    if (!pchDriverInstallDir)
    {
        pchDriverInstallDir = "nullptr";
    }

    m_pLogger->Log(std::string{"SmartClient::Init(\""} +pchUserDriverConfigDir + "\", \"" + pchDriverInstallDir + "\");\n");
    vr::IVRSettings *pSettings = pDriverHost->GetSettings(vr::IVRSettings_Version);
    return vr::EVRInitError::VRInitError_None;
}

/** cleans up the driver right before it is unloaded */
void SmartClient::Cleanup()
{
    /*if (m_pLogger)
    {
        m_pLogger->Log("SmartClient::Cleanup()\n");
    }*/
    //Context::Destroy();
}

/** Called when the client needs to inform an application if an HMD is attached that uses
* this driver. This method should be as lightweight as possible and should have no side effects
* such as hooking process functions or leaving resources loaded. Init will not be called before
* this method and Cleanup will not be called after it.
*/
bool SmartClient::BIsHmdPresent(char const *pchUserConfigDir)
{
    if (pchUserConfigDir == nullptr)
    {
        pchUserConfigDir = "nullptr";
    }
    if (m_pLogger)
    {
        m_pLogger->Log(std::string{"SmartClient::BIsHmdPresent(\""} + pchUserConfigDir + "\")\n");
    }
    return false;
}

/** called when the client inits an HMD to let the client driver know which one is in use */
vr::EVRInitError SmartClient::SetDisplayId(char const *pchDisplayId)
{
    if (!pchDisplayId)
    {
        pchDisplayId = "nullptr";
    }
    m_pLogger->Log(std::string{"SmartClient::SetDisplayId(\""} + pchDisplayId + "\")\n");
    return vr::VRInitError_None;
}

/** Returns the stencil mesh information for the current HMD. If this HMD does not have a stencil mesh the vertex data and count will be
* NULL and 0 respectively. This mesh is meant to be rendered into the stencil buffer (or into the depth buffer setting nearz) before rendering
* each eye's view. The pixels covered by this mesh will never be seen by the user after the lens distortion is applied and based on visibility to the panels.
* This will improve perf by letting the GPU early-reject pixels the user will never see before running the pixel shader.
* NOTE: Render this mesh with backface culling disabled since the winding order of the vertices can be different per-HMD or per-eye.
*/
vr::HiddenAreaMesh_t SmartClient::GetHiddenAreaMesh(vr::EVREye eEye)
{
    m_pLogger->Log(std::string{"SmartClient::GetHiddenAreaMesh("} + std::to_string(eEye) + ")\n");
    return vr::HiddenAreaMesh_t{nullptr, 0u};
}

/** Get the MC image for the current HMD.
* Returns the size in bytes of the buffer required to hold the specified resource. */
uint32_t SmartClient::GetMCImage(std::uint32_t *pImgWidth, std::uint32_t *pImgHeight, std::uint32_t *pChannels, void *pDataBuffer, std::uint32_t unBufferLen)
{
    m_pLogger->Log("SmartClient::GetMCImage(...)\n");
    return 0;
}

} // namespace spvr
