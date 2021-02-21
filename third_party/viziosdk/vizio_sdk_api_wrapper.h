//
// vizio_sdk_api_wrapper.h
// © 2020 Vizio Services, All Rights Reserved
//

#pragma once

#include "vizio_media_device_sdk_api_impl.h"
#include "media/capture/video/video_capture_device_client.h"

namespace media
{

class VizioSDKAPIWrapper
{
public:
    VizioSDKAPIWrapper();
    ~VizioSDKAPIWrapper();
    viziosdk::media::capture::CameraDeviceInfoVec   GetCameraDevices();
    viziosdk::media::capture::Status   OpenCamera(const std::string& id, const viziosdk::media::capture::CameraParameters& params);
    viziosdk::media::capture::Status   CloseCamera(const std::string& id);
    void StartVideoCapture(
        const std::string& id,
        viziosdk::media::capture::VideoStreamCB* video_stream_callback,
        std::unique_ptr<VideoCaptureDevice::Client> client);
    void StopVideoCapture(const std::string& id);

    static const bool bVizioSDK = true;

private:
    std::unique_ptr<viziosdk::media::capture::MediaDeviceSDKAPIImpl> sdkApi;
};

std::shared_ptr<VizioSDKAPIWrapper> get_vizio_sdk_api_wrapper();

void videoReceiver(const uint8_t* data, 
                   int length, 
                   const viziosdk::media::capture::VideoCaptureFormat& format,
                   const viziosdk::high_res_time_point reference_time,
                   const viziosdk::duration_nanos timestamp);

} //namespace media

