//
// vizio_video_capture_device.cc
// © 2020 Vizio Services, All Rights Reserved
//


#include <stddef.h>
#include <assert.h>
#include <unistd.h>
#include <utility>
#include <thread>
#include <linux/videodev2.h>

#include "vizio_video_capture_device.h"
#include "vizio_sdk_logger.h"
#include "vizio_uvc_capture_delegate.h"

namespace viziosdk { namespace media { namespace capture {


VizioVideoCaptureDevice::VizioVideoCaptureDevice(
      std::shared_ptr<VizioUVCCaptureDevice> vizio_uvc,
      const std::string& deviceId)
      : device_descriptor_(deviceId),
        vizio_uvc_((vizio_uvc)) /*,
        capture_impl_(std::make_shared<VizioUVCCaptureDelegate>(
            vizio_uvc_.get(), device_descriptor_)) */ /*;
        vizio_uvc_thread_(std::thread())*/ {}

VizioVideoCaptureDevice::~VizioVideoCaptureDevice() {
    // Check if the thread is running.
    // This means that the device has not been StopAndDeAllocate()d properly.
//    assert(!vizio_uvc_thread_.IsRunning());
//    vizio_uvc_thread_.Stop();
    // TODO cleanup
}

void VizioVideoCaptureDevice::AllocateAndStart(
      const CameraParameters& params,
      std::shared_ptr<VizioVideoCaptureDeviceClient> client) {
    viziosdk::INFO(__file_line__ + __func__);

// TODO protect from reopen
//    if (!vizio_uvc_thread_.joinable())
//        return;  // Wrong state.
//    vizio_uvc_thread_.Start();

#if 1
    capture_impl_ = std::make_unique<VizioUVCCaptureDelegate>(
        vizio_uvc_.get(), device_descriptor_);
    if (!capture_impl_) {
        return;
    }
#endif
    vizio_uvc_thread_ = (std::thread(&VizioUVCCaptureDelegate::AllocateAndStart, capture_impl_.get(),
                   params.formatInfoVec[0].fourcc,
                   params.formatInfoVec[0].width,
                   params.formatInfoVec[0].height,
                   params.formatInfoVec[0].frameRate, std::ref(client)));
    vizio_uvc_thread_.detach();
}

void VizioVideoCaptureDevice::StopAndDeAllocate() {
    if (!vizio_uvc_thread_.joinable())
        return;  // Wrong state.
    // TODO notify vizio_uvc_thread to stop
#if 0
    vizio_uvc_thread_.task_runner()->PostTask(
        FROM_HERE, base::BindOnce(&VizioUVCCaptureDelegate::StopAndDeAllocate,
                                  capture_impl_->GetWeakPtr()));
#endif
    vizio_uvc_thread_.join();

    capture_impl_ = nullptr;
}

}}} // namespace viziosdk::media::capture
