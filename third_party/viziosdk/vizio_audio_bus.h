//
// vizio_audio_bus.h
// © 2020 Vizio Services, All Rights Reserved
//


#pragma once

#include <stdint.h>
#include <memory>
#include <vector>


namespace viziosdk { namespace media { namespace capture {

class AudioParameters;

class AudioBus {
public:
  // Guaranteed alignment of each channel's data; use 16-byte alignment for easy
  // SSE optimizations.
  enum { kChannelAlignment = 16 };

  static std::unique_ptr<AudioBus> Create(const AudioParameters& params);

  AudioBus(int channels, int frames);
//  explicit AudioBus(int channels);
  ~AudioBus();

  // Overwrites the sample values stored in this AudioBus instance with values
  // from a given interleaved |source_buffer| with expected layout
  // [ch0, ch1, ..., chN, ch0, ch1, ...] and sample values in the format
  // corresponding to the given SourceSampleTypeTraits.
  // The sample values are converted to float values by means of the method
  // convert_to_float32() provided by the SourceSampleTypeTraits. For a list of
  // ready-to-use SampleTypeTraits, see file audio_sample_types.h.
  // If |num_frames_to_write| is less than frames(), the remaining frames are
  // zeroed out. If |num_frames_to_write| is more than frames(), this results in
  // undefined behavior.
  template <class SourceSampleTypeTraits>
  void FromInterleaved(
      const typename SourceSampleTypeTraits::ValueType* source_buffer,
      int num_frames_to_write);

  void FromInterleaved(const void* source, int frames, int bytes_per_sample);

  // Similar to FromInterleaved...(), but overwrites the frames starting at a
  // given offset |write_offset_in_frames| and does not zero out frames that are
  // not overwritten.
  template <class SourceSampleTypeTraits>
  void FromInterleavedPartial(
      const typename SourceSampleTypeTraits::ValueType* source_buffer,
      int write_offset_in_frames,
      int num_frames_to_write);

  void FromInterleavedPartial(const void* source, int start_frame, int frames,
                              int bytes_per_sample);

  // Reads the sample values stored in this AudioBus instance and places them
  // into the given |dest_buffer| in interleaved format using the sample format
  // specified by TargetSampleTypeTraits. For a list of ready-to-use
  // SampleTypeTraits, see file audio_sample_types.h. If |num_frames_to_read| is
  // larger than frames(), this results in undefined behavior.
  template <class TargetSampleTypeTraits>
  void ToInterleaved(
      int num_frames_to_read,
      typename TargetSampleTypeTraits::ValueType* dest_buffer) const;

  // Similar to ToInterleaved(), but reads the frames starting at a given
  // offset |read_offset_in_frames|.
  template <class TargetSampleTypeTraits>
  void ToInterleavedPartial(
      int read_offset_in_frames,
      int num_frames_to_read,
      typename TargetSampleTypeTraits::ValueType* dest_buffer) const;

  // Returns the number of frames.
  int frames() const { return frames_; }

  // Returns the number of channels.
  int channels() const { return static_cast<int>(channel_data_.size()); }

  // Returns a raw pointer to the requested channel.  Pointer is guaranteed to
  // have a 16-byte alignment.  Warning: Do not rely on having sane (i.e. not
  // inf, nan, or between [-1.0, 1.0]) values in the channel data.
  float* channel(int channel) { return channel_data_[channel]; }
  const float* channel(int channel) const { return channel_data_[channel]; }

  // Helper method for zeroing out all channels of audio data.
  void Zero();
  void ZeroFrames(int frames);
  void ZeroFramesPartial(int start_frame, int frames);

  static void CheckOverflow(int start_frame, int frames, int total_frames);

  size_t GetBitstreamDataSize() const;
  void SetBitstreamDataSize(size_t data_size);
  int GetBitstreamFrames() const;
  void SetBitstreamFrames(int frames);

private:
  template <class SourceSampleTypeTraits>
  static void CopyConvertFromInterleavedSourceToAudioBus(
      const typename SourceSampleTypeTraits::ValueType* source_buffer,
      int write_offset_in_frames,
      int num_frames_to_write,
      AudioBus* dest);

  template <class TargetSampleTypeTraits>
  static void CopyConvertFromAudioBusToInterleavedTarget(
      const AudioBus* source,
      int read_offset_in_frames,
      int num_frames_to_read,
      typename TargetSampleTypeTraits::ValueType* dest_buffer);

