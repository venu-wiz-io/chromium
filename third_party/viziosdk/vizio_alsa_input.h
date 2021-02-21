//
// vizio_alsa_input.h
// © 2020 Vizio Services, All Rights Reserved
//


#pragma once

#include <alsa/asoundlib.h>
#include <stdint.h>
#include <thread>

#include "vizio_audio_parameters.h"
#include "vizio_audio_io.h"
#include "vizio_sdk_types.h"
#include "vizio_alsa_wrapper.h"

#include <memory>
#include <string>


namespace viziosdk { namespace media { namespace capture {

class AlsaWrapper;
class AudioManagerBase;

// Provides an input stream for audio capture based on the ALSA PCM interface.
// This object is not thread safe and all methods should be invoked in the
// thread that created the object.
class AlsaPcmInputStream
    /* : public AgcAudioStream<AudioInputStream> */ {
 public:
  // Pass this to the constructor if you want to attempt auto-selection
  // of the audio recording device.
  static const char kAutoSelectDevice[];

  // Create a PCM Output stream for the ALSA device identified by
  // |device_name|. If unsure of what to use for |device_name|, use
  // |kAutoSelectDevice|.
  AlsaPcmInputStream(//AudioManagerBase* audio_manager,
                     const std::string& device_name,
                     const AudioParameters& params,
                     AlsaWrapper* wrapper);

  ~AlsaPcmInputStream();

  // Implementation of AudioInputStream.
  bool Open();
  void Start(AudioInputCallback* callback);
  void Stop();
  void Close();
  double GetMaxVolume();
  void SetVolume(double volume);
  double GetVolume();
  bool IsMuted();
  void SetOutputDeviceForAec(const std::string& output_device_id);

 private:
  // Logs the error and invokes any registered callbacks.
  void HandleError(const char* method, int error);

  // Reads one or more buffers of audio from the device, passes on to the
  // registered callback and schedules the next read.
  void ReadAudio();

  // Recovers from any device errors if possible.
  bool Recover(int error);

  // Set |running_| to false on |capture_thread_|.
  void StopRunningOnCaptureThread();

  // Non-refcounted pointer back to the audio manager.
  // The AudioManager indirectly holds on to stream objects, so we don't
  // want circular references.  Additionally, stream objects live on the audio
  // thread, which is owned by the audio manager and we don't want to addref
  // the manager from that thread.
//  AudioManagerBase* audio_manager_;
  std::string device_name_;
  AudioParameters params_;
  int bytes_per_buffer_;
  AlsaWrapper* wrapper_;
  viziosdk::duration_nanos buffer_duration_;  // Length of each recorded buffer.
  AudioInputCallback* callback_;  // Valid during a recording session.
  viziosdk::high_res_time_point next_read_time_;  // Scheduled time for next read callback.
  snd_pcm_t* device_handle_;  // Handle to the ALSA PCM recording device.
  snd_mixer_t* mixer_handle_; // Handle to the ALSA microphone mixer.
  snd_mixer_elem_t* mixer_element_handle_; // Handle to the capture element.
  // Buffer used for reading audio data.
  std::unique_ptr<uint8_t[]> audio_buffer_;
  bool read_callback_behind_schedule_;
  std::unique_ptr<AudioBus> audio_bus_;
  std::thread capture_thread_;
//  bool running_;

};

}}} // namespace viziosdk::media::capture

