//
// vizio_audio_sample_format.h
// © 2020 Vizio Services, All Rights Reserved
//

#pragma once

//#include "media/base/media_shmem_export.h"

namespace viziosdk { namespace media { namespace capture {

enum SampleFormat {
  kUnknownSampleFormat = 0,
  kSampleFormatU8,          // Unsigned 8-bit w/ bias of 128.
  kSampleFormatS16,         // Signed 16-bit.
  kSampleFormatS32,         // Signed 32-bit.
  kSampleFormatF32,         // Float 32-bit.
  kSampleFormatPlanarS16,   // Signed 16-bit planar.
  kSampleFormatPlanarF32,   // Float 32-bit planar.
  kSampleFormatPlanarS32,   // Signed 32-bit planar.
  kSampleFormatS24,         // Signed 24-bit.
  kSampleFormatAc3,         // Compressed AC3 bitstream.
  kSampleFormatEac3,        // Compressed E-AC3 bitstream.
  kSampleFormatMpegHAudio,  // Compressed MPEG-H audio bitstream.

  // Must always be equal to largest value ever logged.
  kSampleFormatMax = kSampleFormatMpegHAudio,
};

// Returns the number of bytes used per channel for the specified
// |sample_format|.
int SampleFormatToBytesPerChannel(
    SampleFormat sample_format);
int SampleFormatToBitsPerChannel(SampleFormat sample_format);

// Returns the name of the sample format as a string
const char* SampleFormatToString(SampleFormat sample_format);

// Returns true if |sample_format| is planar, false otherwise.
bool IsPlanar(SampleFormat sample_format);

// Returns true if |sample_format| is interleaved, false otherwise.
bool IsInterleaved(SampleFormat sample_format);

// Returns true if |sample_format| is compressed bitstream, false otherwise.
bool IsBitstream(SampleFormat sample_format);

}}} // namespace viziosdk::media::capture
