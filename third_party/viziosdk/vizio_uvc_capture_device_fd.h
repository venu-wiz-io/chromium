//
// vizio_uvc_capture_device_fd.h
// © 2020 Vizio Services, All Rights Reserved
//

#pragma once

#include "vizio_uvc_capture_device.h"

namespace viziosdk { namespace media { namespace capture {

class VizioUVCCaptureDeviceFD {
public:
    explicit VizioUVCCaptureDeviceFD(VizioUVCCaptureDevice* vizio_uvc);
    VizioUVCCaptureDeviceFD(VizioUVCCaptureDevice* vizio_uvc, int device_fd);
    ~VizioUVCCaptureDeviceFD();
    int get() const;
    void reset(int fd = kInvalidFD);
    bool is_valid() const;

    static constexpr int kInvalidFD = -1;
private:
    int device_fd_;
    VizioUVCCaptureDevice* const vizio_uvc_;
};

}}} // namespace viziosdk::media::capture
