/*
 * Copyright (c) 2016
 *  Somebody
 */
#include "ServerProvider.h"

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

SmartServer::SmartServer():
m_pHmdDriver{}
{

}

SmartServer::~SmartServer() = default;

vr::EVRInitError SmartServer::Init(vr::IDriverLog *pDriverLog, vr::IServerDriverHost *pDriverHost, const char *pchUserDriverConfigDir, const char *pchDriverInstallDir)
{
    m_pHmdDriver = std::make_unique<HmdDriver>(pDriverHost);
    return vr::EVRInitError::VRInitError_None;
}

/** cleans up the driver right before it is unloaded */
void SmartServer::Cleanup()
{

}

/** returns the number of HMDs that this driver manages that are physically connected. */
uint32_t SmartServer::GetTrackedDeviceCount()
{
    return 1;
}

/** returns a single HMD */
vr::ITrackedDeviceServerDriver *SmartServer::GetTrackedDeviceDriver(uint32_t unWhich, const char *pchInterfaceVersion)
{
    if (std::string{pchInterfaceVersion} != vr::ITrackedDeviceServerDriver_Version)
    {
        return nullptr;
    }

    return m_pHmdDriver.get();
}

/** returns a single HMD by ID */
vr::ITrackedDeviceServerDriver *SmartServer::FindTrackedDeviceDriver(const char *pchId, const char *pchInterfaceVersion)
{
    if (std::string{pchInterfaceVersion} != vr::ITrackedDeviceServerDriver_Version)
    {
        return nullptr;
    }

    return m_pHmdDriver.get();
}

/** Allows the driver do to some work in the main loop of the server. */
void SmartServer::RunFrame()
{

}


// ------------  Power State Functions ----------------------- //

/** Returns true if the driver wants to block Standby mode. */
bool SmartServer::ShouldBlockStandbyMode()
{
    return false;
}

/** Called when the system is entering Standby mode. The driver should switch itself into whatever sort of low-power
* state it has. */
void SmartServer::EnterStandby()
{

}

/** Called when the system is leaving Standby mode. The driver should switch itself back to
full operation. */
void SmartServer::LeaveStandby()
{

}

} // namespace smartvr
