//
// vizio_audio_capture_device_client.h
// © 2020 Vizio Services, All Rights Reserved
//


#pragma once

#include "vizio_audio_io.h"
#include "vizio_mic_device_info.h"

namespace viziosdk { namespace media { namespace capture {

class VizioAudioCaptureDeviceClient : public AudioInputCallback
{
public:
    VizioAudioCaptureDeviceClient();
    ~VizioAudioCaptureDeviceClient() override;

    void SetReceiver(std::string device_id, AudioStreamCB* audio_receiver);

    void OnData(uint8_t data[],
                        int length,
                        const viziosdk::high_res_time_point capture_time,
                        double volume) override;

    void OnData(const AudioBus* source,
                        const viziosdk::high_res_time_point capture_time,
                        double volume) override;

    void OnError() override;

private:
    AudioStreamCB* audio_receiver_;
};

}}} // namespace viziosdk::media::capture
