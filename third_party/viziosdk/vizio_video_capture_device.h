//
// vizio_video_capture_device.h
// © 2020 Vizio Services, All Rights Reserved
//


#pragma once


#include <stdint.h>
#include <memory>
#include <thread>
#include <string>
#include <vector>

#include "vizio_uvc_capture_device_impl.h"
#include "vizio_video_capture_device_client.h"
#include "vizio_camera_device_info.h"

namespace viziosdk { namespace media { namespace capture {

class VizioUVCCaptureDelegate;

class VizioVideoCaptureDevice {
public:
    //static std::vector<uint32_t> GetListOfUsableFourCCs(bool favour_mjpeg);

    explicit VizioVideoCaptureDevice(
        std::shared_ptr<VizioUVCCaptureDevice> vizio_uvc,
        const std::string& deviceId);
    ~VizioVideoCaptureDevice();

    // VideoCaptureDevice implementation.
    void AllocateAndStart(const CameraParameters& params,
                          std::shared_ptr<VizioVideoCaptureDeviceClient> client);
    void StopAndDeAllocate();

// TODO photo interface
#if 0
    void TakePhoto(TakePhotoCallback callback);
    void GetPhotoState(GetPhotoStateCallback callback);
    void SetPhotoOptions(mojom::PhotoSettingsPtr settings,
                         SetPhotoOptionsCallback callback);

protected:
    const VideoCaptureDeviceDescriptor device_descriptor_;
#endif

private:
    std::string device_descriptor_;

    std::shared_ptr<VizioUVCCaptureDevice> vizio_uvc_;

    std::shared_ptr<VizioUVCCaptureDelegate> capture_impl_;

    std::thread vizio_uvc_thread_;  // Thread used for reading data from the device.
};

}}} // namespace viziosdk::media::capture

