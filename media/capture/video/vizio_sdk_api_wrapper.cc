//
// vizio_sdk_api_wrapper.cc
// © 2020 Vizio Services, All Rights Reserved
//

#include <iostream>

#include "vizio_sdk_api_wrapper.h"

using namespace viziosdk::media::capture;

#if 1
namespace {
const int kWidth = 1920;
const int kHeight = 1080;
const float kFrameRate = 30;
//cont uint32_t kFourCC = 875967048;

std::unique_ptr<media::VideoCaptureDevice::Client> client_;
} // anonymous
#endif

namespace media
{
//get the singleton
std::shared_ptr<VizioSDKAPIWrapper> get_vizio_sdk_api_wrapper()
{
    static std::shared_ptr<VizioSDKAPIWrapper> singleton = std::make_shared<VizioSDKAPIWrapper>();
    return singleton;
}

void videoReceiver(const uint8_t* data, 
                   int length, 
                   const viziosdk::media::capture::VideoCaptureFormat& format,
                   const viziosdk::high_res_time_point reference_time,
                   const viziosdk::duration_nanos timestamp)
{
    client_->OnIncomingCapturedData(data, length, 
                                  media::VideoCaptureFormat(gfx::Size(kWidth, kHeight), kFrameRate, media::PIXEL_FORMAT_H264),
				  gfx::ColorSpace(),
                                  0, // rotation
				  false,
                                  base::TimeTicks::Now(), // (base::TimeTicks)reference_time,
                                  base::TimeDelta::FromNanoseconds(timestamp.count()),
                                  0); // frame_feedback_id
}


VizioSDKAPIWrapper::VizioSDKAPIWrapper()
    : sdkApi(std::make_unique<MediaDeviceSDKAPIImpl>())
{
}

VizioSDKAPIWrapper::~VizioSDKAPIWrapper()
{
}

CameraDeviceInfoVec   VizioSDKAPIWrapper::GetCameraDevices()
{
    return sdkApi->GetCameraDevices();
}

viziosdk::media::capture::Status   VizioSDKAPIWrapper::OpenCamera(const std::string& id, const CameraParameters& params)
{
    sdkApi->OpenCamera(id, params);
    return 0;
}

viziosdk::media::capture::Status   VizioSDKAPIWrapper::CloseCamera(const std::string& id)
{
    return 0;
}

void VizioSDKAPIWrapper::StartVideoCapture(
        const std::string& id,
        VideoStreamCB* video_stream_callback,
        std::unique_ptr<VideoCaptureDevice::Client> client)
{
    client_ = std::move(client);
    sdkApi->StartVideoCapture(id, video_stream_callback);
}

void VizioSDKAPIWrapper::StopVideoCapture(const std::string& id)
{
}

} //namespace media


#if 0

void printCameraInfo(CameraDeviceInfoVec& deviceInfoVec)
{
    int counter = 0;

    for (auto& deviceInfo : deviceInfoVec)
    {
        std::cout << "Camera device " << ++counter << ": " << std::endl;
        std::cout << "    deviceId: " << deviceInfo.deviceId << std::endl;
        std::cout << "    groupId: " << deviceInfo.groupId << std::endl;
        std::cout << "    deviceName: " << deviceInfo.deviceName << std::endl;
        std::cout << "    modelId: " << deviceInfo.modelId << std::endl;

        int formatCounter = 0;

        for (auto& formatInfo : deviceInfo.cameraParameters.formatInfoVec)
        {
            std::cout << "    Format " << ++formatCounter << ": " << std::endl;
            std::cout << "        name: " << formatInfo.name << std::endl;
            std::cout << "        width: " << formatInfo.width << std::endl;
            std::cout << "        height: " << formatInfo.height << std::endl;
            std::cout << "        frameRate: " << formatInfo.frameRate << std::endl;
        }
    }
}

int main()
{
    MediaDeviceSDKAPIImpl sdkApi;

    CameraDeviceInfoVec deviceInfoVec = sdkApi.GetCameraDevices();
    printCameraInfo(deviceInfoVec);

    std::string deviceId = std::string("/dev/video0");
    CameraParameters params;
    params.formatInfoVec.emplace_back(std::string("H.264"), 875967048, 1920, 1080, 30);
//    params.formatInfoVec.emplace_back(std::string("YUYV 4:2:2"), 1448695129, 1280, 720, 10);
//    params.formatInfoVec.emplace_back(std::string("Motion-JPEG"), 1196444237, 1920, 1080, 30);

    sdkApi.OpenCamera(deviceId, params);

//    const VideoStreamCB* callBack = static_cast<const VideoStreamCB*>(&videoReceiver);
//    const VideoStreamCB callBack { reinterpret_cast<void (*) (const uint8_t*, int,
//                                                const VideoCaptureFormat&, 
//                                                const viziosdk::high_res_time_point, 
//                                                const viziosdk::duration_nanos)>(videoReceiver) };
    const VideoStreamCB* callBack = &videoReceiver;

    sdkApi.StartVideoCapture(deviceId, callBack);

    while (true)
    {}
}

#endif // 0
