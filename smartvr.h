/*
 * Copyright (c) 2016
 *  Somebody
 */
#ifndef SPVR_SMARTVR_H
#define SPVR_SMARTVR_H

#include "SVRLibConfig.h"

extern char const copyright[];

HMD_DLL_EXPORT void *HmdDriverFactory(char const *pInterfaceName, int *pReturnCode);

#endif // SPVR_SMARTVR_H