  int frames_;

  // Protect SetChannelData() and set_frames() for use by CreateWrapper().
//  bool can_set_channel_data_;

  // One float pointer per channel pointing to a contiguous block of memory for
  // that channel. If the memory is owned by this instance, this will
  // point to the memory in |data_|. Otherwise, it may point to memory provided
  // by the client.
  std::vector<float*> channel_data_;

  // Whether the data is compressed bitstream or not.
  bool is_bitstream_format_ = false;
  // The PCM frame count for a compressed bitstream.
  int bitstream_frames_ = 0;

  // The data size for a compressed bitstream.
  size_t bitstream_data_size_ = 0;

};


// Delegates to FromInterleavedPartial()
template <class SourceSampleTypeTraits>
void AudioBus::FromInterleaved(
    const typename SourceSampleTypeTraits::ValueType* source_buffer,
    int num_frames_to_write) {
  FromInterleavedPartial<SourceSampleTypeTraits>(source_buffer, 0,
                                                 num_frames_to_write);
  // Zero any remaining frames.
  ZeroFramesPartial(num_frames_to_write, frames_ - num_frames_to_write);
}

template <class SourceSampleTypeTraits>
void AudioBus::FromInterleavedPartial(
    const typename SourceSampleTypeTraits::ValueType* source_buffer,
    int write_offset_in_frames,
    int num_frames_to_write) {
  CheckOverflow(write_offset_in_frames, num_frames_to_write, frames_);
  CopyConvertFromInterleavedSourceToAudioBus<SourceSampleTypeTraits>(
      source_buffer, write_offset_in_frames, num_frames_to_write, this);
}

// Delegates to ToInterleavedPartial()
template <class TargetSampleTypeTraits>
void AudioBus::ToInterleaved(
    int num_frames_to_read,
    typename TargetSampleTypeTraits::ValueType* dest_buffer) const {
  ToInterleavedPartial<TargetSampleTypeTraits>(0, num_frames_to_read,
                                               dest_buffer);
}

template <class TargetSampleTypeTraits>
void AudioBus::ToInterleavedPartial(
    int read_offset_in_frames,
    int num_frames_to_read,
    typename TargetSampleTypeTraits::ValueType* dest) const {
  CheckOverflow(read_offset_in_frames, num_frames_to_read, frames_);
  CopyConvertFromAudioBusToInterleavedTarget<TargetSampleTypeTraits>(
      this, read_offset_in_frames, num_frames_to_read, dest);
}

template <class SourceSampleTypeTraits>
void AudioBus::CopyConvertFromInterleavedSourceToAudioBus(
    const typename SourceSampleTypeTraits::ValueType* source_buffer,
    int write_offset_in_frames,
    int num_frames_to_write,
    AudioBus* dest) {
  const int channels = dest->channels();
  for (int ch = 0; ch < channels; ++ch) {
    float* channel_data = dest->channel(ch);
    for (int target_frame_index = write_offset_in_frames,
             read_pos_in_source = ch;
         target_frame_index < write_offset_in_frames + num_frames_to_write;
         ++target_frame_index, read_pos_in_source += channels) {
      auto source_value = source_buffer[read_pos_in_source];
      channel_data[target_frame_index] =
          SourceSampleTypeTraits::ToFloat(source_value);
    }
  }
}

template <class TargetSampleTypeTraits>
void AudioBus::CopyConvertFromAudioBusToInterleavedTarget(
    const AudioBus* source,
    int read_offset_in_frames,
    int num_frames_to_read,
    typename TargetSampleTypeTraits::ValueType* dest_buffer) {
  const int channels = source->channels();
  for (int ch = 0; ch < channels; ++ch) {
    const float* channel_data = source->channel(ch);
    for (int source_frame_index = read_offset_in_frames, write_pos_in_dest = ch;
         source_frame_index < read_offset_in_frames + num_frames_to_read;
         ++source_frame_index, write_pos_in_dest += channels) {
      float sourceSampleValue = channel_data[source_frame_index];
      dest_buffer[write_pos_in_dest] =
          TargetSampleTypeTraits::FromFloat(sourceSampleValue);
    }
  }
}


}}} // namespace viziosdk::media::capture

