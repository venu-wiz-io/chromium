//
// v4l2_capture_delegate.cc
// © 2020 Vizio Services, All Rights Reserved
//

#include <linux/version.h>
#include <linux/videodev2.h>
#include <poll.h>
#include <assert.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <cstring>
#include <utility>
#include <algorithm>

#include "vizio_uvc_capture_delegate.h"
#include "vizio_sdk_logger.h"
#include "vizio_video_capture_device.h"
#include "vizio_video_types.h"

#define FROM_HERE    __PRETTY_FUNCTION__

namespace viziosdk { namespace media { namespace capture {

namespace {

// Desired number of video buffers to allocate. The actual number of allocated
// buffers by vizio_uvc driver can be higher or lower than this number.
// kNumVideoBuffers should not be too small, or Chrome may not return enough
// buffers back to driver in time.
constexpr uint32_t kNumVideoBuffers = 4;
// Timeout in milliseconds v4l2_thread_ blocks waiting for a frame from the hw.
// This value has been fine tuned. Before changing or modifying it see
// https://crbug.com/470717
constexpr int kCaptureTimeoutMs = 1000;
// The number of continuous timeouts tolerated before treated as error.
constexpr int kContinuousTimeoutLimit = 10;
// MJPEG is preferred if the requested width or height is larger than this.
#if 0
constexpr int kMjpegWidth = 640;
constexpr int kMjpegHeight = 480;
#endif
// Typical framerate, in fps
constexpr int kTypicalFramerate = 30;


// V4L2 color formats supported by VizioUVCCaptureDelegate derived classes.
// This list is ordered by precedence of use -- but see caveats for MJPEG.
struct {
    uint32_t fourcc;
    VideoPixelFormat pixel_format;
    size_t num_planes;
} constexpr kSupportedFormatsAndPlanarity[] = {
      {V4L2_PIX_FMT_H264, PIXEL_FORMAT_H264, 1},
      {V4L2_PIX_FMT_YUV420, PIXEL_FORMAT_I420, 1},
      {V4L2_PIX_FMT_Y16, PIXEL_FORMAT_Y16, 1},
      {V4L2_PIX_FMT_Z16, PIXEL_FORMAT_Y16, 1},
      {V4L2_PIX_FMT_YUYV, PIXEL_FORMAT_YUY2, 1},
      {V4L2_PIX_FMT_UYVY, PIXEL_FORMAT_UYVY, 1},
      {V4L2_PIX_FMT_RGB24, PIXEL_FORMAT_RGB24, 1},
      // MJPEG is usually sitting fairly low since we don't want to have to
      // decode. However, it is needed for large resolutions due to USB bandwidth
      // limitations, so GetListOfUsableFourCcs() can duplicate it on top, see
      // that method.
      {V4L2_PIX_FMT_MJPEG, PIXEL_FORMAT_MJPEG, 1},
      // JPEG works as MJPEG on some gspca webcams from field reports, see
      // https://code.google.com/p/webrtc/issues/detail?id=529, put it as the
      // least preferred format.
      {V4L2_PIX_FMT_JPEG, PIXEL_FORMAT_MJPEG, 1},
};

// Maximum number of ioctl retries before giving up trying to reset controls.
constexpr int kMaxIOCtrlRetries = 5;

// Base id and class identifier for Controls to be reset.
struct {
    uint32_t control_base;
    uint32_t class_id;
} constexpr kControls[] = {{V4L2_CID_USER_BASE, V4L2_CID_USER_CLASS},
                             {V4L2_CID_CAMERA_CLASS_BASE, V4L2_CID_CAMERA_CLASS}};

// Fill in |format| with the given parameters.
void FillV4L2Format(v4l2_format* format,
                      uint32_t width,
                      uint32_t height,
                      uint32_t pixelformat_fourcc) {
    memset(format, 0, sizeof(*format));
    format->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    format->fmt.pix.width = width;
    format->fmt.pix.height = height;
    format->fmt.pix.pixelformat = pixelformat_fourcc;
    format->fmt.pix.field       = V4L2_FIELD_INTERLACED;
}

// Fills all parts of |buffer|.
void FillV4L2Buffer(v4l2_buffer* buffer, int index) {
    memset(buffer, 0, sizeof(*buffer));
    buffer->memory = V4L2_MEMORY_MMAP;
    buffer->index = index;
    buffer->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
}

void FillV4L2RequestBuffer(v4l2_requestbuffers* request_buffer, int count) {
    memset(request_buffer, 0, sizeof(*request_buffer));
    request_buffer->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    request_buffer->memory = V4L2_MEMORY_MMAP;
    request_buffer->count = count;
}

// Determines if |control_id| is special, i.e. controls another one's state.
bool IsSpecialControl(int control_id) {
    switch (control_id) {
      case V4L2_CID_AUTO_WHITE_BALANCE:
      case V4L2_CID_EXPOSURE_AUTO:
      case V4L2_CID_EXPOSURE_AUTO_PRIORITY:
      case V4L2_CID_FOCUS_AUTO:
        return true;
    }
    return false;
}

// Determines if |control_id| should be skipped, https://crbug.com/697885.
#if !defined(V4L2_CID_PAN_SPEED)
#define V4L2_CID_PAN_SPEED (V4L2_CID_CAMERA_CLASS_BASE + 32)
#endif
#if !defined(V4L2_CID_TILT_SPEED)
#define V4L2_CID_TILT_SPEED (V4L2_CID_CAMERA_CLASS_BASE + 33)
#endif
#if !defined(V4L2_CID_PANTILT_CMD)
#define V4L2_CID_PANTILT_CMD (V4L2_CID_CAMERA_CLASS_BASE + 34)
#endif
bool IsBlacklistedControl(int control_id) {
    switch (control_id) {
      case V4L2_CID_PAN_RELATIVE:
      case V4L2_CID_TILT_RELATIVE:
      case V4L2_CID_PAN_RESET:
      case V4L2_CID_TILT_RESET:
      case V4L2_CID_PAN_ABSOLUTE:
      case V4L2_CID_TILT_ABSOLUTE:
      case V4L2_CID_ZOOM_ABSOLUTE:
      case V4L2_CID_ZOOM_RELATIVE:
      case V4L2_CID_ZOOM_CONTINUOUS:
      case V4L2_CID_PAN_SPEED:
      case V4L2_CID_TILT_SPEED:
      case V4L2_CID_PANTILT_CMD:
        return true;
    }
    return false;
}

}  // namespace

// Class keeping track of a SPLANE V4L2 buffer, mmap()ed on construction and
// munmap()ed on destruction.
class VizioUVCCaptureDelegate::BufferTracker {
public:
    explicit BufferTracker(VizioUVCCaptureDevice* vizio_uvc);
    virtual ~BufferTracker();

