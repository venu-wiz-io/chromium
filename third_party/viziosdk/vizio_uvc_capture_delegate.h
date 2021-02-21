//
// vizio_uvc_capture_delegate.h
// © 2020 Vizio Services, All Rights Reserved
//

#pragma once


#include <stddef.h>
#include <stdint.h>
#include <linux/videodev2.h>

#include <memory>
#include <string>
#include <vector>

#include "vizio_sdk_types.h"
#include "vizio_uvc_capture_device_impl.h"
#include "vizio_uvc_capture_device_fd.h"
#include "vizio_video_capture_device_client.h"
#include "vizio_video_capture_format.h"
#include "vizio_video_capture_types.h"


namespace viziosdk { namespace media { namespace capture {

// Class doing the actual capture using V4L2 API. 
class VizioUVCCaptureDelegate {
public:
    // Retrieves the #planes for a given |fourcc|, or 0 if unknown.
    static size_t GetNumPlanesForFourCc(uint32_t fourcc);
    static VideoPixelFormat V4l2FourCcToChromiumPixelFormat(
        uint32_t v4l2_fourcc);

    static std::vector<uint32_t> GetListOfUsableFourCcs(bool prefer_mjpeg);

    VizioUVCCaptureDelegate(
        VizioUVCCaptureDevice* vizio_uvc,
        const std::string& deviceId);
    ~VizioUVCCaptureDelegate();

    void AllocateAndStart(const uint32_t fourcc,
                          const uint32_t width,
                          const uint32_t height,
                          const float frame_rate,
                          std::shared_ptr<VizioVideoCaptureDeviceClient> client);
    void StopAndDeAllocate();

private:
//     friend class VizioUVCCaptureDelegateTest;

    class BufferTracker;

    bool RunIoctl(int request, void* argp);

    int DoIoctl(int request, void* argp);

    // Sets all user control to their default. Some controls are enabled by
    // another flag, usually having the word "auto" in the name, see
    // IsSpecialControl() in the .cc file. These flags are preset beforehand, then
    // set to their defaults individually afterwards.
    void ResetUserAndCameraControlsToDefault();

    // VIDIOC_QUERYBUFs a buffer from V4L2, creates a BufferTracker for it and
    // enqueues it (VIDIOC_QBUF) back into V4L2.
    bool MapAndQueueBuffer(int index);

    void DoCapture();

    void SetErrorState(VideoCaptureError error,
                       const std::string& from_here,
                       const std::string& reason);

    VizioUVCCaptureDevice* const vizio_uvc_;
//    const scoped_refptr<base::SingleThreadTaskRunner> vizio_uvc_task_runner_;
    const std::string device_id_;
//    const int power_line_frequency_;

    // The following members are only known on AllocateAndStart().
    VideoCaptureFormat capture_format_;
    v4l2_format video_fmt_;
    std::shared_ptr<VizioVideoCaptureDeviceClient> client_;
    VizioUVCCaptureDeviceFD device_fd_;

    // Vector of BufferTracker to keep track of mmap()ed pointers and their use.
    std::vector<std::shared_ptr<BufferTracker>> buffer_tracker_pool_;

    bool is_capturing_;
    int timeout_count_;

    viziosdk::high_res_time_point first_ref_time_;

    int rotation_;
};

}}} // namespace viziosdk::media::capture
