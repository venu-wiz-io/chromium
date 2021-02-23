//
// vizio_audio_sdk_api_wrapper.cc
// © 2020 Vizio Services, All Rights Reserved
//

#include <iostream>

#include "vizio_audio_sdk_api_wrapper.h"

using namespace viziosdk::media::capture;

#if 1
namespace {

media::AudioInputStream::AudioInputCallback* client_ = nullptr;
std::unique_ptr<media::AudioBus> audio_bus_;

#if 0

struct WaveHeader
{
        char RIFF_marker[4];
        uint32_t file_size;
        char filetype_header[4];
        char format_marker[4];
        uint32_t data_header_length;
        uint16_t format_type;
        uint16_t number_of_channels;
        uint32_t sample_rate;
        uint32_t bytes_per_second;
        uint16_t bytes_per_frame;
        uint16_t bits_per_sample;
};

struct WaveHeader *genericWAVHeader(uint32_t sample_rate, uint16_t bit_depth, uint16_t channels)
{
    struct WaveHeader *hdr;
    hdr = (WaveHeader*) malloc(sizeof(*hdr));
    if (!hdr)
        return NULL;

    memcpy(&hdr->RIFF_marker, "RIFF", 4);
    memcpy(&hdr->filetype_header, "WAVE", 4);
    memcpy(&hdr->format_marker, "fmt ", 4);
    hdr->data_header_length = 16;
    hdr->format_type = 1;
    hdr->number_of_channels = channels;
    hdr->sample_rate = sample_rate;
    hdr->bytes_per_second = sample_rate * channels * bit_depth / 8;
    hdr->bytes_per_frame = channels * bit_depth / 8;
    hdr->bits_per_sample = bit_depth;

    return hdr;
}

int writeWAVHeader(int fd, struct WaveHeader *hdr)
{
    if (!hdr)
        return -1;

    write(fd, &hdr->RIFF_marker, 4);
    write(fd, &hdr->file_size, 4);
    write(fd, &hdr->filetype_header, 4);
    write(fd, &hdr->format_marker, 4);
    write(fd, &hdr->data_header_length, 4);
    write(fd, &hdr->format_type, 2);
    write(fd, &hdr->number_of_channels, 2);
    write(fd, &hdr->sample_rate, 4);
    write(fd, &hdr->bytes_per_second, 4);
    write(fd, &hdr->bytes_per_frame, 2);
    write(fd, &hdr->bits_per_sample, 2);
    write(fd, "data", 4);

    uint32_t data_size = hdr->file_size + 8 - 44;
    write(fd, &data_size, 4);

    return 0;
}

FILE *audio_fp = nullptr;

const std::string audio_name = "pcm_s16le";
const uint32_t sampleRate = 44100;
const uint8_t channels = 1;
const uint16_t bitDepth = 16;

#endif

} // anonymous
#endif

namespace media
{
//get the singleton
std::shared_ptr<VizioAudioSDKAPIWrapper> get_vizio_audio_sdk_api_wrapper()
{
    static std::shared_ptr<VizioAudioSDKAPIWrapper> singleton = std::make_shared<VizioAudioSDKAPIWrapper>();
    return singleton;
}

void audioReceiver(uint8_t* data, 
                   int length,
                   const viziosdk::high_res_time_point reference_time)
{
#if 1
      audio_bus_->FromInterleaved<SignedInt16SampleTypeTraits>(
          reinterpret_cast<int16_t*>(data),
          audio_bus_->frames());

//      printf("%s length: %d", __func__, length);

      if (client_)
      {
          client_->OnData(audio_bus_.get(),
                          base::TimeTicks::Now(),
                          1 /*normalized_volume*/);
      }

#else
//    viziosdk::DEBUG(__file_line__ + __func__ + " length: " + std::to_string(length));

    static bool once = true;

    // .wav header
  if (once) {
    struct WaveHeader *hdr;
    hdr = genericWAVHeader(sampleRate, bitDepth, channels);
//    uint32_t pcm_data_size = hdr->sample_rate * hdr->bytes_per_frame * duration / 1000;
//    hdr->file_size = pcm_data_size + 44 - 8;

    auto filedesc = open("/tmp/pcm_audio_sdk_wrapper.wav", O_WRONLY | O_CREAT, 0644);
    writeWAVHeader(filedesc, hdr);
    close(filedesc);

    audio_fp=fopen("/tmp/pcm_audio_sdk_wrapper.wav","ab");

    once = false;
  }

//static    FILE *audio_fp=fopen("/tmp/pcm_audio.wav","ab");
            fwrite(data, 2 /*length*/, 1, audio_fp);
            fflush(audio_fp);
//            fclose(audio_fp);
#endif
}


VizioAudioSDKAPIWrapper::VizioAudioSDKAPIWrapper()
    : sdkApi(std::make_unique<MediaDeviceSDKAPIImpl>()),
      opened(false)
{
}

VizioAudioSDKAPIWrapper::~VizioAudioSDKAPIWrapper()
{
}

MicrophoneDeviceInfoVec   VizioAudioSDKAPIWrapper::GetMicrophoneDevices()
{
    return sdkApi->GetMicrophoneDevices();
}

Status   VizioAudioSDKAPIWrapper::OpenMic(const std::string& id, const VizioAudioParameters& vizioAudioParams, const AudioParameters& params)
{
    std::cout << __func__ << std::endl;
    audio_bus_ = (AudioBus::Create(params));
    opened = sdkApi->OpenMic(id, vizioAudioParams);
    return 0;
}

Status   VizioAudioSDKAPIWrapper::CloseMic(const std::string& id)
{
    return 0;
}

void VizioAudioSDKAPIWrapper::StartAudioCapture(
        const std::string& id,
        AudioStreamCB* audio_stream_callback,
       media::AudioInputStream::AudioInputCallback* client)
{
    client_ = (client);
    if (opened)
        sdkApi->StartAudioCapture(id, audio_stream_callback);
}

void VizioAudioSDKAPIWrapper::StopAudioCapture(const std::string& id)
{
}

void VizioAudioSDKAPIWrapper::SetDevice(const std::string& id)
{
    device_id = id;
}

std::string VizioAudioSDKAPIWrapper::GetDevice() const
{
    return device_id;
}

} //namespace media


