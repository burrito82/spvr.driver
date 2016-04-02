/*
 * Copyright (c) 2016
 *  Somebody
 */
#ifndef SPVR_HMDDRIVER_H
#define SPVR_HMDDRIVER_H

#include "openvr_driver.h"

#include <cstdint>
#include <string>
#include <thread>

namespace spvr
{

class Logger;

class HmdDriver final : public vr::ITrackedDeviceServerDriver, public vr::IVRDisplayComponent
{
public:
    explicit HmdDriver(vr::IServerDriverHost *pServerDriverHost, Logger *pDriverLog = nullptr);
    ~HmdDriver() = default;

    void RunFrame();

    char const *GetSerialNumber() const;
    char const *GetModelNumber() const;

private:
    std::string HmdDriver::GetStringTrackedDeviceProperty(vr::ETrackedDeviceProperty prop, vr::ETrackedPropertyError &rError);

    vr::IServerDriverHost *m_pServerDriverHost;
    Logger *m_pDriverLog;
    std::uint32_t m_uObjectId;

    std::string m_sSerialNumber;
    std::string m_sModelNumber;

    float m_fIPD;
    float m_fDisplayFrequency;
    float m_fSecondsFromVsyncToPhotons;

    std::int32_t m_iWindowX;
    std::int32_t m_iWindowY;
    std::int32_t m_iWindowWidth;
    std::int32_t m_iWindowHeight;
    std::int32_t m_iRenderWidth;
    std::int32_t m_iRenderHeight;

    std::thread m_oPoseUpdateThread;

    float m_fDistortionK0;
    float m_fDistortionK1;
    float m_fDistortionScale;

    // ITrackedDeviceServerDriver
public:

    // ------------------------------------
    // Management Methods
    // ------------------------------------
    /** This is called before an HMD is returned to the application. It will always be
    * called before any display or tracking methods. Memory and processor use by the
    * ITrackedDeviceServerDriver object should be kept to a minimum until it is activated.
    * The pose listener is guaranteed to be valid until Deactivate is called, but
    * should not be used after that point. */
    virtual vr::EVRInitError Activate(std::uint32_t uObjectId) override;

    /** This is called when The VR system is switching from this Hmd being the active display
    * to another Hmd being the active display. The driver should clean whatever memory
    * and thread use it can when it is deactivated */
    virtual void Deactivate() override;

    /** Handles a request from the system to power off this device */
    virtual void PowerOff() override;

    /** Requests a component interface of the driver for device-specific functionality. The driver should return NULL
    * if the requested interface or version is not supported. */
    virtual void *GetComponent(char const *pchComponentNameAndVersion) override;

    /** A VR Client has made this debug request of the driver. The set of valid requests is entirely
    * up to the driver and the client to figure out, as is the format of the response. Responses that
    * exceed the length of the supplied buffer should be truncated and null terminated */
    virtual void DebugRequest(char const *pchRequest, char *pchResponseBuffer, std::uint32_t uResponseBufferSize) override;

    // ------------------------------------
    // Tracking Methods
    // ------------------------------------
    virtual vr::DriverPose_t GetPose() override;

    // ------------------------------------
    // Property Methods
    // ------------------------------------

    /** Returns a bool property. If the property is not available this function will return false. */
    virtual bool GetBoolTrackedDeviceProperty(vr::ETrackedDeviceProperty prop, vr::ETrackedPropertyError *pError) override;

    /** Returns a float property. If the property is not available this function will return 0. */
    virtual float GetFloatTrackedDeviceProperty(vr::ETrackedDeviceProperty prop, vr::ETrackedPropertyError *pError) override;

    /** Returns an int property. If the property is not available this function will return 0. */
    virtual std::int32_t GetInt32TrackedDeviceProperty(vr::ETrackedDeviceProperty prop, vr::ETrackedPropertyError *pError) override;

