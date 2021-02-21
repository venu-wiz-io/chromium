//
// vizio_media_device_sdk_api.h
// © 2020 Vizio Services, All Rights Reserved
//


#pragma once

#include <memory>
#include <functional>

#include "vizio_camera_device_info.h"
#include "vizio_mic_device_info.h"
#include "vizio_video_capture_format.h"
#include "vizio_sdk_types.h"

namespace viziosdk { namespace media { namespace capture {

// MediaDeviceSDKAPI class provides API calls for apps to access media devices (cameras and microphones)

using Status = int;

class MediaDeviceSDKAPI
{
public:
    virtual ~MediaDeviceSDKAPI() {}

    //
    // For camera devices
    //

    // GetCameraDevices
    //    This API call is called by the app to get all the camera devices available on the TV. 
    // 	  This call returns an array of camera devices. For each camera, the following properties are returned:
    //        Device ID – unique device ID
    //        Group ID – ID of the group this device is part of, if any
    //        Name – name of the device 

    virtual CameraDeviceInfoVec   GetCameraDevices() = 0; 

    // OpenCamera
    //    This API call is called by the app to turn on a camera device identified by the passed argument.

    virtual Status   OpenCamera(const std::string& id, const CameraParameters& params /*,
        const VideoStreamCB* video_stream_callback*/) = 0;

    // CloseCamera
    //    This API call is called by the app to turn off a camera device identified by the passed argument.

    virtual Status   CloseCamera(const std::string& id) = 0;

    // StartVideoCapture
    //    This API call is called by the app to register a call back function to receive video stream 
    //    from a camera device that has been successfully opened by OpenCamera call.
    //    Prior to calling StartVideoCapture, the app must have called SetFrameRateAndResolution() and SetCodec().
    //    media::VideoFrame is defined in media/base/video_frame.h

    virtual void StartVideoCapture(
        const std::string& id,
        VideoStreamCB* video_stream_callback) = 0;

    // StopVideoCapture
    //    This API call is called by the app to stop video capture on a given device.

    virtual void StopVideoCapture(const std::string& id) = 0;

    // TakePhoto
    //    This API call is called by the app to capture a photo

// TODO
#if 0
    using TakePhotoCB =
        base::OnceCallback<void(const scoped_refptr<media::VideoFrame>& )>;

    virtual void TakePhoto(
        const std::string& id,
        const TakePhotoCB& take_photo_callback) = 0;
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

    virtual MicrophoneDeviceInfoVec   GetMicrophoneDevices() = 0;

    // OpenMic
    //    This API call is called by the app to turn on a microphone device identified by the passed argument.

    virtual Status   OpenMic(const std::string& id, const VizioAudioParameters& params) = 0;

    // CloseMic
    //    This API call is called by the app to turn off a microphone device identified by the passed argument.

    virtual Status   CloseMic(const std::string& id) = 0;

    // StartAudioCapture
    //    This API call is called by the app to register a call back function to receive audio stream(s) 
    //    from one or more microphone device(s) that has been successfully opened by OpenCamera call.
    //    Prior to calling StartAudioCapture, the app must have called SetMicParameters().
    //    media::AudioInputStream is defined in media/audio/audio_io.h

    virtual void StartAudioCapture(
        const std::string& id,
        AudioStreamCB* audio_stream_callback) = 0;

    // StopAudioCapture
    //    This API call is called by the app to stop audio capture on a given device.

    virtual void StopAudioCapture(const std::string& id) = 0;

}; 

}}} // namespace viziosdk::media::capture

