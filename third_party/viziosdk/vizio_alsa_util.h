//
// vizio_alsa_util.h
// © 2020 Vizio Services, All Rights Reserved
//


#pragma once

#include "alsa/asoundlib.h"
#include <string>

namespace viziosdk { namespace media { namespace capture { 

class AlsaWrapper;

namespace alsa_util {

snd_pcm_t* OpenCaptureDevice(AlsaWrapper* wrapper,
                             const char* device_name,
                             int channels,
                             int sample_rate,
                             snd_pcm_format_t pcm_format,
                             int latency_us);

snd_pcm_t* OpenPlaybackDevice(AlsaWrapper* wrapper,
                              const char* device_name,
                              int channels,
                              int sample_rate,
                              snd_pcm_format_t pcm_format,
                              int latency_us);

int CloseDevice(AlsaWrapper* wrapper, snd_pcm_t* handle);

snd_mixer_t* OpenMixer(AlsaWrapper* wrapper,
                       const std::string& device_name);

void CloseMixer(AlsaWrapper* wrapper,
                snd_mixer_t* mixer,
                const std::string& device_name);

snd_mixer_elem_t* LoadCaptureMixerElement(AlsaWrapper* wrapper,
                                          snd_mixer_t* mixer);

} // namespace alsa_util
}}} // namespace viziosdk::media::capture
