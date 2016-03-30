/*
 * Copyright (c) 2016
 *  Somebody
 */
#ifndef SPVR_SERVERPROVIDER_H
#define SPVR_SERVERPROVIDER_H

#include "openvr_driver.h"

#include <cstdint>
#include <memory>

namespace spvr
{

class HmdDriver;
class Logger;

class SmartServer final : public vr::IServerTrackedDeviceProvider
{
    Logger *m_pLogger;
public:
    SmartServer();
    ~SmartServer();

    /** initializes the driver. This will be called before any other methods are called.
    * If Init returns anything other than VRInitError_None the driver DLL will be unloaded.
    *
    * pDriverHost will never be NULL, and will always be a pointer to a IServerDriverHost interface
    *
    * pchUserDriverConfigDir - The absolute path of the directory where the driver should store user
    *	config files.
    * pchDriverInstallDir - The absolute path of the root directory for the driver.
    */
    virtual vr::EVRInitError Init(vr::IDriverLog *pDriverLog, vr::IServerDriverHost *pDriverHost, char const *pchUserDriverConfigDir, char const *pchDriverInstallDir) override;

    /** cleans up the driver right before it is unloaded */
    virtual void Cleanup() override;

    /** returns the number of HMDs that this driver manages that are physically connected. */
    virtual std::uint32_t GetTrackedDeviceCount() override;

    /** returns a single HMD */
    virtual vr::ITrackedDeviceServerDriver *GetTrackedDeviceDriver(std::uint32_t uWhich, char const *pchInterfaceVersion) override;

    /** returns a single HMD by ID */
    virtual vr::ITrackedDeviceServerDriver* FindTrackedDeviceDriver(char const *pchId, char const *pchInterfaceVersion) override;

    /** Allows the driver do to some work in the main loop of the server. */
    virtual void RunFrame() override;

    // ------------  Power State Functions ----------------------- //

    /** Returns true if the driver wants to block Standby mode. */
    virtual bool ShouldBlockStandbyMode() override;

    /** Called when the system is entering Standby mode. The driver should switch itself into whatever sort of low-power
    * state it has. */
    virtual void EnterStandby() override;

    /** Called when the system is leaving Standby mode. The driver should switch itself back to
    full operation. */
    virtual void LeaveStandby() override;
private:
    std::unique_ptr<HmdDriver> m_pHmdDriver;
};

} // namespace spvr

#endif // SPVR_SERVERPROVIDER_H
