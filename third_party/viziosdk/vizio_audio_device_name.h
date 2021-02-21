//
// vizio_audio_device_name.h
// © 2020 Vizio Services, All Rights Reserved
//


#pragma once

#include <list>
#include <string>

//#include "media/base/media_export.h"

namespace viziosdk { namespace media { namespace capture {

struct AudioDeviceName {
  AudioDeviceName();
  AudioDeviceName(std::string device_name, std::string unique_id);

  // Creates default device representation.
  // Shouldn't be used in the audio service, since the audio service doesn't
  // have access to localized device names.
  static AudioDeviceName CreateDefault();

  // Creates communications device representation.
  // Shouldn't be used in the audio service, since the audio service doesn't
  // have access to localized device names.
  static AudioDeviceName CreateCommunications();

  std::string device_name;  // Friendly name of the device.
  std::string unique_id;    // Unique identifier for the device.
};

typedef std::list<AudioDeviceName> AudioDeviceNames;

}}} // namespace viziosdk::media::capture

