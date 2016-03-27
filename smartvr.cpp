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

//std::unique_ptr<smartvr::SmartClient> S_pClientInstance{};
//std::unique_ptr<smartvr::SmartServer> S_pServerInstance{};
smartvr::SmartClient S_oClientInstance{};
smartvr::SmartServer S_oServerInstance{};

} // unnamed namespace

char const copyright[] =
"Copyright (c) 2016\n\tSomebody.  All rights reserved.\n\n";

void *HmdDriverFactory(char const *pInterfaceName, int *pReturnCode)
{
    if (0 == std::strcmp(vr::IClientTrackedDeviceProvider_Version, pInterfaceName))
    {
        /*if (!S_pClientInstance)
        {
            S_pClientInstance = std::make_unique<smartvr::SmartClient>();
        }
        return static_cast<vr::IClientTrackedDeviceProvider *>(S_pClientInstance.get());*/
        return static_cast<vr::IClientTrackedDeviceProvider *>(&S_oClientInstance);
    }

    if (0 == std::strcmp(vr::IServerTrackedDeviceProvider_Version, pInterfaceName))
    {
        /*if (!S_pServerInstance)
        {
            S_pServerInstance = std::make_unique<smartvr::SmartServer>();
        }
        return static_cast<vr::IServerTrackedDeviceProvider *>(S_pServerInstance.get());*/
        return static_cast<vr::IServerTrackedDeviceProvider *>(&S_oServerInstance);
    }

    if (pReturnCode)
    {
        *pReturnCode = vr::VRInitError_Init_InterfaceNotFound;
    }

    return nullptr;
}
