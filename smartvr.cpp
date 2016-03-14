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

namespace smartvr
{

std::ofstream debug{"debug.log"};

} // namespace smartvr

char const copyright[] =
"Copyright (c) 2016\n\tSomebody.  All rights reserved.\n\n";

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
