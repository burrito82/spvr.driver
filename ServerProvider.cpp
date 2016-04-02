/*
 * Copyright (c) 2016
 *  Somebody
 */
#include "ServerProvider.h"

#include "Context.h"
#include "HmdDriver.h"
#include "Logger.h"

#include <chrono>
#include <cstring>
#include <fstream>
#include <memory>
#include <string>

namespace spvr
{

SmartServer::SmartServer():
m_pHmdDriver{}
{

}

SmartServer::~SmartServer() = default;

vr::EVRInitError SmartServer::Init(vr::IDriverLog *pDriverLog, vr::IServerDriverHost *pDriverHost, const char *pchUserDriverConfigDir, const char *pchDriverInstallDir)
{
    if (!pchUserDriverConfigDir)
    {
        pchUserDriverConfigDir = "nullptr";
    }
    if (!pchDriverInstallDir)
    {
        pchDriverInstallDir = "nullptr";
    }
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
    if (pDriverLog)
    {
        m_pLogger->AddDriverLog(pDriverLog);
    }
    m_pLogger->Debug(std::string{"SmartServer::Init(\""} +pchUserDriverConfigDir + "\", \"" + pchDriverInstallDir + "\");\n");

    try
    {
        if (pDriverHost)
        {
            m_pHmdDriver = std::make_unique<HmdDriver>(pDriverHost, m_pLogger);
            pDriverHost->TrackedDeviceAdded(m_pHmdDriver->GetSerialNumber());
        }
        else
        {
            m_pLogger->Log("Initialization of HmdDriver failed, pDriverHost == nullptr!\n");
            return vr::VRInitError_Init_HmdNotFound;
        }
    }
    catch (...)
    {
        m_pLogger->Log("Initialization of HmdDriver failed!\n");
        return vr::VRInitError_Init_HmdNotFound;
    }

    return vr::VRInitError_None;
}

/** cleans up the driver right before it is unloaded */
void SmartServer::Cleanup()
{
    if (m_pLogger)
    {
        m_pLogger->Debug("SmartServer::Cleanup()\n");
    }
    m_pHmdDriver.reset(nullptr);
    //Context::Destroy();
}

/** returns the number of HMDs that this driver manages that are physically connected. */
std::uint32_t SmartServer::GetTrackedDeviceCount()
{
    m_pLogger->Debug("SmartServer::GetTrackedDeviceCount()\n");
    return 1;
}

/** returns a single HMD */
vr::ITrackedDeviceServerDriver *SmartServer::GetTrackedDeviceDriver(std::uint32_t uWhich, char const *pchInterfaceVersion)
{
    if (!pchInterfaceVersion)
    {
        pchInterfaceVersion = "nullptr";
    }
    m_pLogger->Debug(std::string{"SmartServer::GetTrackedDeviceDriver("} +std::to_string(uWhich) + ", \"" + pchInterfaceVersion + "\");\n");
    if (std::string{pchInterfaceVersion} != vr::ITrackedDeviceServerDriver_Version)
    {
        return nullptr;
    }

    return m_pHmdDriver.get();
}

/** returns a single HMD by ID */
vr::ITrackedDeviceServerDriver *SmartServer::FindTrackedDeviceDriver(char const *pchId, char const *pchInterfaceVersion)
{
    if (!pchId)
    {
        pchId = "nullptr";
    }
    if (!pchInterfaceVersion)
    {
        pchInterfaceVersion = "nullptr";
    }
    m_pLogger->Debug(std::string{"SmartServer::GetTrackedDeviceDriver("} +pchId + ", \"" + pchInterfaceVersion + "\");\n");
    if (std::string{pchInterfaceVersion} != vr::ITrackedDeviceServerDriver_Version)
    {
        return nullptr;
    }

    return m_pHmdDriver.get();
}

/** Allows the driver do to some work in the main loop of the server. */
void SmartServer::RunFrame()
{
    if (m_pHmdDriver)
    {
        static auto msSinceLastRunFrame = std::chrono::system_clock::now();
        auto const now = std::chrono::system_clock::now();
        auto const timeDiff = now - msSinceLastRunFrame;
        msSinceLastRunFrame = now;
        //m_pLogger->Log(std::string{"SmartServer::RunFrame() [time since last: "} +std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeDiff).count()) + " ms]\n");
        m_pHmdDriver->RunFrame();
    }
}


// ------------  Power State Functions ----------------------- //

/** Returns true if the driver wants to block Standby mode. */
bool SmartServer::ShouldBlockStandbyMode()
{
    m_pLogger->Debug("SmartServer::ShouldBlockStandbyMode()\n");
    return false;
}

/** Called when the system is entering Standby mode. The driver should switch itself into whatever sort of low-power
* state it has. */
void SmartServer::EnterStandby()
{
    m_pLogger->Debug("SmartServer::EnterStandby()\n");
}

/** Called when the system is leaving Standby mode. The driver should switch itself back to
full operation. */
void SmartServer::LeaveStandby()
{
    m_pLogger->Debug("SmartServer::LeaveStandby()\n");
}

} // namespace spvr
