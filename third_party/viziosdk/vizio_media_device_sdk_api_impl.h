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
#include "vizio_audio_manager_alsa.h"
#include "vizio_audio_capture_device_client.h"

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

    MicrophoneDeviceInfoVec   GetMicrophoneDevices() override;

    // OpenMic
    //    This API call is called by the app to turn on a microphone device identified by the passed argument.

    Status   OpenMic(const std::string& id, const VizioAudioParameters& params) override;

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
        AudioStreamCB* audio_stream_callback) override;

    // StopAudioCapture
    //    This API call is called by the app to stop audio capture on a given device.

    void StopAudioCapture(const std::string& id) override;

private:
    std::unique_ptr<VizioVideoCaptureDeviceFactory>                         capDevFactory;
    std::unique_ptr<VizioVideoCaptureDevice>         capDev;
    std::shared_ptr<VizioVideoCaptureDeviceClient>   capDevClient;
    std::unique_ptr<CastAudioManagerAlsa>            alsaAudioManager;
    std::unique_ptr<AlsaPcmInputStream>              alsaPcmInputStream;
    std::shared_ptr<VizioAudioCaptureDeviceClient>   audioCapDevClient;
    bool                                             mic_opened;
}; 


}}} // namespace viziosdk::media::capture
