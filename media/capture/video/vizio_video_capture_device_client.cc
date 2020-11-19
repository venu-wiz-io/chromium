//
// vizio_video_capture_device_client.cc
// © 2020 Vizio Services, All Rights Reserved
//

#include <iostream>
#include <algorithm>
#include <memory>
#include <utility>

#include "vizio_video_capture_device_client.h"
#include "vizio_sdk_logger.h"
#include "vizio_media_device_sdk_api_impl.h"
#include "vizio_video_size.h"
#if 0
#include "media/base/video_frame.h"
#include "media/capture/video/scoped_buffer_pool_reservation.h"
#include "media/capture/video/video_capture_buffer_handle.h"
#include "media/capture/video/video_capture_buffer_pool.h"
#include "media/capture/video/video_frame_receiver.h"
#include "media/capture/video_capture_types.h"
#include "third_party/libyuv/include/libyuv.h"
#endif


namespace viziosdk { namespace media { namespace capture {

class VideoCaptureBufferHandle;

class BufferPoolBufferHandleProvider {
   public:
    BufferPoolBufferHandleProvider(
        std::shared_ptr<VizioVideoCaptureBufferPool> buffer_pool,
        int buffer_id)
        : buffer_pool_((buffer_pool)) /*, buffer_id_(buffer_id) */ {}

#if 0
    // Implementation of HandleProvider:
    mojo::ScopedSharedBufferHandle GetHandleForInterProcessTransit(
        bool read_only) override {
      return buffer_pool_->GetHandleForInterProcessTransit(buffer_id_, read_only);
    }
    base::SharedMemoryHandle GetNonOwnedSharedMemoryHandleForLegacyIPC()
        override {
      return buffer_pool_->GetNonOwnedSharedMemoryHandleForLegacyIPC(buffer_id_);
    }
    std::unique_ptr<VideoCaptureBufferHandle> GetHandleForInProcessAccess() {
// TODO
//      return std::make_unique<VideoCaptureBufferHandle>();
      return buffer_pool_->GetHandleForInProcessAccess(buffer_id_);
    }
#endif

