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

std::unique_ptr<smartvr::SmartClient> S_pClientInstance{};
std::unique_ptr<smartvr::SmartServer> S_pServerInstance{};

} // unnamed namespace

char const copyright[] =
"Copyright (c) 2016\n\tSomebody.  All rights reserved.\n\n";

void *HmdDriverFactory(const char *pInterfaceName, int *pReturnCode)
{
    if (0 == std::strcmp(vr::IClientTrackedDeviceProvider_Version, pInterfaceName))
    {
        if (!S_pClientInstance)
        {
            S_pClientInstance = std::make_unique<smartvr::SmartClient>();
        }
        return static_cast<vr::IClientTrackedDeviceProvider *>(S_pClientInstance.get());
    }

    if (0 == std::strcmp(vr::IServerTrackedDeviceProvider_Version, pInterfaceName))
    {
        if (!S_pServerInstance)
        {
            S_pServerInstance = std::make_unique<smartvr::SmartServer>();
        }
        return static_cast<vr::IServerTrackedDeviceProvider *>(S_pServerInstance.get());
    }

    if (pReturnCode)
    {
        *pReturnCode = vr::VRInitError_Init_InterfaceNotFound;
    }

    return nullptr;
}
