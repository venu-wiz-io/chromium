//
// vizio_video_capture_device_factory.h
// © 2020 Vizio Services, All Rights Reserved
//


#pragma once

// Implementation of a VizioVideoCaptureDeviceFactory class.

#include <memory>
#include <string>
#include <vector>

#include "vizio_video_capture_device.h"

namespace viziosdk { namespace media { namespace capture {

class VizioVideoCaptureDeviceFactory {
public:
    class DeviceProvider {
    public:
        virtual ~DeviceProvider() {}
        virtual void GetDeviceIds(std::vector<std::string>* target_container) = 0;
        virtual std::string GetDeviceModelId(const std::string& device_id) = 0;
        virtual std::string GetDeviceDisplayName(const std::string& device_id) = 0;
    };

    VizioVideoCaptureDeviceFactory();
    ~VizioVideoCaptureDeviceFactory();

    std::unique_ptr<VizioVideoCaptureDevice> CreateDevice(
        const std::string& deviceId);
    void GetDeviceDescriptors(
          CameraDeviceInfoVec& deviceInfoVec);
    void GetSupportedFormats(
        CameraDeviceInfo& deviceInfo);

private:
    // Simple wrapper to do HANDLE_EINTR(v4l2_->ioctl(fd, ...)).
    int DoIoctl(int fd, int request, void* argp);

    std::vector<float> GetFrameRateList(int fd,
                                        uint32_t fourcc,
                                        uint32_t width,
                                        uint32_t height);

    std::shared_ptr<VizioUVCCaptureDevice> vizio_uvc_;
    std::unique_ptr<DeviceProvider> device_provider_;
};

}}} // namespace viziosdk::media::capture
