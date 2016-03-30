/*
 * Copyright (c) 2016
 *  Somebody
 */
#include "smartvr.h"

#include "ClientProvider.h"
#include "ServerProvider.h"

#include "openvr_driver.h"

#include <cstring>
#include <fstream>
#include <memory>

namespace
{

spvr::SmartClient S_oClientInstance{};
spvr::SmartServer S_oServerInstance{};

} // unnamed namespace

void *HmdDriverFactory(char const *pInterfaceName, int *pReturnCode)
{
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
