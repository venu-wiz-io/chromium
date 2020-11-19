//
// vizio_camera_device_info.h
// © 2020 Vizio Services, All Rights Reserved
//

#pragma once

#include <vector>
#include <string>

#include "vizio_video_capture_format.h"
#include "vizio_sdk_types.h"

namespace viziosdk { namespace media { namespace capture {

struct FormatInfo
{
    FormatInfo();
    FormatInfo(std::string _name, uint32_t _fourcc, uint32_t _width, uint32_t _height, float _frameRate);
    ~FormatInfo();

    std::string name;
    uint32_t fourcc;
    uint32_t width;
    uint32_t height;
    float frameRate;
};

using FormatInfoVec = std::vector<FormatInfo>;

struct CameraParameters
{
    CameraParameters();
    ~CameraParameters();
    CameraParameters(const CameraParameters&);

    FormatInfoVec formatInfoVec;

    bool white_balance_temp_auto;
    int32_t white_balance_temp;
    int32_t exposure;
    int32_t pan;		// units to be defined
    int32_t tilt;		// units to be defined
    int32_t focus;	        // units to be defined
    int32_t zoom;		// units to be defined
    int16_t backlight_compensation;
    int16_t gamma;
    int8_t brightness;
    int8_t contrast;
    int8_t saturation;
    int8_t hue;
    int8_t gain;
    int8_t sharpness;
};

struct CameraDeviceInfo
{
    CameraDeviceInfo(std::string _deviceId, std::string _groupId, std::string _deviceName, std::string _modelId);
    ~CameraDeviceInfo();
    CameraDeviceInfo(const CameraDeviceInfo&);

    std::string deviceId;
    std::string groupId;
    std::string deviceName;
    std::string modelId;

    CameraParameters cameraParameters;
};

using CameraDeviceInfoVec = std::vector<CameraDeviceInfo>;

using VideoStreamCB =
    void(const uint8_t* data, 
         int length, 
         const VideoCaptureFormat& format,
         const viziosdk::high_res_time_point reference_time,
         const viziosdk::duration_nanos timestamp);

}}} // namespace viziosdk::media::capture
