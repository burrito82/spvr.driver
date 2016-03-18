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

namespace smartvr
{

namespace keys
{

static const char *const Section = "smartvr";
static const char *const SerialNumber_String = "0000-0000-0000-0000";
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
vr::EVRInitError SmartClient::Init(vr::IDriverLog *pDriverLog, vr::IClientDriverHost *pDriverHost, const char *pchUserDriverConfigDir, const char *pchDriverInstallDir)
{
    auto &rContext = Context::GetInstance();
    rContext.GetLogger().AddDriverLog(pDriverLog);
    /*debug << "SmartClient::Init(...)\n"
        << "pchUserDriverConfigDir: " << pchUserDriverConfigDir << "\n"
        << "pchDriverInstallDir: " << pchDriverInstallDir << std::endl;*/
    rContext.GetLogger().Log("Hello World!");
    return vr::EVRInitError::VRInitError_None;
}

/** cleans up the driver right before it is unloaded */
void SmartClient::Cleanup()
{
    Context::GetInstance().GetLogger().Log("SmartClient::Cleanup()");
    Context::Destroy();
}

/** Called when the client needs to inform an application if an HMD is attached that uses
* this driver. This method should be as lightweight as possible and should have no side effects
* such as hooking process functions or leaving resources loaded. Init will not be called before
* this method and Cleanup will not be called after it.
*/
bool SmartClient::BIsHmdPresent(const char *pchUserConfigDir)
{
    if (pchUserConfigDir == nullptr)
    {
        pchUserConfigDir = "nullptr";
    }
    //debug << "SmartClient::BIsHmdPresent(\"" << pchUserConfigDir << "\")" << std::endl;
    return true;
}

/** called when the client inits an HMD to let the client driver know which one is in use */
vr::EVRInitError SmartClient::SetDisplayId(const char *pchDisplayId)
{
    //debug << "SmartClient::SetDisplayId(\"" << pchDisplayId << "\")" << std::endl;
    return vr::EVRInitError::VRInitError_None;
}

/** Returns the stencil mesh information for the current HMD. If this HMD does not have a stencil mesh the vertex data and count will be
* NULL and 0 respectively. This mesh is meant to be rendered into the stencil buffer (or into the depth buffer setting nearz) before rendering
* each eye's view. The pixels covered by this mesh will never be seen by the user after the lens distortion is applied and based on visibility to the panels.
* This will improve perf by letting the GPU early-reject pixels the user will never see before running the pixel shader.
* NOTE: Render this mesh with backface culling disabled since the winding order of the vertices can be different per-HMD or per-eye.
*/
vr::HiddenAreaMesh_t SmartClient::GetHiddenAreaMesh(vr::EVREye eEye)
{
    //debug << "SmartClient::GetHiddenAreaMesh(" << eEye << ")" << std::endl;
    return vr::HiddenAreaMesh_t{nullptr, 0};
}

/** Get the MC image for the current HMD.
* Returns the size in bytes of the buffer required to hold the specified resource. */
uint32_t SmartClient::GetMCImage(uint32_t *pImgWidth, uint32_t *pImgHeight, uint32_t *pChannels, void *pDataBuffer, uint32_t unBufferLen)
{
    //debug << "SmartClient::GetMCImage(...)" << std::endl;
    return 0;
}

} // namespace smartvr