    // Abstract method to mmap() given |fd| according to |buffer|.
    bool Init(int fd, const v4l2_buffer& buffer);

    const uint8_t* start() const { return start_; }
    size_t payload_size() const { return payload_size_; }
    void set_payload_size(size_t payload_size) {
      assert(payload_size <= length_);
      payload_size_ = payload_size;
    }

private:
    //friend class base::RefCounted<BufferTracker>;

    VizioUVCCaptureDevice* const v4l2_;
    uint8_t* start_;
    size_t length_;
    size_t payload_size_;
};

// static
size_t VizioUVCCaptureDelegate::GetNumPlanesForFourCc(uint32_t fourcc) {
    for (const auto& fourcc_and_pixel_format : kSupportedFormatsAndPlanarity) {
      if (fourcc_and_pixel_format.fourcc == fourcc)
        return fourcc_and_pixel_format.num_planes;
    }
//    DVLOG(1) << "Unknown fourcc " << FourccToString(fourcc);
    return 0;
}

// static
VideoPixelFormat VizioUVCCaptureDelegate::V4l2FourCcToChromiumPixelFormat(
    uint32_t v4l2_fourcc) {
  for (const auto& fourcc_and_pixel_format : kSupportedFormatsAndPlanarity) {
    if (fourcc_and_pixel_format.fourcc == v4l2_fourcc)
      return fourcc_and_pixel_format.pixel_format;
  }
  // Not finding a pixel format is OK during device capabilities enumeration.
  // Let the caller decide if PIXEL_FORMAT_UNKNOWN is an error or
  // not.
//  DVLOG(1) << "Unsupported pixel format: " << FourccToString(v4l2_fourcc);
  return PIXEL_FORMAT_UNKNOWN;
}

// static
std::vector<uint32_t> VizioUVCCaptureDelegate::GetListOfUsableFourCcs(
      bool prefer_mjpeg) {
    std::vector<uint32_t> supported_formats;
    supported_formats.reserve(sizeof(kSupportedFormatsAndPlanarity));
#if 1
      supported_formats.push_back(V4L2_PIX_FMT_H264);
#else

#ifndef PREF_H264
    // Duplicate MJPEG on top of the list depending on |prefer_mjpeg|.
    if (prefer_mjpeg)
      supported_formats.push_back(V4L2_PIX_FMT_MJPEG);
#endif

#endif

    for (const auto& format : kSupportedFormatsAndPlanarity)
      supported_formats.push_back(format.fourcc);

    return supported_formats;
}

VizioUVCCaptureDelegate::VizioUVCCaptureDelegate(
      VizioUVCCaptureDevice* vizio_uvc,
      const std::string& deviceId)
      : vizio_uvc_(vizio_uvc),
        device_id_(deviceId),
        device_fd_(vizio_uvc),
        is_capturing_(false),
        timeout_count_(0),
        first_ref_time_(std::chrono::system_clock::from_time_t(0)) {}

void VizioUVCCaptureDelegate::AllocateAndStart(
      const uint32_t fourcc,
      const uint32_t width,
      const uint32_t height,
      const float frame_rate,
      std::shared_ptr<VizioVideoCaptureDeviceClient> client) {
    viziosdk::INFO(__file_line__ + __func__);
    client_ = client;

    device_fd_.reset(vizio_uvc_->open(device_id_.c_str(), O_RDWR));
    if (!device_fd_.is_valid()) {
      SetErrorState(VideoCaptureError::kV4L2FailedToOpenV4L2DeviceDriverFile,
                    FROM_HERE, "Failed to open V4L2 device driver file.");
      return;
    }

    ResetUserAndCameraControlsToDefault();

    v4l2_capability cap = {};
    if (!(DoIoctl(VIDIOC_QUERYCAP, &cap) == 0 &&
          ((cap.capabilities & V4L2_CAP_VIDEO_CAPTURE) &&
           !(cap.capabilities & V4L2_CAP_VIDEO_OUTPUT)))) {
      device_fd_.reset();
      SetErrorState(VideoCaptureError::kV4L2ThisIsNotAV4L2VideoCaptureDevice,
                    FROM_HERE, "This is not a V4L2 video capture device");
      return;
    }

    // Get supported video formats in preferred order. For large resolutions,
    // favour mjpeg over raw formats.
#if 0
    const std::vector<uint32_t>& desired_v4l2_formats =
        GetListOfUsableFourCcs(width > kMjpegWidth || height > kMjpegHeight);
    auto best = desired_v4l2_formats.end();

    v4l2_fmtdesc fmtdesc = {};
    fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    for (; DoIoctl(VIDIOC_ENUM_FMT, &fmtdesc) == 0; ++fmtdesc.index)
      best = std::find(desired_v4l2_formats.begin(), best, fmtdesc.pixelformat);

    if (best == desired_v4l2_formats.end()) {
      SetErrorState(VideoCaptureError::kV4L2FailedToFindASupportedCameraFormat,
                    FROM_HERE, "Failed to find a supported camera format.");
      return;
    }
#endif

    FillV4L2Format(&video_fmt_, width, height, fourcc);

    if (DoIoctl(VIDIOC_S_FMT, &video_fmt_) < 0) {
      SetErrorState(VideoCaptureError::kV4L2FailedToSetVideoCaptureFormat,
                    FROM_HERE, "Failed to set video capture format");
      return;
    }
    const VideoPixelFormat pixel_format =
        V4l2FourCcToChromiumPixelFormat(video_fmt_.fmt.pix.pixelformat);
    if (pixel_format == PIXEL_FORMAT_UNKNOWN) {
      SetErrorState(VideoCaptureError::kV4L2UnsupportedPixelFormat, FROM_HERE,
                    "Unsupported pixel format");
      return;
    }

    // Set capture framerate in the form of capture interval.
    v4l2_streamparm streamparm = {};
    streamparm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    // The following line checks that the driver knows about framerate get/set.
    if (DoIoctl(VIDIOC_G_PARM, &streamparm) >= 0) {
      // Now check if the device is able to accept a capture framerate set.
      if (streamparm.parm.capture.capability & V4L2_CAP_TIMEPERFRAME) {
        // |frame_rate| is float, approximate by a fraction.
        streamparm.parm.capture.timeperframe.numerator = kFrameRatePrecision;
        streamparm.parm.capture.timeperframe.denominator =
            (frame_rate) ? (frame_rate * kFrameRatePrecision)
                         : (kTypicalFramerate * kFrameRatePrecision);

        if (DoIoctl(VIDIOC_S_PARM, &streamparm) < 0) {
          SetErrorState(VideoCaptureError::kV4L2FailedToSetCameraFramerate,
                        FROM_HERE, "Failed to set camera framerate");
          return;
        }
      }
    }
#if 0
    if ((power_line_frequency_ == V4L2_CID_POWER_LINE_FREQUENCY_50HZ) ||
        (power_line_frequency_ == V4L2_CID_POWER_LINE_FREQUENCY_60HZ) ||
        (power_line_frequency_ == V4L2_CID_POWER_LINE_FREQUENCY_AUTO)) {
      struct v4l2_control control = {};
      control.id = V4L2_CID_POWER_LINE_FREQUENCY;
      control.value = power_line_frequency_;
      const int retval = DoIoctl(VIDIOC_S_CTRL, &control);
      if (retval != 0)
        DVLOG(1) << "Error setting power line frequency removal";
    }
#endif

    capture_format_.frame_size.SetSize(video_fmt_.fmt.pix.width, video_fmt_.fmt.pix.height);
    capture_format_.frame_rate = frame_rate;
    capture_format_.pixel_format = pixel_format;

    v4l2_requestbuffers r_buffer;
    FillV4L2RequestBuffer(&r_buffer, kNumVideoBuffers);
    if (DoIoctl(VIDIOC_REQBUFS, &r_buffer) < 0) {
      SetErrorState(VideoCaptureError::kV4L2ErrorRequestingMmapBuffers, FROM_HERE,
                    "Error requesting MMAP buffers from V4L2");
      return;
    }
    for (unsigned int i = 0; i < r_buffer.count; ++i) {
      if (!MapAndQueueBuffer(i)) {
        SetErrorState(VideoCaptureError::kV4L2AllocateBufferFailed, FROM_HERE,
                      "Allocate buffer failed");
        return;
      }
    }

    v4l2_buf_type capture_type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (DoIoctl(VIDIOC_STREAMON, &capture_type) < 0) {
      SetErrorState(VideoCaptureError::kV4L2VidiocStreamonFailed, FROM_HERE,
                    "VIDIOC_STREAMON failed");
      return;
    }
    client_->OnStarted();
    is_capturing_ = true;

    // start fetching frames from vizio_uvc.
    DoCapture();

    // Post task to start fetching frames from vizio_uvc.
//    v4l2_task_runner_->PostTask(
//        FROM_HERE, base::BindOnce(&VizioUVCCaptureDelegate::DoCapture, GetWeakPtr()));
}

void VizioUVCCaptureDelegate::StopAndDeAllocate() {
    //DCHECK(v4l2_task_runner_->BelongsToCurrentThread());
    // The order is important: stop streaming, clear |buffer_pool_|,
    // thus munmap()ing the v4l2_buffers, and then return them to the OS.
    v4l2_buf_type capture_type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (DoIoctl(VIDIOC_STREAMOFF, &capture_type) < 0) {
      SetErrorState(VideoCaptureError::kV4L2VidiocStreamoffFailed, FROM_HERE,
                    "VIDIOC_STREAMOFF failed");
      return;
    }

    buffer_tracker_pool_.clear();

    v4l2_requestbuffers r_buffer;
    FillV4L2RequestBuffer(&r_buffer, 0);
    if (DoIoctl(VIDIOC_REQBUFS, &r_buffer) < 0) {
      SetErrorState(VideoCaptureError::kV4L2FailedToVidiocReqbufsWithCount0,
                    FROM_HERE, "Failed to VIDIOC_REQBUFS with count = 0");
    }

    // At this point we can close the device.
    // This is also needed for correctly changing settings later via VIDIOC_S_FMT.
    device_fd_.reset();
    is_capturing_ = false;
    client_.reset();
}

VizioUVCCaptureDelegate::~VizioUVCCaptureDelegate() = default;

bool VizioUVCCaptureDelegate::RunIoctl(int request, void* argp) {
    int num_retries = 0;
    for (; DoIoctl(request, argp) < 0 && num_retries < kMaxIOCtrlRetries;
         ++num_retries) {
    }
    return num_retries != kMaxIOCtrlRetries;
}

int VizioUVCCaptureDelegate::DoIoctl(int request, void* argp) {
    return (vizio_uvc_->ioctl(device_fd_.get(), request, argp));
}

void VizioUVCCaptureDelegate::ResetUserAndCameraControlsToDefault() {
    // Set V4L2_CID_AUTO_WHITE_BALANCE to false first.
    v4l2_control auto_white_balance = {};
    auto_white_balance.id = V4L2_CID_AUTO_WHITE_BALANCE;
    auto_white_balance.value = false;
    if (!RunIoctl(VIDIOC_S_CTRL, &auto_white_balance))
      return;

    std::vector<struct v4l2_ext_control> special_camera_controls;
    // Set V4L2_CID_EXPOSURE_AUTO to V4L2_EXPOSURE_MANUAL.
    v4l2_ext_control auto_exposure = {};
    auto_exposure.id = V4L2_CID_EXPOSURE_AUTO;
    auto_exposure.value = V4L2_EXPOSURE_MANUAL;
    special_camera_controls.push_back(auto_exposure);
    // Set V4L2_CID_EXPOSURE_AUTO_PRIORITY to false.
    v4l2_ext_control priority_auto_exposure = {};
    priority_auto_exposure.id = V4L2_CID_EXPOSURE_AUTO_PRIORITY;
    priority_auto_exposure.value = false;
    special_camera_controls.push_back(priority_auto_exposure);
    // Set V4L2_CID_FOCUS_AUTO to false.
    v4l2_ext_control auto_focus = {};
    auto_focus.id = V4L2_CID_FOCUS_AUTO;
    auto_focus.value = false;
    special_camera_controls.push_back(auto_focus);

    struct v4l2_ext_controls ext_controls = {};
    ext_controls.ctrl_class = V4L2_CID_CAMERA_CLASS;
    ext_controls.count = special_camera_controls.size();
    ext_controls.controls = special_camera_controls.data();
//    if (DoIoctl(VIDIOC_S_EXT_CTRLS, &ext_controls) < 0)
//      DPLOG(INFO) << "VIDIOC_S_EXT_CTRLS";

//    std::vector<struct v4l2_ext_control> camera_controls;
    for (const auto& control : kControls) {
      std::vector<struct v4l2_ext_control> camera_controls;

      v4l2_queryctrl range = {};
      range.id = control.control_base | V4L2_CTRL_FLAG_NEXT_CTRL;
      while (0 == DoIoctl(VIDIOC_QUERYCTRL, &range)) {
        if (V4L2_CTRL_ID2CLASS(range.id) != V4L2_CTRL_ID2CLASS(control.class_id))
          break;
        range.id |= V4L2_CTRL_FLAG_NEXT_CTRL;

        if (IsSpecialControl(range.id & ~V4L2_CTRL_FLAG_NEXT_CTRL))
          continue;
        if (IsBlacklistedControl(range.id & ~V4L2_CTRL_FLAG_NEXT_CTRL))
          continue;

        struct v4l2_ext_control ext_control = {};
        ext_control.id = range.id & ~V4L2_CTRL_FLAG_NEXT_CTRL;
        ext_control.value = range.default_value;
        camera_controls.push_back(ext_control);
      }

      if (!camera_controls.empty()) {
        struct v4l2_ext_controls ext_controls2 = {};
        ext_controls2.ctrl_class = control.class_id;
        ext_controls2.count = camera_controls.size();
        ext_controls2.controls = camera_controls.data();
//        if (DoIoctl(VIDIOC_S_EXT_CTRLS, &ext_controls2) < 0)
//          DPLOG(INFO) << "VIDIOC_S_EXT_CTRLS";
      }
    }

    // Now set the special flags to the default values
    v4l2_queryctrl range = {};
    range.id = V4L2_CID_AUTO_WHITE_BALANCE;
    DoIoctl(VIDIOC_QUERYCTRL, &range);
    auto_white_balance.value = range.default_value;
    DoIoctl(VIDIOC_S_CTRL, &auto_white_balance);

    special_camera_controls.clear();
    memset(&range, 0, sizeof(range));
    range.id = V4L2_CID_EXPOSURE_AUTO;
    DoIoctl(VIDIOC_QUERYCTRL, &range);
    auto_exposure.value = range.default_value;
    special_camera_controls.push_back(auto_exposure);

    memset(&range, 0, sizeof(range));
    range.id = V4L2_CID_EXPOSURE_AUTO_PRIORITY;
    DoIoctl(VIDIOC_QUERYCTRL, &range);
    priority_auto_exposure.value = range.default_value;
    special_camera_controls.push_back(priority_auto_exposure);

    memset(&range, 0, sizeof(range));
    range.id = V4L2_CID_FOCUS_AUTO;
    DoIoctl(VIDIOC_QUERYCTRL, &range);
    auto_focus.value = range.default_value;
    special_camera_controls.push_back(auto_focus);

    memset(&ext_controls, 0, sizeof(ext_controls));
    ext_controls.ctrl_class = V4L2_CID_CAMERA_CLASS;
    ext_controls.count = special_camera_controls.size();
    ext_controls.controls = special_camera_controls.data();
//    if (DoIoctl(VIDIOC_S_EXT_CTRLS, &ext_controls) < 0)
//      DPLOG(INFO) << "VIDIOC_S_EXT_CTRLS";
}

bool VizioUVCCaptureDelegate::MapAndQueueBuffer(int index) {
    v4l2_buffer buffer;
    FillV4L2Buffer(&buffer, index);

    if (DoIoctl(VIDIOC_QUERYBUF, &buffer) < 0) {
//      DLOG(ERROR) << "Error querying status of a MMAP V4L2 buffer";
      return false;
    }

    const auto buffer_tracker = std::make_shared<BufferTracker>(vizio_uvc_);
    if (!buffer_tracker->Init(device_fd_.get(), buffer)) {
//      DLOG(ERROR) << "Error creating BufferTracker";
      return false;
    }
    buffer_tracker_pool_.push_back(buffer_tracker);

    // Enqueue the buffer in the drivers incoming queue.
    if (DoIoctl(VIDIOC_QBUF, &buffer) < 0) {
//      DLOG(ERROR) << "Error enqueuing a V4L2 buffer back into the driver";
      return false;
    }
    return true;
}

void VizioUVCCaptureDelegate::DoCapture() {
    viziosdk::INFO(__file_line__ + __func__);
//    DCHECK(v4l2_task_runner_->BelongsToCurrentThread());
    if (!is_capturing_)
      return;

    while (true)
    {
        pollfd device_pfd = {};
        device_pfd.fd = device_fd_.get();
        device_pfd.events = POLLIN;

        const int result =
            (vizio_uvc_->poll(&device_pfd, 1, kCaptureTimeoutMs));
        if (result < 0) {
          SetErrorState(VideoCaptureError::kV4L2PollFailed, FROM_HERE, "Poll failed");
          return;
        }

        // Check if poll() timed out; track the amount of times it did in a row and
        // throw an error if it times out too many times.
        if (result == 0) {
          timeout_count_++;
          if (timeout_count_ >= kContinuousTimeoutLimit) {
            SetErrorState(
                VideoCaptureError::kV4L2MultipleContinuousTimeoutsWhileReadPolling,
                FROM_HERE, "Multiple continuous timeouts while read-polling.");
            timeout_count_ = 0;
            return;
          }
        } else {
          timeout_count_ = 0;
        }

        // Deenqueue, send and reenqueue a buffer if the driver has filled one in.
        if (device_pfd.revents & POLLIN) {
          v4l2_buffer buffer;
          FillV4L2Buffer(&buffer, 0);

          if (DoIoctl(VIDIOC_DQBUF, &buffer) < 0) {
            SetErrorState(VideoCaptureError::kV4L2FailedToDequeueCaptureBuffer,
                          FROM_HERE, "Failed to dequeue capture buffer");
            return;
          }

          buffer_tracker_pool_[buffer.index]->set_payload_size(buffer.bytesused);
          const std::shared_ptr<BufferTracker>& buffer_tracker =
              buffer_tracker_pool_[buffer.index];

          // There's a wide-spread issue where the kernel does not report accurate,
          // monotonically-increasing timestamps in the v4l2_buffer::timestamp
          // field (goo.gl/Nlfamz).
          // Until this issue is fixed, just use the reference clock as a source of
          // media timestamps.
          const viziosdk::high_res_time_point now = std::chrono::system_clock::now();
          if (first_ref_time_ == std::chrono::system_clock::from_time_t(0))
            first_ref_time_ = now;
          const auto timedelta = now - first_ref_time_;

#ifdef V4L2_BUF_FLAG_ERROR
          bool buf_error_flag_set = buffer.flags & V4L2_BUF_FLAG_ERROR;
#else
          bool buf_error_flag_set = false;
#endif
          if (buf_error_flag_set) {
#ifdef V4L2_BUF_FLAG_ERROR
//        LOG(ERROR) << "Dequeued vizio_uvc buffer contains corrupted data ("
//                   << buffer.bytesused << " bytes).";
            buffer.bytesused = 0;
            client_->OnFrameDropped(
                VideoCaptureFrameDropReason::kV4L2BufferErrorFlagWasSet);
#endif
          // TODO filter all supported formats here
          } else if (video_fmt_.fmt.pix.pixelformat != V4L2_PIX_FMT_H264 && buffer.bytesused < capture_format_.ImageAllocationSize()) {
//        LOG(ERROR) << "Dequeued vizio_uvc buffer contains invalid length ("
//                   << buffer.bytesused << " bytes).";
            buffer.bytesused = 0;
            client_->OnFrameDropped(
                VideoCaptureFrameDropReason::kV4L2InvalidNumberOfBytesInBuffer);
          } else {

           client_->OnIncomingCapturedData(
                buffer_tracker->start(), buffer_tracker->payload_size(),
                capture_format_, rotation_, now, timedelta);
          }

          if (DoIoctl(VIDIOC_QBUF, &buffer) < 0) {
            SetErrorState(VideoCaptureError::kV4L2FailedToEnqueueCaptureBuffer,
                          FROM_HERE, "Failed to enqueue capture buffer");
            return;
          }
        }
    }

//    v4l2_task_runner_->PostTask(
//        FROM_HERE, base::BindOnce(&VizioUVCCaptureDelegate::DoCapture, GetWeakPtr()));
}

void VizioUVCCaptureDelegate::SetErrorState(VideoCaptureError error,
                                          const std::string& from_here,
                                          const std::string& reason) {
//    DCHECK(v4l2_task_runner_->BelongsToCurrentThread());
    is_capturing_ = false;
    client_->OnError(error, from_here, reason);
}

VizioUVCCaptureDelegate::BufferTracker::BufferTracker(VizioUVCCaptureDevice* vizio_uvc)
      : v4l2_(vizio_uvc) {}

VizioUVCCaptureDelegate::BufferTracker::~BufferTracker() {
    if (!start_)
      return;
    /*const int result =*/ v4l2_->munmap(start_, length_);
//    PLOG_IF(ERROR, result < 0) << "Error munmap()ing V4L2 buffer";
}

bool VizioUVCCaptureDelegate::BufferTracker::Init(int fd,
                                                const v4l2_buffer& buffer) {
    constexpr int kFlags = PROT_READ | PROT_WRITE;
    void* const start = v4l2_->mmap(nullptr, buffer.length, kFlags, MAP_SHARED,
                                    fd, buffer.m.offset);
    if (start == MAP_FAILED) {
//      DLOG(ERROR) << "Error mmap()ing a V4L2 buffer into userspace";
      return false;
    }

    viziosdk::DEBUG(__file_line__ + __func__ + std::to_string(std::hash<std::thread::id>()(std::this_thread::get_id())));

    start_ = static_cast<uint8_t*>(start);
    length_ = buffer.length;
    payload_size_ = 0;
    return true;
}

}}} // namespace viziosdk::media::capture
