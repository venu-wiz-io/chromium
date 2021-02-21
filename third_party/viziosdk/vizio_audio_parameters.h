//
// vizio_audio_parameters.h
// © 2020 Vizio Services, All Rights Reserved
//


#pragma once

#include <stdint.h>
#include <string>
#include <vector>

#include "vizio_audio_sample_format.h"


namespace viziosdk { namespace media { namespace capture {

class AudioParameters {
public:
  enum Format {
    AUDIO_PCM_LINEAR = 0,            // PCM is 'raw' amplitude samples.
    AUDIO_PCM_LOW_LATENCY,           // Linear PCM, low latency requested.
    AUDIO_BITSTREAM_AC3,             // Compressed AC3 bitstream.
    AUDIO_BITSTREAM_EAC3,            // Compressed E-AC3 bitstream.
    AUDIO_FAKE,                      // Creates a fake AudioOutputStream object.
    AUDIO_FORMAT_LAST = AUDIO_FAKE,  // Only used for validation of format.
  };

  AudioParameters();
  AudioParameters(Format format,
                  SampleFormat sample_format,
                  int/*ChannelLayout*/ channels,
                  int sample_rate,
                  int frames_per_buffer);
  ~AudioParameters();


  // Returns size of audio buffer in bytes when using |fmt| for samples.
  int GetBytesPerBuffer(SampleFormat fmt) const;

  // Returns the number of bytes representing a frame of audio when using |fmt|
  // for samples.
  int GetBytesPerFrame(SampleFormat fmt) const;

  SampleFormat sample_format() const { return sample_format_; }

  int frames_per_buffer() const { return frames_per_buffer_; }

  int sample_rate() const { return sample_rate_; }

  int channels() const { return channels_; }

private:
//  Format format_;                 // Format of the stream.
  SampleFormat sample_format_;
//  int /*ChannelLayout*/ channel_layout_;  // Order of surround sound channels.
  int channels_;                  // Number of channels. Value set based on
                                  // |channel_layout|.
//  int effects_;                   // Bitmask using PlatformEffectsMask.
  int sample_rate_;               // Sampling frequency/rate.
  int frames_per_buffer_;         // Number of frames in a buffer.
};


}}} // namespace viziosdk::media::capture