    /** Returns a uint64 property. If the property is not available this function will return 0. */
    virtual std::uint64_t GetUint64TrackedDeviceProperty(vr::ETrackedDeviceProperty prop, vr::ETrackedPropertyError *pError) override;

    /** Returns a matrix property. If the device index is not valid or the property is not a matrix type, this function will return identity. */
    virtual vr::HmdMatrix34_t GetMatrix34TrackedDeviceProperty(vr::ETrackedDeviceProperty prop, vr::ETrackedPropertyError *pError) override;

    /** Returns a string property. If the property is not available this function will return 0 and pError will be
    * set to an error. Otherwise it returns the length of the number of bytes necessary to hold this string including
    * the trailing null. If the buffer is too small the error will be TrackedProp_BufferTooSmall. Strings will
    * generally fit in buffers of k_unTrackingStringSize characters. Drivers may not return strings longer than
    * k_unMaxPropertyStringSize. */
    virtual std::uint32_t GetStringTrackedDeviceProperty(vr::ETrackedDeviceProperty prop, char *pchValue, std::uint32_t uBufferSize, vr::ETrackedPropertyError *pError) override;

    // IVRDisplayComponent
public:
    // ------------------------------------
    // Display Methods
    // ------------------------------------

    /** Size and position that the window needs to be on the VR display. */
    virtual void GetWindowBounds(std::int32_t *piX, std::int32_t *piY, std::uint32_t *puWidth, std::uint32_t *puHeight) override;

    /** Returns true if the display is extending the desktop. */
    virtual bool IsDisplayOnDesktop() override;

    /** Returns true if the display is real and not a fictional display. */
    virtual bool IsDisplayRealDisplay() override;

    /** Suggested size for the intermediate render target that the distortion pulls from. */
    virtual void GetRecommendedRenderTargetSize(std::uint32_t *pnWidth, std::uint32_t *pnHeight) override;

    /** Gets the viewport in the frame buffer to draw the output of the distortion into */
    virtual void GetEyeOutputViewport(vr::EVREye eEye, std::uint32_t *puX, std::uint32_t *puY, std::uint32_t *puWidth, std::uint32_t *puHeight) override;

    /** The components necessary to build your own projection matrix in case your
    * application is doing something fancy like infinite Z */
    virtual void GetProjectionRaw(vr::EVREye eEye, float *pfLeft, float *pfRight, float *pfTop, float *pfBottom) override;

    /** Returns the result of the distortion function for the specified eye and input UVs. UVs go from 0,0 in
    * the upper left of that eye's viewport and 1,1 in the lower right of that eye's viewport. */
    virtual vr::DistortionCoordinates_t ComputeDistortion(vr::EVREye eEye, float fU, float fV) override;

    // -----------------------------------
    // Direct mode methods
    // -----------------------------------

    /** Specific to Oculus compositor support, textures supplied must be created using this method. */
    virtual void CreateSwapTextureSet(std::uint32_t uPid, std::uint32_t uFormat, std::uint32_t uWidth, std::uint32_t uHeight, void *(*pSharedTextureHandles)[2]) override;

    /** Used to textures created using CreateSwapTextureSet.  Only one of the set's handles needs to be used to destroy the entire set. */
    virtual void DestroySwapTextureSet(void *pSharedTextureHandle) override;

    /** Used to purge all texture sets for a given process. */
    virtual void DestroyAllSwapTextureSets(std::uint32_t uPid) override;

    /** Call once per layer to draw for this frame.  One shared texture handle per eye.  Textures must be created
    * using CreateSwapTextureSet and should be alternated per frame.  Call Present once all layers have been submitted. */
    virtual void SubmitLayer(void *pSharedTextureHandles[2], vr::VRTextureBounds_t const (&bounds)[2], vr::HmdMatrix34_t const *pPose) override;

    /** Submits queued layers for display. */
    virtual void Present(void *hSyncTexture) override;
};

} // namespace spvr

#endif // SPVR_HMDDRIVER_H
