//
// vizio_uvc_capture_device_impl.h
// © 2020 Vizio Services, All Rights Reserved
//

#pragma once

#include "vizio_uvc_capture_device.h"

namespace viziosdk { namespace media { namespace capture {

// Implementation of VizioUVCCaptureDevice interface 
class VizioUVCCaptureDeviceImpl : public VizioUVCCaptureDevice {
public:
    ~VizioUVCCaptureDeviceImpl() override;

    int open(const char* device_name, int flags) override;
    int close(int fd) override;
    int ioctl(int fd, int request, void* argp) override;
    void* mmap(void* start,
               size_t length,
               int prot,
               int flags,
               int fd,
               off_t offset) override;

    int munmap(void* start, size_t length) override;
    int poll(struct pollfd* ufds, unsigned int nfds, int timeout) override;
};

}}} // namespace viziosdk::media::capture

