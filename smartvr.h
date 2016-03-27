/*
 * Copyright (c) 2016
 *  Somebody
 */
#ifndef SVR_SMARTVR_H
#define SVR_SMARTVR_H

#include "SVRLibConfig.h"

extern char const copyright[];

HMD_DLL_EXPORT void *HmdDriverFactory(char const *pInterfaceName, int *pReturnCode);

#endif // SVR_SMARTVR_H
