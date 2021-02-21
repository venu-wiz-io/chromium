//
// vizio_mic_device_info.h
// © 2020 Vizio Services, All Rights Reserved
//


#pragma once

#include <vector>
#include <string>

#include "vizio_sdk_types.h"

namespace viziosdk { namespace media { namespace capture {

struct AudioCodecInfo
{
    AudioCodecInfo();
    AudioCodecInfo(std::string _name, uint32_t _sampleRate, uint8_t _channels, uint16_t _bitDepth);
    ~AudioCodecInfo();

    std::string name;
    uint32_t sampleRate;
    uint8_t channels;
    uint16_t bitDepth;        // # of bits
};

using AudioCodecInfoVec = std::vector<AudioCodecInfo>;

struct VizioAudioParameters
{
    VizioAudioParameters();
    ~VizioAudioParameters();
    VizioAudioParameters(const VizioAudioParameters&);

    // TODO other audio parameters
    AudioCodecInfoVec audioCodecInfoVec;
};

struct MicrophoneDeviceInfo
{
    MicrophoneDeviceInfo(std::string _deviceId, std::string _groupId, std::string _deviceName, std::string _modelId);
    ~MicrophoneDeviceInfo();
    MicrophoneDeviceInfo(const MicrophoneDeviceInfo&);

    std::string deviceId;
    std::string groupId;
    std::string deviceName;
    std::string modelId;

    VizioAudioParameters audioParameters;
};

using MicrophoneDeviceInfoVec = std::vector<MicrophoneDeviceInfo>;

using AudioStreamCB =
    void(uint8_t data[], 
         int length, 
         const viziosdk::high_res_time_point reference_time);

}}} // namespace viziosdk::media::capture

