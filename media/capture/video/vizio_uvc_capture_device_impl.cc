//
// vizio_uvc_capture_device_impl.cc
// © 2020 Vizio Services, All Rights Reserved
//


#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/poll.h>
#include <unistd.h>

#include "vizio_uvc_capture_device_impl.h"

namespace viziosdk { namespace media { namespace capture {

VizioUVCCaptureDeviceImpl::~VizioUVCCaptureDeviceImpl() = default;

int VizioUVCCaptureDeviceImpl::open(const char* device_name, int flags) {
    return ::open(device_name, flags);
}

int VizioUVCCaptureDeviceImpl::close(int fd) {
    return ::close(fd);
}

int VizioUVCCaptureDeviceImpl::ioctl(int fd, int request, void* argp) {
    return ::ioctl(fd, request, argp);
}

void* VizioUVCCaptureDeviceImpl::mmap(void* start,
                                      size_t length,
                                      int prot,
                                      int flags,
                                      int fd,
                                      off_t offset) {
    return ::mmap(start, length, prot, flags, fd, offset);
}

int VizioUVCCaptureDeviceImpl::munmap(void* start, size_t length) {
    return ::munmap(start, length);
}

int VizioUVCCaptureDeviceImpl::poll(struct pollfd* ufds,
                                    unsigned int nfds,
                                    int timeout) {
    return ::poll(ufds, nfds, timeout);
}

}}} // namespace viziosdk::media::capture
