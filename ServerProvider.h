/*
 * Copyright (c) 2016
 *  Somebody
 */
#ifndef SVR_SERVERPROVIDER_H
#define SVR_SERVERPROVIDER_H

#include "openvr_driver.h"

namespace smartvr
{

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
    virtual vr::EVRInitError Init(vr::IDriverLog *pDriverLog, vr::IServerDriverHost *pDriverHost, const char *pchUserDriverConfigDir, const char *pchDriverInstallDir) override;

    /** cleans up the driver right before it is unloaded */
    virtual void Cleanup() override;

    /** returns the number of HMDs that this driver manages that are physically connected. */
    virtual uint32_t GetTrackedDeviceCount() override;

    /** returns a single HMD */
    virtual vr::ITrackedDeviceServerDriver *GetTrackedDeviceDriver(uint32_t unWhich, const char *pchInterfaceVersion) override;

    /** returns a single HMD by ID */
    virtual vr::ITrackedDeviceServerDriver* FindTrackedDeviceDriver(const char *pchId, const char *pchInterfaceVersion) override;

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
};

} // namespace smartvr

#endif // SVR_SERVERPROVIDER_H
