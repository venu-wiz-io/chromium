//
// vizio_audio_sdk_api_wrapper.h
// © 2020 Vizio Services, All Rights Reserved
//

#pragma once

#include "vizio_media_device_sdk_api_impl.h"
#include "media/audio/audio_io.h"
#include "media/base/audio_bus.h"

namespace media
{

class VizioAudioSDKAPIWrapper
{
public:
    VizioAudioSDKAPIWrapper();
    ~VizioAudioSDKAPIWrapper();

    viziosdk::media::capture::MicrophoneDeviceInfoVec   GetMicrophoneDevices();
    viziosdk::media::capture::Status   OpenMic(const std::string& id, 
                                               const viziosdk::media::capture::VizioAudioParameters& vizioAudioParams, 
                                               const AudioParameters& params);
    viziosdk::media::capture::Status   CloseMic(const std::string& id);
    void StartAudioCapture(
        const std::string& id,
        viziosdk::media::capture::AudioStreamCB* audio_stream_callback,
       media::AudioInputStream::AudioInputCallback* client_);
    void StopAudioCapture(const std::string& id);

    void SetDevice(const std::string& id);
    std::string GetDevice() const;

    static const bool bVizioSDK = true;

private:
    std::unique_ptr<viziosdk::media::capture::MediaDeviceSDKAPIImpl> sdkApi;
    std::string device_id;
    bool opened;
};

std::shared_ptr<VizioAudioSDKAPIWrapper> get_vizio_audio_sdk_api_wrapper();

void audioReceiver(uint8_t* data,
                   int length,
                   const viziosdk::high_res_time_point reference_time);

} //namespace media

