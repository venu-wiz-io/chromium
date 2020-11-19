//
// vizio_uvc_capture_device_fd.cc
// © 2020 Vizio Services, All Rights Reserved
//

#include "vizio_uvc_capture_device_fd.h"

namespace viziosdk { namespace media { namespace capture {

VizioUVCCaptureDeviceFD::VizioUVCCaptureDeviceFD(VizioUVCCaptureDevice* vizio_uvc)
    : device_fd_(kInvalidFD), vizio_uvc_(vizio_uvc) {}

VizioUVCCaptureDeviceFD::VizioUVCCaptureDeviceFD(VizioUVCCaptureDevice* vizio_uvc, int device_fd)
    : device_fd_(device_fd), vizio_uvc_(vizio_uvc) {}

VizioUVCCaptureDeviceFD::~VizioUVCCaptureDeviceFD() {
  if (is_valid())
    reset();
}

int VizioUVCCaptureDeviceFD::get() const {
  return device_fd_;
}

void VizioUVCCaptureDeviceFD::reset(int fd) {
  if (is_valid())
    vizio_uvc_->close(device_fd_);
  device_fd_ = fd;
}

bool VizioUVCCaptureDeviceFD::is_valid() const {
  return device_fd_ != kInvalidFD;
}

}}} // namespace viziosdk::media::capture
