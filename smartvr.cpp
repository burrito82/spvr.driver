/*
 * Copyright (c) 2016
 *  Somebody
 */
#include "smartvr.h"

#include <cstring>
#include <fstream>
#include <memory>
#include <string>

#include "openvr_driver.h"

char const copyright[] =
"Copyright (c) 2016\n\tSomebody.  All rights reserved.\n\n";

namespace smartvr
{

static std::ofstream debug{"debug.log"};

class SmartClient final : public vr::IClientTrackedDeviceProvider
{
public:
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
    virtual vr::EVRInitError Init(vr::IDriverLog *pDriverLog, vr::IClientDriverHost *pDriverHost, const char *pchUserDriverConfigDir, const char *pchDriverInstallDir) override
    {
        debug << "SmartClient::Init(...)\n"
            << "pchUserDriverConfigDir: " << pchUserDriverConfigDir << "\n"
            << "pchDriverInstallDir: " << pchDriverInstallDir << std::endl;
        pDriverLog->Log("Hello World!");
        return vr::EVRInitError::VRInitError_None;
    }

    /** cleans up the driver right before it is unloaded */
    virtual void Cleanup() override
    {
        debug << "SmartClient::Cleanup()" << std::endl;
    }

    /** Called when the client needs to inform an application if an HMD is attached that uses
    * this driver. This method should be as lightweight as possible and should have no side effects
    * such as hooking process functions or leaving resources loaded. Init will not be called before
    * this method and Cleanup will not be called after it.
    */
    virtual bool BIsHmdPresent(const char *pchUserConfigDir) override
    {
        if (pchUserConfigDir == nullptr)
        {
            pchUserConfigDir = "nullptr";
        }
        debug << "SmartClient::BIsHmdPresent(\"" << pchUserConfigDir << "\")" << std::endl;
        return true;
    }

    /** called when the client inits an HMD to let the client driver know which one is in use */
    virtual vr::EVRInitError SetDisplayId(const char *pchDisplayId) override
    {
        debug << "SmartClient::SetDisplayId(\"" << pchDisplayId << "\")" << std::endl;
        return vr::EVRInitError::VRInitError_None;
    }

    /** Returns the stencil mesh information for the current HMD. If this HMD does not have a stencil mesh the vertex data and count will be
    * NULL and 0 respectively. This mesh is meant to be rendered into the stencil buffer (or into the depth buffer setting nearz) before rendering
    * each eye's view. The pixels covered by this mesh will never be seen by the user after the lens distortion is applied and based on visibility to the panels.
    * This will improve perf by letting the GPU early-reject pixels the user will never see before running the pixel shader.
    * NOTE: Render this mesh with backface culling disabled since the winding order of the vertices can be different per-HMD or per-eye.
    */
    virtual vr::HiddenAreaMesh_t GetHiddenAreaMesh(vr::EVREye eEye) override
    {
        debug << "SmartClient::GetHiddenAreaMesh(" << eEye << ")" << std::endl;
        return vr::HiddenAreaMesh_t{nullptr, 0};
    }

    /** Get the MC image for the current HMD.
    * Returns the size in bytes of the buffer required to hold the specified resource. */
    virtual uint32_t GetMCImage(uint32_t *pImgWidth, uint32_t *pImgHeight, uint32_t *pChannels, void *pDataBuffer, uint32_t unBufferLen) override
    {
        debug << "SmartClient::GetMCImage(...)" << std::endl;
        return *pImgWidth * *pImgHeight * *pChannels;
    }
private:
};

class SmartServer final : public vr::IServerTrackedDeviceProvider
{
public:
    /** initializes the driver. This will be called before any other methods are called.
    * If Init returns anything other than VRInitError_None the driver DLL will be unloaded.
    *
    * pDriverHost will never be NULL, and will always be a pointer to a IServerDriverHost interface
    *
    * pchUserDriverConfigDir - The absolute path of the directory where the driver should store user
    *	config files.
    * pchDriverInstallDir - The absolute path of the root directory for the driver.
    */
    virtual vr::EVRInitError Init(vr::IDriverLog *pDriverLog, vr::IServerDriverHost *pDriverHost, const char *pchUserDriverConfigDir, const char *pchDriverInstallDir) override
    {
        return vr::EVRInitError::VRInitError_None;
    }

    /** cleans up the driver right before it is unloaded */
    virtual void Cleanup() override
    {

    }

    /** returns the number of HMDs that this driver manages that are physically connected. */
    virtual uint32_t GetTrackedDeviceCount() override
    {
        return 1;
    }

    /** returns a single HMD */
    virtual vr::ITrackedDeviceServerDriver *GetTrackedDeviceDriver(uint32_t unWhich, const char *pchInterfaceVersion) override
    {
        return nullptr;
    }

    /** returns a single HMD by ID */
    virtual vr::ITrackedDeviceServerDriver* FindTrackedDeviceDriver(const char *pchId, const char *pchInterfaceVersion) override
    {
        return nullptr;
    }

    /** Allows the driver do to some work in the main loop of the server. */
    virtual void RunFrame() override
    {

    }


    // ------------  Power State Functions ----------------------- //

    /** Returns true if the driver wants to block Standby mode. */
    virtual bool ShouldBlockStandbyMode() override
    {
        return false;
    }

    /** Called when the system is entering Standby mode. The driver should switch itself into whatever sort of low-power
    * state it has. */
    virtual void EnterStandby() override
    {

    }

    /** Called when the system is leaving Standby mode. The driver should switch itself back to
    full operation. */
    virtual void LeaveStandby() override
    {

    }
};

} // namespace smartvr

void *HmdDriverFactory(const char *pInterfaceName, int *pReturnCode)
{
    static smartvr::SmartClient S_oClientInstance{};
    static smartvr::SmartServer S_oServerInstance{};

    if (0 == std::strcmp(vr::IClientTrackedDeviceProvider_Version, pInterfaceName))
    {
        return static_cast<vr::IClientTrackedDeviceProvider *>(&S_oClientInstance);
    }

    if (0 == std::strcmp(vr::IServerTrackedDeviceProvider_Version, pInterfaceName))
    {
        return static_cast<vr::IServerTrackedDeviceProvider *>(&S_oServerInstance);
    }

    if (pReturnCode)
    {
        *pReturnCode = vr::VRInitError_Init_InterfaceNotFound;
    }

    return nullptr;
}
