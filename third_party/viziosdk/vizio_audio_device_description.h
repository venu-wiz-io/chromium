//
// vizio_audio_device_description.h
// © 2020 Vizio Services, All Rights Reserved
//


#pragma once

#include <string>
#include <vector>


namespace viziosdk { namespace media { namespace capture {

// Provides common information on audio device names and ids.
struct AudioDeviceDescription {
  // Unique Id of the generic "default" device. Associated with the localized
  // name returned from GetDefaultDeviceName().
  static const char kDefaultDeviceId[];

  // Unique Id of the generic default communications device. Associated with
  // the localized name returned from GetCommunicationsDeviceName().
  static const char kCommunicationsDeviceId[];

  std::string device_name;  // Friendly name of the device.
  std::string unique_id;    // Unique identifier for the device.
  std::string group_id;     // Group identifier.
};

typedef std::vector<AudioDeviceDescription> AudioDeviceDescriptions;

}}} // namespace viziosdk::media::capture
