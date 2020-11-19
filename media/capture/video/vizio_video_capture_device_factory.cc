//
// vizio_video_capture_device_factory.cc
// © 2020 Vizio Services, All Rights Reserved
//


#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>
#include <dirent.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>
#include <algorithm>
#include <utility>

#include "vizio_video_capture_device_factory.h"
#include "vizio_sdk_logger.h"
#include "vizio_camera_device_info.h"
#include "vizio_video_capture_device.h"
#include "vizio_uvc_capture_device_fd.h"


namespace viziosdk { namespace media { namespace capture {

namespace {

// USB VID and PID are both 4 bytes long.
const size_t kVidPidSize = 4;
const size_t kMaxInterfaceNameSize = 256;

// /sys/class/video4linux/video{N}/device is a symlink to the corresponding
// USB device info directory.
const char kVidPathTemplate[] = "/sys/class/video4linux/%s/device/../idVendor";
const char kPidPathTemplate[] = "/sys/class/video4linux/%s/device/../idProduct";
const char kInterfacePathTemplate[] =
      "/sys/class/video4linux/%s/device/interface";

bool ReadIdFile(const std::string& path, std::string* id) {
    char id_buf[kVidPidSize];
    FILE* file = fopen(path.c_str(), "rb");
    if (!file)
      return false;
    const bool success = fread(id_buf, kVidPidSize, 1, file) == 1;
    fclose(file);
    if (!success)
      return false;
    id->append(id_buf, kVidPidSize);
    return true;
}

std::string ExtractFileNameFromDeviceId(const std::string& device_id) {
    // |unique_id| is of the form "/dev/video2".  |file_name| is "video2".
    const char kDevDir[] = "/dev/";
//    DCHECK(base::StartsWith(device_id, kDevDir, base::CompareCase::SENSITIVE));
    assert(std::equal(device_id.begin(), device_id.begin() + strlen(kDevDir), std::string(kDevDir).begin()));
    return device_id.substr(strlen(kDevDir), device_id.length());
}

// TODO
bool ReadFileToStringWithMaxSize(const std::string& interface_path, std::string& display_name, int) {
    return true;
}

bool isSupportedPixelFormat(uint32_t v4l2_fourcc)
{
    // pass all formats to SDK user now
    // TODO filter as needed in future
    return true;
}

std::vector<std::string> getDeviceFiles(const std::string& path, const char* prefix)
{
    std::vector<std::string> files;

    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir (path.c_str())) != NULL) {
        while ((ent = readdir (dir)) != NULL) {
            if (std::string(ent->d_name).find(prefix) == 0)
                files.push_back(std::string(ent->d_name));
        }
        closedir (dir);
    } else {
        viziosdk::WARN(__file_line__ + __func__ + std::string(" could not open directory"));
    }

