//
// vizio_camera_device_info.cc
// © 2020 Vizio Services, All Rights Reserved
//

#include "vizio_camera_device_info.h"

namespace viziosdk { namespace media { namespace capture {


FormatInfo::FormatInfo() {}
FormatInfo::FormatInfo(std::string _name, uint32_t _fourcc, uint32_t _width, uint32_t _height, float _frameRate)
        : name(_name),
          fourcc(_fourcc),
          width(_width),
          height(_height),
          frameRate(_frameRate) {}
FormatInfo::~FormatInfo() {}

CameraParameters::CameraParameters()
{
}

CameraParameters::~CameraParameters()
{
}

CameraParameters::CameraParameters(const CameraParameters&)
{
}

CameraDeviceInfo::CameraDeviceInfo(std::string _deviceId, std::string _groupId, std::string _deviceName, std::string _modelId)
        : deviceId(_deviceId),
          groupId(_groupId),
          deviceName(_deviceName),
          modelId(_modelId)
{
}

CameraDeviceInfo::~CameraDeviceInfo() 
{
}

CameraDeviceInfo::CameraDeviceInfo(const CameraDeviceInfo&)
{
}

}}} // namespace viziosdk::media::capture
