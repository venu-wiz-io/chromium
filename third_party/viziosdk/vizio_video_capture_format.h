//
// vizio_camera_device_info.h
// © 2020 Vizio Services, All Rights Reserved
//

#pragma once

#include "vizio_video_types.h"
#include "vizio_video_size.h"
#include "vizio_video_capture_types.h"

namespace viziosdk { namespace media { namespace capture {

#if 0
struct VideoCaptureFormat
{
    uint32_t width;
    uint32_t height;
    double frame_rate;
    VideoPixelFormat pixel_format;
};
#endif

struct VizioVideoFrame
{
};


// Video capture format specification.
// This class is used by the video capture device to specify the format of every
// frame captured and returned to a client. It is also used to specify a
// supported capture format by a device.
struct VideoCaptureFormat {
  VideoCaptureFormat() {}
  VideoCaptureFormat(const Size& frame_size,
                     float frame_rate,
                     VideoPixelFormat pixel_format);

  static std::string ToString(const VideoCaptureFormat& format);

  // Compares the priority of the pixel formats. Returns true if |lhs| is the
  // preferred pixel format in comparison with |rhs|. Returns false otherwise.
  static bool ComparePixelFormatPreference(const VideoPixelFormat& lhs,
                                           const VideoPixelFormat& rhs);

  // Returns the required buffer size to hold an image of a given
  // VideoCaptureFormat with no padding and tightly packed.
  size_t ImageAllocationSize() const;

  // Checks that all values are in the expected range. All limits are specified
  // in media::Limits.
  bool IsValid() const;

  bool operator==(const VideoCaptureFormat& other) const {
    return frame_size == other.frame_size && frame_rate == other.frame_rate &&
           pixel_format == other.pixel_format;
  }

  Size frame_size;
  float frame_rate;
  VideoPixelFormat pixel_format;
};

typedef std::vector<VideoCaptureFormat> VideoCaptureFormats;

// Parameters for starting video capture.
// This class is used by the client of a video capture device to specify the
// format of frames in which the client would like to have captured frames
// returned.
struct VideoCaptureParams {
  // Result struct for SuggestContraints() method.
  struct SuggestedConstraints {
    Size min_frame_size;
    Size max_frame_size;
    bool fixed_aspect_ratio;
  };

  VideoCaptureParams();

  // Returns true if requested_format.IsValid() and all other values are within
  // their expected ranges.
  bool IsValid() const;

  // Computes and returns suggested capture constraints based on the requested
  // format and resolution change policy: minimum resolution, maximum
  // resolution, and whether a fixed aspect ratio is required.
  SuggestedConstraints SuggestConstraints() const;

  // Requests a resolution and format at which the capture will occur.
  VideoCaptureFormat requested_format;

  VideoCaptureBufferType buffer_type;

  // Flag indicating if face detection should be enabled. This is for
  // allowing the driver to apply appropriate settings for optimal
  // exposures around the face area. Currently only applicable on
  // Android platform with Camera2 driver support.
  bool enable_face_detection;
};

}}} // namespace viziosdk::media::capture
