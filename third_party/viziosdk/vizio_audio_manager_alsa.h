//
// vizio_audio_manager_alsa.h
// © 2020 Vizio Services, All Rights Reserved
//


#pragma once

#include <memory>
#include <string>

#include "vizio_mic_device_info.h"
#include "vizio_audio_device_name.h"
#include "vizio_audio_parameters.h"
#include "vizio_alsa_input.h"


namespace viziosdk { namespace media { namespace capture {

class AlsaWrapper;

class CastAudioManagerAlsa {
 public:
  CastAudioManagerAlsa(
      bool use_mixer);
  ~CastAudioManagerAlsa();

  // CastAudioManager implementation.
  bool HasAudioInputDevices();
  void GetAudioInputDeviceNames(
      MicrophoneDeviceInfoVec& deviceInfoVec);
  void GetAudioInputDeviceNames(
      AudioDeviceNames* device_names);
#if 0
  AudioParameters GetInputStreamParameters(
      const std::string& device_id);
#endif

  AlsaPcmInputStream* MakeInputStream(
      const std::string& device_id,
      const VizioAudioParameters& vizioAudioParams);

 private:
  enum StreamType {
    kStreamPlayback = 0,
    kStreamCapture,
  };

  // CastAudioManager implementation.

  AlsaPcmInputStream* MakeInputStream(
      const AudioParameters& params,
      const std::string& device_id);

  // Gets a list of available ALSA devices.
  void GetAlsaAudioDevices(StreamType type,
                           AudioDeviceNames* device_names);

  // Gets the ALSA devices' names and ids that support streams of the
  // given type.
  void GetAlsaDevicesInfo(StreamType type,
                          void** hint,
                          AudioDeviceNames* device_names);

  // Checks if the specific ALSA device is available.
  static bool IsAlsaDeviceAvailable(StreamType type, const char* device_name);

  static const char* UnwantedDeviceTypeWhenEnumerating(StreamType wanted_type);

  std::unique_ptr<AlsaWrapper> wrapper_;
};

}}} // namespace viziosdk::media::capture

