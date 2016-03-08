/*
 * Copyright (c) 2016
 *  Somebody
 */
#ifndef SVR_SMARTVR_H
#define SVR_SMARTVR_H

extern char const copyright[];

#define HMD_DLL_EXPORT extern "C" __declspec(dllexport)

HMD_DLL_EXPORT
void *HmdDriverFactory(const char *pInterfaceName, int *pReturnCode);

#endif // SVR_SMARTVR_H

