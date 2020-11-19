//
// vizio_video_capture_device_client.h
// © 2020 Vizio Services, All Rights Reserved
//


#pragma once


#include <stddef.h>
#include <stdint.h>
#include <memory>
#include <vector>
#include <functional>
#include <atomic>

//#include "vizio_video_capture_buffer_pool.h"
#include "vizio_video_capture_types.h"
#include "vizio_video_size.h"
#include "vizio_camera_device_info.h"


namespace viziosdk { namespace media { namespace capture {

#if 1
struct VizioVideoCaptureBufferPool {
// TODO
};
#endif

// TODO
using VideoFrameReceiver = std::function<void(void)>;


// Implementation of UVC to API client path that uses a buffer pool
// for consumption by a given VideoFrameReceiver. 

class VizioVideoCaptureDeviceClient {
public:
    VizioVideoCaptureDeviceClient(
/*        std::unique_ptr<VideoFrameReceiver> receiver,
        std::shared_ptr<VizioVideoCaptureBufferPool> buffer_pool*/ );
    ~VizioVideoCaptureDeviceClient();

    void StartVideo();
    void StopVideo();
    void SetReceiver(std::string device_id, VideoStreamCB* video_receiver);


      // TODO buffer or raw data interface
    void OnIncomingCapturedData(const uint8_t* data,
                                int length,
                                const VideoCaptureFormat& frame_format,
                                int clockwise_rotation,
                                const viziosdk::high_res_time_point reference_time,
                                const viziosdk::duration_nanos timedelta,
                                int frame_feedback_id = 0);
// TODO
#if 0
    void OnIncomingCapturedBuffer(Buffer buffer,
                                  const VideoCaptureFormat& format,
                                  uint32_t reference_time,
                                  uint64_t timestamp);
    static Buffer MakeBufferStruct(
        scoped_refptr<VizioVideoCaptureBufferPool> buffer_pool,
        int buffer_id,
        int frame_feedback_id);
    ReserveResult ReserveOutputBuffer(const Size& dimensions,
                                      VideoPixelFormat format,
                                      int frame_feedback_id,
                                      Buffer* buffer);
#endif

    void OnError(VideoCaptureError error,
                 const std::string& from_here,
                 const std::string& reason);
    void OnFrameDropped(VideoCaptureFrameDropReason reason);
    void OnLog(const std::string& message);
    void OnStarted();
    double GetBufferPoolUtilization() const;

    // Struct bundling several parameters being passed between a
    // VideoCaptureDevice and its VideoCaptureDevice::Client.
    struct Buffer {
     public:
      // Destructor-only interface for encapsulating scoped access permission to
      // a Buffer.
      class ScopedAccessPermission {
       public:
        virtual ~ScopedAccessPermission() {}
      };

      class HandleProvider {
       public:
        virtual ~HandleProvider() {}
#if 0
        virtual mojo::ScopedSharedBufferHandle GetHandleForInterProcessTransit(
            bool read_only) = 0;
        virtual base::SharedMemoryHandle
        GetNonOwnedSharedMemoryHandleForLegacyIPC() = 0;
        virtual std::unique_ptr<VideoCaptureBufferHandle>
        GetHandleForInProcessAccess() = 0;
#endif
      };

      Buffer();
      Buffer(int buffer_id,
             int frame_feedback_id,
             std::unique_ptr<HandleProvider> handle_provider,
             std::unique_ptr<ScopedAccessPermission> access_permission);
      ~Buffer();
      Buffer(Buffer&& other);
      Buffer& operator=(Buffer&& other);

      int id;
      int frame_feedback_id;
      std::unique_ptr<HandleProvider> handle_provider;
      std::unique_ptr<ScopedAccessPermission> access_permission;
    };

    // Result code for calls to ReserveOutputBuffer()
    enum class ReserveResult {
      kSucceeded,
      kMaxBufferCountExceeded,
      kAllocationFailed
    };

private:
    // The receiver to which we post events.
#if 0
    const std::unique_ptr<VideoFrameReceiver> receiver_;
    std::vector<int> buffer_ids_known_by_receiver_;
#endif

    // The pool of shared-memory buffers used for capturing.
    const std::shared_ptr<VizioVideoCaptureBufferPool> buffer_pool_;

    VideoPixelFormat last_captured_pixel_format_;

    std::atomic_flag start_video;
    VideoStreamCB* video_receiver_;
};

}}} // namespace viziosdk::media::capture