   private:
    const std::shared_ptr<VizioVideoCaptureBufferPool> buffer_pool_;
//    const int buffer_id_;
};

VizioVideoCaptureDeviceClient::VizioVideoCaptureDeviceClient(
/*      std::unique_ptr<VideoFrameReceiver> receiver,
      std::shared_ptr<VizioVideoCaptureBufferPool> buffer_pool */)
      : /* receiver_(std::move(receiver)),
        buffer_pool_(std::move(buffer_pool)), */
        last_captured_pixel_format_(PIXEL_FORMAT_UNKNOWN)
{
//    start_video = ATOMIC_FLAG_INIT;
    start_video.clear();
}

VizioVideoCaptureDeviceClient::~VizioVideoCaptureDeviceClient() {
#if 0
    for (int buffer_id : buffer_ids_known_by_receiver_)
      receiver_->OnBufferRetired(buffer_id);
#endif
}

void VizioVideoCaptureDeviceClient::StartVideo()
{
    start_video.test_and_set();
}

void VizioVideoCaptureDeviceClient::StopVideo()
{
    start_video.clear();
}

void VizioVideoCaptureDeviceClient::SetReceiver(std::string device_id, VideoStreamCB* video_receiver)
{
    video_receiver_ = std::move(video_receiver);
}

void VizioVideoCaptureDeviceClient::OnIncomingCapturedData(
      const uint8_t* data,
      int length,
      const VideoCaptureFormat& format,
      int rotation,
      const viziosdk::high_res_time_point reference_time,
      const viziosdk::duration_nanos timestamp,
      int frame_feedback_id) {
//    viziosdk::INFO(__file_line__ + __func__);
    viziosdk::DEBUG(__file_line__ + __func__ + std::to_string(std::hash<std::thread::id>()(std::this_thread::get_id())));

    (*video_receiver_)(data, length, format, reference_time, timestamp);

//    DFAKE_SCOPED_RECURSIVE_LOCK(call_from_producer_);
//    TRACE_EVENT0("media", "VizioVideoCaptureDeviceClient::OnIncomingCapturedData");

#if 0
    VideoPixelFormat pix_format = (format.pixel_format == PIXEL_FORMAT_H264)?PIXEL_FORMAT_H264:PIXEL_FORMAT_I420;

    if (last_captured_pixel_format_ != format.pixel_format) {
      OnLog("Pixel format: " + VideoPixelFormatToString(format.pixel_format));
      last_captured_pixel_format_ = format.pixel_format;

      if (format.pixel_format == PIXEL_FORMAT_MJPEG &&
          optional_jpeg_decoder_factory_callback_) {
        external_jpeg_decoder_ =
            std::move(optional_jpeg_decoder_factory_callback_).Run();
        DCHECK(external_jpeg_decoder_);
        external_jpeg_decoder_->Initialize();
      }
    }

    if (!format.IsValid()) {
      receiver_->OnFrameDropped(
          VideoCaptureFrameDropReason::kDeviceClientFrameHasInvalidFormat);
      return;
    }

    if (format.pixel_format == PIXEL_FORMAT_Y16) {
      return OnIncomingCapturedY16Data(data, length, format, reference_time,
                                       timestamp, frame_feedback_id);
    }

    // |chopped_{width,height} and |new_unrotated_{width,height}| are the lowest
    // bit decomposition of {width, height}, grabbing the odd and even parts.
    const int chopped_width = format.frame_size.width() & 1;
    const int chopped_height = format.frame_size.height() & 1;
    const int new_unrotated_width = format.frame_size.width() & ~1;
    const int new_unrotated_height = format.frame_size.height() & ~1;

    int destination_width = new_unrotated_width;
    int destination_height = new_unrotated_height;
    if (rotation == 90 || rotation == 270)
      std::swap(destination_width, destination_height);

    libyuv::RotationMode rotation_mode = TranslateRotation(rotation);

    const Size dimensions(destination_width, destination_height);
    Buffer buffer;
    auto reservation_result_code = ReserveOutputBuffer(
        dimensions, pix_format, frame_feedback_id, &buffer);
    if (reservation_result_code != ReserveResult::kSucceeded) {
      receiver_->OnFrameDropped(
          ConvertReservationFailureToFrameDropReason(reservation_result_code));
      return;
    }

    DCHECK(dimensions.height());
    DCHECK(dimensions.width());

    uint8_t* y_plane_data;
    uint8_t* u_plane_data;
    uint8_t* v_plane_data;
    int yplane_stride, uv_plane_stride;
    GetI420BufferAccess(buffer, dimensions, &y_plane_data, &u_plane_data,
                        &v_plane_data, &yplane_stride, &uv_plane_stride);

    int crop_x = 0;
    int crop_y = 0;
    libyuv::FourCC origin_colorspace = libyuv::FOURCC_ANY;

    bool flip = false;
    switch (format.pixel_format) {
      case PIXEL_FORMAT_UNKNOWN:  // Color format not set.
        break;
      case PIXEL_FORMAT_I420:
        DCHECK(!chopped_width && !chopped_height);
        origin_colorspace = libyuv::FOURCC_I420;
        break;
      case PIXEL_FORMAT_YV12:
        DCHECK(!chopped_width && !chopped_height);
        origin_colorspace = libyuv::FOURCC_YV12;
        break;
      case PIXEL_FORMAT_NV12:
        DCHECK(!chopped_width && !chopped_height);
        origin_colorspace = libyuv::FOURCC_NV12;
        break;
      case PIXEL_FORMAT_NV21:
        DCHECK(!chopped_width && !chopped_height);
        origin_colorspace = libyuv::FOURCC_NV21;
        break;
      case PIXEL_FORMAT_YUY2:
        DCHECK(!chopped_width && !chopped_height);
        origin_colorspace = libyuv::FOURCC_YUY2;
        break;
      case PIXEL_FORMAT_UYVY:
        DCHECK(!chopped_width && !chopped_height);
        origin_colorspace = libyuv::FOURCC_UYVY;
        break;
      case PIXEL_FORMAT_RGB24:
// Linux RGB24 defines red at lowest byte address,
// see http://linuxtv.org/downloads/v4l-dvb-apis/packed-rgb.html.
// Windows RGB24 defines blue at lowest byte,
// see https://msdn.microsoft.com/en-us/library/windows/desktop/dd407253
#if defined(OS_LINUX)
        origin_colorspace = libyuv::FOURCC_RAW;
#elif defined(OS_WIN)
        origin_colorspace = libyuv::FOURCC_24BG;
#else
        NOTREACHED() << "RGB24 is only available in Linux and Windows platforms";
#endif
#if defined(OS_WIN)
        // TODO(wjia): Currently, for RGB24 on WIN, capture device always passes
        // in positive src_width and src_height. Remove this hardcoded value when
        // negative src_height is supported. The negative src_height indicates
        // that vertical flipping is needed.
        flip = true;
#endif
        break;
      case PIXEL_FORMAT_RGB32:
// Fallback to PIXEL_FORMAT_ARGB setting |flip| in Windows
// platforms.
#if defined(OS_WIN)
        flip = true;
        FALLTHROUGH;
#endif
      case PIXEL_FORMAT_ARGB:
        origin_colorspace = libyuv::FOURCC_ARGB;
        break;
      case PIXEL_FORMAT_MJPEG:
        origin_colorspace = libyuv::FOURCC_MJPG;
        break;
      case PIXEL_FORMAT_H264:
        origin_colorspace = libyuv::FOURCC_H264;
        break;
      default:
        NOTREACHED();
    }

    if(format.pixel_format == PIXEL_FORMAT_H264) {
      memcpy(y_plane_data, data, length);
    } else if (libyuv::ConvertToI420(
            data, length, y_plane_data, yplane_stride, u_plane_data,
            uv_plane_stride, v_plane_data, uv_plane_stride, crop_x, crop_y,
            format.frame_size.width(),
            (flip ? -1 : 1) * format.frame_size.height(), new_unrotated_width,
            new_unrotated_height, rotation_mode, origin_colorspace) != 0) {
      DLOG(WARNING) << "Failed to convert buffer's pixel format to I420 from "
                    << VideoPixelFormatToString(format.pixel_format);
      receiver_->OnFrameDropped(
          VideoCaptureFrameDropReason::kDeviceClientLibyuvConvertToI420Failed);
      return;
    }

    const VideoCaptureFormat output_format =
        VideoCaptureFormat(dimensions, format.frame_rate, 
        pix_format);
    OnIncomingCapturedBuffer(std::move(buffer), output_format, reference_time,
                             timestamp);
#endif
}

#if 0
VideoCaptureDevice::Client::Buffer VizioVideoCaptureDeviceClient::MakeBufferStruct(
      scoped_refptr<VizioVideoCaptureBufferPool> buffer_pool,
      int buffer_id,
      int frame_feedback_id) {
    return Buffer(
        buffer_id, frame_feedback_id,
        std::make_unique<BufferPoolBufferHandleProvider>(buffer_pool, buffer_id),
        std::make_unique<ScopedBufferPoolReservation<ProducerReleaseTraits>>(
            buffer_pool, buffer_id));
}
#endif

#if 0
VizioVideoCaptureDeviceClient::ReserveResult
VizioVideoCaptureDeviceClient::ReserveOutputBuffer(const Size& frame_size,
                                                VideoPixelFormat pixel_format,
                                                int frame_feedback_id,
                                                Buffer* buffer) {
#if 0
    DFAKE_SCOPED_RECURSIVE_LOCK(call_from_producer_);
    DCHECK_GT(frame_size.width(), 0);
    DCHECK_GT(frame_size.height(), 0);
    DCHECK(IsFormatSupported(pixel_format));
#endif

    int buffer_id_to_drop = VizioVideoCaptureBufferPool::kInvalidId;
    int buffer_id = VizioVideoCaptureBufferPool::kInvalidId;
    auto reservation_result_code = buffer_pool_->ReserveForProducer(
        frame_size, pixel_format, nullptr, frame_feedback_id, &buffer_id,
        &buffer_id_to_drop);
    if (buffer_id_to_drop != VizioVideoCaptureBufferPool::kInvalidId) {
      // |buffer_pool_| has decided to release a buffer. Notify receiver in case
      // the buffer has already been shared with it.
      auto entry_iter =
          std::find(buffer_ids_known_by_receiver_.begin(),
                    buffer_ids_known_by_receiver_.end(), buffer_id_to_drop);
      if (entry_iter != buffer_ids_known_by_receiver_.end()) {
        buffer_ids_known_by_receiver_.erase(entry_iter);
        receiver_->OnBufferRetired(buffer_id_to_drop);
      }
    }
    if (reservation_result_code != ReserveResult::kSucceeded)
      return reservation_result_code;

    DCHECK_NE(VizioVideoCaptureBufferPool::kInvalidId, buffer_id);

    if (!base::ContainsValue(buffer_ids_known_by_receiver_, buffer_id)) {
      media::mojom::VideoBufferHandlePtr buffer_handle =
          media::mojom::VideoBufferHandle::New();
      switch (target_buffer_type_) {
        case VideoCaptureBufferType::kSharedMemory:
          buffer_handle->set_shared_buffer_handle(
              buffer_pool_->GetHandleForInterProcessTransit(buffer_id,
                                                            true /*read_only*/));
          break;
        case VideoCaptureBufferType::kSharedMemoryViaRawFileDescriptor:
          buffer_handle->set_shared_memory_via_raw_file_descriptor(
              buffer_pool_->CreateSharedMemoryViaRawFileDescriptorStruct(
                  buffer_id));
          break;
        case VideoCaptureBufferType::kMailboxHolder:
          NOTREACHED();
          break;
      }
      receiver_->OnNewBuffer(buffer_id, std::move(buffer_handle));
      buffer_ids_known_by_receiver_.push_back(buffer_id);
    }

    *buffer = MakeBufferStruct(buffer_pool_, buffer_id, frame_feedback_id);
    return ReserveResult::kSucceeded;
}
#endif

#if 0
void VizioVideoCaptureDeviceClient::OnIncomingCapturedBuffer(
      Buffer buffer,
      const VideoCaptureFormat& format,
      base::TimeTicks reference_time,
      base::TimeDelta timestamp,
      gfx::Rect visible_rect,
      const VideoFrameMetadata& additional_metadata) {
    DFAKE_SCOPED_RECURSIVE_LOCK(call_from_producer_);

    VideoFrameMetadata metadata;
    metadata.MergeMetadataFrom(&additional_metadata);
    metadata.SetDouble(VideoFrameMetadata::FRAME_RATE, format.frame_rate);
    metadata.SetTimeTicks(VideoFrameMetadata::REFERENCE_TIME, reference_time);

    mojom::VideoFrameInfoPtr info = mojom::VideoFrameInfo::New();
    info->timestamp = timestamp;
    info->pixel_format = format.pixel_format;
    info->coded_size = format.frame_size;
    info->visible_rect = visible_rect;
    info->metadata = metadata.GetInternalValues().Clone();

    buffer_pool_->HoldForConsumers(buffer.id, 1);
    receiver_->OnFrameReadyInBuffer(
        buffer.id, buffer.frame_feedback_id,
        std::make_unique<ScopedBufferPoolReservation<ConsumerReleaseTraits>>(
            buffer_pool_, buffer.id),
        std::move(info));
}
#endif

void VizioVideoCaptureDeviceClient::OnError(VideoCaptureError error,
                                         const std::string& from_here,
                                         const std::string& reason) {
#if 1
    const std::string log_message = std::string("error@ ") + from_here + " " + reason;
    std::cout << log_message << std::endl;
// TODO
#else
    const std::string log_message = base::StringPrintf(
        "error@ %s, %s, OS message: %s", from_here.ToString().c_str(),
        reason.c_str(),
        logging::SystemErrorCodeToString(logging::GetLastSystemErrorCode())
            .c_str());
    DLOG(ERROR) << log_message;
    OnLog(log_message);
    receiver_->OnError(error);
#endif
}

void VizioVideoCaptureDeviceClient::OnFrameDropped(
      VideoCaptureFrameDropReason reason) {
//    receiver_->OnFrameDropped(reason);
}

void VizioVideoCaptureDeviceClient::OnLog(const std::string& message) {
//    receiver_->OnLog(message);
}

void VizioVideoCaptureDeviceClient::OnStarted() {
//    receiver_->OnStarted();
}

double VizioVideoCaptureDeviceClient::GetBufferPoolUtilization() const {
    return 0.0;
//    return buffer_pool_->GetBufferPoolUtilization();
}

}}} // namespace viziosdk::media::capture
