//
// vizio_audio_io.h
// © 2020 Vizio Services, All Rights Reserved
//


#pragma once

#include <stdint.h>

#include "vizio_sdk_types.h"
#include "vizio_audio_bus.h"


namespace viziosdk { namespace media { namespace capture {

  class AudioInputCallback {
   public:
    AudioInputCallback() {}
    virtual ~AudioInputCallback() {}

    virtual void OnData(uint8_t data[],
                        int length,
                        const viziosdk::high_res_time_point capture_time,
                        double volume) = 0;

    virtual void OnData(const AudioBus* source,
                        const viziosdk::high_res_time_point capture_time,
                        double volume) = 0;

    virtual void OnError() = 0;
  };

}}} // namespace viziosdk::media::capture

