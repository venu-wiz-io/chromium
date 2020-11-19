//
// vizio_media_device_sdk_api_impl.cc
// © 2020 Vizio Services, All Rights Reserved
//



#include "vizio_media_device_sdk_api_impl.h"
#include "vizio_sdk_logger.h"

namespace
{
//    int videoBufferPoolCount = 30;
}

namespace viziosdk { namespace media { namespace capture {

MediaDeviceSDKAPIImpl::MediaDeviceSDKAPIImpl()
    : capDevFactory(std::make_unique<VizioVideoCaptureDeviceFactory>()),
      capDevClient(std::make_shared<VizioVideoCaptureDeviceClient>()) /*,
      videoFrameReceiver(std::unique_ptr<SDKAPIVideoFrameReceiver>()),
      videoBufferPool(new ::media::VideoCaptureBufferPoolImpl(::media::VideoCaptureBufferType::kSharedMemory, videoBufferPoolCount)),
*/
{
}

MediaDeviceSDKAPIImpl::~MediaDeviceSDKAPIImpl()
{
}

//
// For cameras
// 
 
// GetCameraDevices
//    This API call is called by the app to get all the camera devices available on the TV. 
// 	  This call returns an array of camera devices. For each camera, the following properties are returned:
//        Device ID – unique device ID
//        Group ID – ID of the group this device is part of, if any
//        Name – name of the device 

CameraDeviceInfoVec   MediaDeviceSDKAPIImpl::GetCameraDevices()
{
    viziosdk::INFO(__file_line__ + __func__);

    CameraDeviceInfoVec deviceInfoVec;
    capDevFactory->GetDeviceDescriptors(deviceInfoVec);

    for (auto& deviceInfo : deviceInfoVec)
    {
        capDevFactory->GetSupportedFormats(deviceInfo);
    }

    return deviceInfoVec;
}

// OpenCamera
//    This API call is called by the app to turn on a camera device identified by the passed argument.

Status   MediaDeviceSDKAPIImpl::OpenCamera(const std::string& id, const CameraParameters& params)
//        const VideoStreamCB* video_stream_callback)
{
    viziosdk::INFO(__file_line__ + __func__);

#if 1
    capDev = capDevFactory->CreateDevice(id);

    if (capDev == nullptr)
    {
        viziosdk::ERROR(__file_line__ + __func__ + " failed to open device " + id);
        return -1;
    }

    capDev->AllocateAndStart(params, capDevClient);
#else
    std::unique_ptr<VizioVideoCaptureDevice> capDev = capDevFactory->CreateDevice(id);

    if (capDev == nullptr)
    {
        viziosdk::ERROR(__file_line__ + __func__ + " failed to open device " + id);
        return -1;
    }

    std::shared_ptr<VizioVideoCaptureDeviceClient> capDevClient = std::make_shared<VizioVideoCaptureDeviceClient>();
    capDevClientMap[id] = capDevClient;

    capDev->AllocateAndStart(params, capDevClient);
    capDevMap[id] = std::move(capDev);
#endif

    return 0;
}

// CloseCamera
//    This API call is called by the app to turn off a camera device identified by the passed argument.

Status   MediaDeviceSDKAPIImpl::CloseCamera(const std::string& id)
{
#if 1
    capDev->StopAndDeAllocate();
#else
    capDevMap[id]->StopAndDeAllocate();
#endif

    return 0;
}

// StartVideoCapture
//    This API call is called by the app to register a call back function to receive video stream 
//    from a camera device that has been successfully opened by OpenCamera call.
//    Prior to calling StartVideoCapture, the app must have called SetFrameRateAndResolution() and SetCodec().
//    media::VideoFrame is defined in media/base/video_frame.h

void MediaDeviceSDKAPIImpl::StartVideoCapture(
        const std::string& id,
        VideoStreamCB* video_stream_callback)
{
#if 0
    std::shared_ptr<VizioVideoCaptureDeviceClient> capDevClient = capDevClientMap[id];
#endif
    capDevClient->SetReceiver(id, video_stream_callback);
    capDevClient->StartVideo();
}

// StopVideoCapture
//    This API call is called by the app to stop video capture on a given device.

void MediaDeviceSDKAPIImpl::StopVideoCapture(const std::string& id)
{
#if 0
    std::shared_ptr<VizioVideoCaptureDeviceClient> capDevClient = capDevClientMap[id];
#endif
    capDevClient->StopVideo();
//    capDevClient->ClearReceiver(id);
}

//
// For photos
// 
 
// TakePhoto
//    This API call is called by the app to capture a photo

#if 0
void MediaDeviceSDKAPIImpl::TakePhoto(
        const std::string& id,
        const TakePhotoCB& take_photo_callback)
{
}
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
MicrophoneDeviceInfoVec   MediaDeviceSDKAPIImpl::GetMicrophoneDevices()
{
    /* TODO */ return MicrophoneDeviceInfoVec();
}

// OpenMic
//    This API call is called by the app to turn on a microphone device identified by the passed argument.

Status   MediaDeviceSDKAPIImpl::OpenMic(const std::string& id)
{
    /* TODO */ return 0;
}

// CloseMic
//    This API call is called by the app to turn off a microphone device identified by the passed argument.

Status   MediaDeviceSDKAPIImpl::CloseMic(const std::string& id)
{
    /* TODO */ return 0;
}

// StartAudioCapture
//    This API call is called by the app to register a call back function to receive audio stream(s) 
//    from one or more microphone device(s) that has been successfully opened by OpenCamera call.
//    Prior to calling StartAudioCapture, the app must have called SetMicParameters().
//    media::AudioInputStream is defined in media/audio/audio_io.h

void MediaDeviceSDKAPIImpl::StartAudioCapture(
        const std::string& id,
        const AudioStreamCB& audio_stream_callback)
{
}

// StopAudioCapture
//    This API call is called by the app to stop audio capture on a given device.

void MediaDeviceSDKAPIImpl::StopAudioCapture(const std::string& id)
{
}

// SetMicParameters
//    This API call is called by the app to set microphone parameters. 
//    These parameters include, among others (see media::AudioParameters in media/base/audio_parameters.h): 
//        Format
//        EchoCancelation
//        NoiseReduction
//        Automatic Gain Control

Status   MediaDeviceSDKAPIImpl::SetMicParameters(const std::string& id, const media::AudioParameters& parameters)
{
    /* TODO */ return 0;
}
#endif


//
// SDK API private methods
//

void MediaDeviceSDKAPIImpl::BuildVideoCaptureParams(const CameraParameters& params)
{
}


//
// SDKAPIVideoFrameReceiver methods
//

#if 0
void SDKAPIVideoFrameReceiver::OnFrameReadyInBuffer(
    int buffer_id,
    int frame_feedback_id,
    std::unique_ptr<
        ::media::VideoCaptureDevice::Client::Buffer::ScopedAccessPermission>
        buffer_read_permission,
    mojom::VideoFrameInfoPtr frame_info)
{
}
#endif


}}} // namespace viziosdk::media::capture

