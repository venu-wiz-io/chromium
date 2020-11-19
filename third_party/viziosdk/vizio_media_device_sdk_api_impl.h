//
// vizio_media_device_sdk_api_impl.h
// © 2020 Vizio Services, All Rights Reserved
//


#pragma once

#include <map>

#include "vizio_media_device_sdk_api.h"
#include "vizio_video_capture_device_factory.h"
#include "vizio_video_capture_device.h"
#include "vizio_video_capture_device_client.h"

namespace viziosdk { namespace media { namespace capture {

class MediaDeviceSDKAPIImpl : public MediaDeviceSDKAPI
{
public:
    MediaDeviceSDKAPIImpl();
    ~MediaDeviceSDKAPIImpl() override;

    //
    // For camera devices
    //

    // GetCameraDevices
    //    This API call is called by the app to get all the camera devices available on the TV. 
    // 	  This call returns an array of camera devices. For each camera, the following properties are returned:
    //        Device ID – unique device ID
    //        Group ID – ID of the group this device is part of, if any
    //        Name – name of the device 

    CameraDeviceInfoVec   GetCameraDevices() override;

    // OpenCamera
    //    This API call is called by the app to turn on a camera device identified by the passed argument.

    Status   OpenCamera(const std::string& id, const CameraParameters& params /*,
                         const VideoStreamCB* video_stream_callback*/) override;

    // CloseCamera
    //    This API call is called by the app to turn off a camera device identified by the passed argument.

    Status   CloseCamera(const std::string& id) override;

    // StartVideoCapture
    //    This API call is called by the app to register a call back function to receive video stream 
    //    from a camera device that has been successfully opened by OpenCamera call.
    //    Prior to calling StartVideoCapture, the app must have called SetFrameRateAndResolution() and SetCodec().
    //    media::VideoFrame is defined in media/base/video_frame.h

    void StartVideoCapture(
        const std::string& id,
        VideoStreamCB* video_stream_callback) override;

    // StopVideoCapture
    //    This API call is called by the app to stop video capture on a given device.

    void StopVideoCapture(const std::string& id) override;

    // TakePhoto
    //    This API call is called by the app to capture a photo

// TODO
#if 0
    void TakePhoto(
        const std::string& id,
        const TakePhotoCB& take_photo_callback) override;
#endif

    //
    // For microphones
    // 
 
    // GetMicrophoneDevices
    //    This API call is called by the app to get all the microphone devices available on the TV. 
    //    This call returns an array of microphone devices. For each microphone, the following properties are returned:
    //        Device ID – unique device ID
    //        Group ID – ID of the group this device is part of, if any
    //        Name – name of the device 

// TODO
#if 0
    MicrophoneDeviceInfoVec   GetMicrophoneDevices() override;

    // OpenMic
    //    This API call is called by the app to turn on a microphone device identified by the passed argument.

    Status   OpenMic(const std::string& id) override;

    // CloseMic
    //    This API call is called by the app to turn off a microphone device identified by the passed argument.

    Status   CloseMic(const std::string& id) override;

    // StartAudioCapture
    //    This API call is called by the app to register a call back function to receive audio stream(s) 
    //    from one or more microphone device(s) that has been successfully opened by OpenCamera call.
    //    Prior to calling StartAudioCapture, the app must have called SetMicParameters().
    //    media::AudioInputStream is defined in media/audio/audio_io.h

    void StartAudioCapture(
        const std::string& id,
        const AudioStreamCB& audio_stream_callback) override;

    // StopAudioCapture
    //    This API call is called by the app to stop audio capture on a given device.

    void StopAudioCapture(const std::string& id) override;

    // SetMicParameters
    //    This API call is called by the app to set microphone parameters. 
    //    These parameters include, among others (see media::AudioParameters in media/base/audio_parameters.h): 
    //        Format
    //        EchoCancelation
    //        NoiseReduction
    //        Automatic Gain Control

    Status   SetMicParameters(const std::string& id, const media::AudioParameters& parameters) override;
#endif

private:
    std::unique_ptr<VizioVideoCaptureDeviceFactory>                         capDevFactory;
#if 1
    std::unique_ptr<VizioVideoCaptureDevice>         capDev;
    std::shared_ptr<VizioVideoCaptureDeviceClient>   capDevClient;
#else
    std::map<std::string, std::shared_ptr<VizioVideoCaptureDevice>>         capDevMap;
    std::map<std::string, std::shared_ptr<VizioVideoCaptureDeviceClient>>   capDevClientMap;
#endif

#if 0
    std::vector<::media::VideoCaptureDeviceInfo>                 devicesInfo;
    std::unique_ptr<::media::VideoCaptureSystemImpl>             videoCapSystem;
    std::unique_ptr<::media::VideoCaptureDeviceLinux>            capDev;
    std::unique_ptr<::media::VideoCaptureDeviceFactoryLinux>     capDevFactory;
    std::unique_ptr<::media::VideoFrameReceiver>                 videoFrameReceiver;
    ::scoped_refptr<::media::VideoCaptureBufferPool>             videoBufferPool;
    ::media::VideoCaptureDeviceClient                            capDevClient;
#endif

    void BuildVideoCaptureParams(const CameraParameters& params);
}; 


#if 0
class SDKAPIVideoFrameReceiver : public ::media::VideoFrameReceiver
{
    void OnNewBuffer(
        int32_t buffer_id,
        ::media::mojom::VideoBufferHandlePtr buffer_handle) override {}
  
    // Tells the VideoFrameReceiver that a new frame is ready for consumption
    // in the buffer with id |buffer_id| and allows it to read the data from
    // the buffer. The producer guarantees that the buffer and its contents stay
    // alive and unchanged until VideoFrameReceiver releases the given
    // |buffer_read_permission|.
    void OnFrameReadyInBuffer(
        int buffer_id,
        int frame_feedback_id,
        std::unique_ptr<
            ::media::VideoCaptureDevice::Client::Buffer::ScopedAccessPermission>
            buffer_read_permission,
        mojom::VideoFrameInfoPtr frame_info) override;
  
    // Tells the VideoFrameReceiver that the producer is no longer going to use
    // the buffer with id |buffer_id| for frame delivery. This may be called even
    // while the receiver is still holding |buffer_read_permission| from a call to
    // OnFrameReadInBuffer() for the same buffer. In that case, it means that the
    // caller is asking the VideoFrameReceiver to release the read permission and
    // buffer handle at its earliest convenience.
    // After this call, a producer may immediately reuse the retired |buffer_id|
    // with a new buffer via a call to OnNewBufferHandle().
    void OnBufferRetired(int buffer_id) override {}
  
    void OnError(::media::VideoCaptureError error) override {}
    void OnFrameDropped(::media::VideoCaptureFrameDropReason reason) override {}
    void OnLog(const std::string& message) override {}
    void OnStarted() override {}
    void OnStartedUsingGpuDecode() override {}
};
#endif

}}} // namespace viziosdk::media::capture