    return files;
}

class DevVideoFilePathsDeviceProvider
      : public VizioVideoCaptureDeviceFactory::DeviceProvider {
   public:
    void GetDeviceIds(std::vector<std::string>* target_container) override {
      const std::string path("/dev/");
#if 1
      std::vector<std::string> files = getDeviceFiles(path, "video");

      for (auto& file : files) {
          target_container->emplace_back(path + file);
      }
#else
      base::FileEnumerator enumerator(path, false, base::FileEnumerator::FILES,
                                      "video*");
      while (!enumerator.Next().empty()) {
        const base::FileEnumerator::FileInfo info = enumerator.GetInfo();
        target_container->emplace_back(path.value() + info.GetName().value());
      }
#endif
    }

    std::string GetDeviceModelId(const std::string& device_id) override {
      const std::string file_name = ExtractFileNameFromDeviceId(device_id);
      std::string usb_id;
      char vid_path_str[1024];
      sprintf(vid_path_str, kVidPathTemplate, file_name.c_str());
      const std::string vid_path { vid_path_str };
      if (!ReadIdFile(vid_path, &usb_id))
        return usb_id;

      usb_id.append(":");
      char pid_path_str[1024];
      sprintf(pid_path_str, kPidPathTemplate, file_name.c_str());
      const std::string pid_path { pid_path_str };
      if (!ReadIdFile(pid_path, &usb_id))
        usb_id.clear();

      return usb_id;
    }

    std::string GetDeviceDisplayName(const std::string& device_id) override {
      const std::string file_name = ExtractFileNameFromDeviceId(device_id);
      char interface_path_str[1024];
      sprintf(interface_path_str, kInterfacePathTemplate, file_name.c_str());
      const std::string interface_path { interface_path_str };
      std::string display_name;
      if (!ReadFileToStringWithMaxSize(interface_path,
                                             display_name,
                                             kMaxInterfaceNameSize)) {
        return std::string();
      }
      return display_name;
    }

};

}  // namespace


VizioVideoCaptureDeviceFactory::VizioVideoCaptureDeviceFactory()
      : vizio_uvc_(std::make_shared<VizioUVCCaptureDeviceImpl>()),
        device_provider_(std::make_unique<DevVideoFilePathsDeviceProvider>()) {}

VizioVideoCaptureDeviceFactory::~VizioVideoCaptureDeviceFactory() = default;

std::unique_ptr<VizioVideoCaptureDevice>
VizioVideoCaptureDeviceFactory::CreateDevice(
      const std::string& deviceId) {
    viziosdk::INFO(__file_line__ + __func__);

    auto self =
        std::make_unique<VizioVideoCaptureDevice>(vizio_uvc_, deviceId);

    // make sure the device driver is available, by opening it
    VizioUVCCaptureDeviceFD fd(
        vizio_uvc_.get(), vizio_uvc_->open(deviceId.c_str(), O_RDONLY));
    if (!fd.is_valid()) {
        return nullptr;
    }

    return self;
}

void VizioVideoCaptureDeviceFactory::GetDeviceDescriptors(
      CameraDeviceInfoVec& deviceInfoVec) 
{
    viziosdk::INFO(__file_line__ + __func__);

    std::vector<std::string> filepaths;
    device_provider_->GetDeviceIds(&filepaths);

    for (auto& unique_id : filepaths) {
        const VizioUVCCaptureDeviceFD fd(
            vizio_uvc_.get(), vizio_uvc_->open(unique_id.c_str(), O_RDONLY));
        if (!fd.is_valid()) {
//          DLOG(ERROR) << "Couldn't open " << unique_id;
          continue;
        }

        v4l2_capability cap;
        if ((DoIoctl(fd.get(), VIDIOC_QUERYCAP, &cap) == 0) &&
              (cap.capabilities & V4L2_CAP_VIDEO_CAPTURE))
        {
            const std::string model_id =
                device_provider_->GetDeviceModelId(unique_id);
            std::string display_name =
                device_provider_->GetDeviceDisplayName(unique_id);
            if (display_name.empty())
                display_name = reinterpret_cast<char*>(cap.card);

            std::string capStr = " " + std::string(reinterpret_cast<char*>(cap.driver))
                                    + " " + std::string(reinterpret_cast<char*>(cap.card))
                                    + " " + std::string(reinterpret_cast<char*>(cap.bus_info))
                                    + " " + std::to_string(cap.version)
                                    + " " + std::to_string(cap.capabilities)
                                    + " " + std::to_string(cap.device_caps);
            viziosdk::DEBUG(__file_line__ + __func__ + capStr);

        deviceInfoVec.emplace_back(
            unique_id, "", display_name, model_id);
        }
    }
}

void VizioVideoCaptureDeviceFactory::GetSupportedFormats(
      CameraDeviceInfo& deviceInfo) {
    viziosdk::INFO(__file_line__ + __func__);

    if (deviceInfo.deviceId.empty())
      return;

    VizioUVCCaptureDeviceFD fd(vizio_uvc_.get(), vizio_uvc_->open(deviceInfo.deviceId.c_str(), O_RDONLY));
    if (!fd.is_valid())  // Failed to open this device.
      return;

    deviceInfo.cameraParameters.formatInfoVec.clear();

    FormatInfo formatInfo;
    v4l2_fmtdesc v4l2_format = {};
    v4l2_format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    for (; DoIoctl(fd.get(), VIDIOC_ENUM_FMT, &v4l2_format) == 0; ++v4l2_format.index) {
        if (!isSupportedPixelFormat(v4l2_format.pixelformat))
            continue;

        formatInfo.name = reinterpret_cast<char*>(v4l2_format.description);
        formatInfo.name = v4l2_format.pixelformat;

        std::string capStr = " " + std::to_string(v4l2_format.index)
                                + " " + std::to_string(v4l2_format.type)
                                + " " + std::to_string(v4l2_format.flags)
                                + " " + std::string(reinterpret_cast<char*>(v4l2_format.description))
                                + " " + std::to_string(v4l2_format.pixelformat);
        viziosdk::DEBUG(__file_line__ + __func__ + capStr);

        v4l2_frmsizeenum frame_size = {};
        frame_size.pixel_format = v4l2_format.pixelformat;
        for (; DoIoctl(fd.get(), VIDIOC_ENUM_FRAMESIZES, &frame_size) == 0;
             ++frame_size.index) {
            if (frame_size.type == V4L2_FRMSIZE_TYPE_DISCRETE) {
                formatInfo.width = frame_size.discrete.width;
                formatInfo.height = frame_size.discrete.height;
            } 

            const std::vector<float> frame_rates = GetFrameRateList(
                fd.get(), v4l2_format.pixelformat, frame_size.discrete.width,
                frame_size.discrete.height);
            for (const auto& frame_rate : frame_rates) {
                formatInfo.frameRate = frame_rate;
                deviceInfo.cameraParameters.formatInfoVec.push_back(formatInfo);
            }
        }
    }
}

std::vector<float> VizioVideoCaptureDeviceFactory::GetFrameRateList(
      int fd,
      uint32_t fourcc,
      uint32_t width,
      uint32_t height) {
    std::vector<float> frame_rates;

    v4l2_frmivalenum frame_interval = {};
    frame_interval.pixel_format = fourcc;
    frame_interval.width = width;
    frame_interval.height = height;

    for (; DoIoctl(fd, VIDIOC_ENUM_FRAMEINTERVALS, &frame_interval) == 0;
         ++frame_interval.index) {
        if (frame_interval.type == V4L2_FRMIVAL_TYPE_DISCRETE) {
            if (frame_interval.discrete.numerator != 0) {
                frame_rates.push_back(
                    frame_interval.discrete.denominator /
                static_cast<float>(frame_interval.discrete.numerator));
            }
        } 
    }

    return frame_rates;
}

int VizioVideoCaptureDeviceFactory::DoIoctl(int fd, int request, void* argp) {
    return vizio_uvc_->ioctl(fd, request, argp);
}

}}} // namespace viziosdk::media::capture
