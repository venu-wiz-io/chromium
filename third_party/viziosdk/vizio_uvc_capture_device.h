//
// vizio_uvc_capture_device.h
// © 2020 Vizio Services, All Rights Reserved
//

#pragma once

#include <poll.h>
#include <sys/fcntl.h>

namespace viziosdk { namespace media { namespace capture {

// Interface for abstracting out the UVC driver API
class VizioUVCCaptureDevice {
public:
    virtual int open(const char* device_name, int flags) = 0;
    virtual int close(int fd) = 0;
    virtual int ioctl(int fd, int request, void* argp) = 0;
    virtual void* mmap(void* start,
                       size_t length,
                       int prot,
                       int flags,
                       int fd,
                       off_t offset) = 0;

    virtual int munmap(void* start, size_t length) = 0;
    virtual int poll(struct pollfd* ufds, unsigned int nfds, int timeout) = 0;

protected:
    virtual ~VizioUVCCaptureDevice() {}
};

}}} // namespace viziosdk::media::capture

