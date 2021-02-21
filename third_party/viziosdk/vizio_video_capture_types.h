//
// vizio_uvc_capture_delegate.h
// © 2020 Vizio Services, All Rights Reserved
//

#pragma once

#include <stddef.h>
#include <vector>

#include "vizio_video_types.h"
#include "vizio_video_size.h"

namespace viziosdk { namespace media { namespace capture {

typedef int VideoCaptureSessionId;

// Policies for capture devices that have source content that varies in size.
// It is up to the implementation how the captured content will be transformed
// (e.g., scaling and/or letterboxing) in order to produce video frames that
// strictly adheree to one of these policies.
enum class ResolutionChangePolicy {
  // Capture device outputs a fixed resolution all the time. The resolution of
  // the first frame is the resolution for all frames.
  FIXED_RESOLUTION,

  // Capture device is allowed to output frames of varying resolutions. The
  // width and height will not exceed the maximum dimensions specified. The
  // aspect ratio of the frames will match the aspect ratio of the maximum
  // dimensions as closely as possible.
  FIXED_ASPECT_RATIO,

  // Capture device is allowed to output frames of varying resolutions not
  // exceeding the maximum dimensions specified.
  ANY_WITHIN_LIMIT,

  // Must always be equal to largest entry in the enum.
  LAST = ANY_WITHIN_LIMIT,
};

// Potential values of the googPowerLineFrequency optional constraint passed to
// getUserMedia. Note that the numeric values are currently significant, and are
// used to map enum values to corresponding frequency values.
// TODO(ajose): http://crbug.com/525167 Consider making this a class.
enum class PowerLineFrequency {
  FREQUENCY_DEFAULT = 0,
  FREQUENCY_50HZ = 50,
  FREQUENCY_60HZ = 60,
  FREQUENCY_MAX = FREQUENCY_60HZ
};

enum class VideoCaptureBufferType {
  kSharedMemory,
  kSharedMemoryViaRawFileDescriptor,
  kMailboxHolder
};

// WARNING: Do not change the values assigned to the entries. They are used for
// UMA logging.
enum class VideoCaptureError {
  kNone = 0,
  kVideoCaptureControllerInvalidOrUnsupportedVideoCaptureParametersRequested =
      1,
  kVideoCaptureControllerIsAlreadyInErrorState = 2,
  kVideoCaptureManagerDeviceConnectionLost = 3,
  kFrameSinkVideoCaptureDeviceAleradyEndedOnFatalError = 4,
  kFrameSinkVideoCaptureDeviceEncounteredFatalError = 5,
  kV4L2FailedToOpenV4L2DeviceDriverFile = 6,
  kV4L2ThisIsNotAV4L2VideoCaptureDevice = 7,
  kV4L2FailedToFindASupportedCameraFormat = 8,
  kV4L2FailedToSetVideoCaptureFormat = 9,
  kV4L2UnsupportedPixelFormat = 10,
  kV4L2FailedToSetCameraFramerate = 11,
  kV4L2ErrorRequestingMmapBuffers = 12,
  kV4L2AllocateBufferFailed = 13,
  kV4L2VidiocStreamonFailed = 14,
  kV4L2VidiocStreamoffFailed = 15,
  kV4L2FailedToVidiocReqbufsWithCount0 = 16,
  kV4L2PollFailed = 17,
  kV4L2MultipleContinuousTimeoutsWhileReadPolling = 18,
  kV4L2FailedToDequeueCaptureBuffer = 19,
  kV4L2FailedToEnqueueCaptureBuffer = 20,
  kSingleClientVideoCaptureHostLostConnectionToDevice = 21,
  kSingleClientVideoCaptureDeviceLaunchAborted = 22,
  kDesktopCaptureDeviceWebrtcDesktopCapturerHasFailed = 23,
  kFileVideoCaptureDeviceCouldNotOpenVideoFile = 24,
  kDeviceCaptureLinuxFailedToCreateVideoCaptureDelegate = 25,
  kErrorFakeDeviceIntentionallyEmittingErrorEvent = 26,
  kDeviceClientTooManyFramesDroppedY16 = 28,
  kDeviceMediaToMojoAdapterEncounteredUnsupportedBufferType = 29,
  kVideoCaptureManagerProcessDeviceStartQueueDeviceInfoNotFound = 30,
  kInProcessDeviceLauncherFailedToCreateDeviceInstance = 31,
  kServiceDeviceLauncherLostConnectionToDeviceFactoryDuringDeviceStart = 32,
  kServiceDeviceLauncherServiceRespondedWithDeviceNotFound = 33,
  kServiceDeviceLauncherConnectionLostWhileWaitingForCallback = 34,
  kIntentionalErrorRaisedByUnitTest = 35,
  kCrosHalV3FailedToStartDeviceThread = 36,
  kCrosHalV3DeviceDelegateMojoConnectionError = 37,
  kCrosHalV3DeviceDelegateFailedToGetCameraInfo = 38,
  kCrosHalV3DeviceDelegateMissingSensorOrientationInfo = 39,
  kCrosHalV3DeviceDelegateFailedToOpenCameraDevice = 40,
  kCrosHalV3DeviceDelegateFailedToInitializeCameraDevice = 41,
  kCrosHalV3DeviceDelegateFailedToConfigureStreams = 42,
  kCrosHalV3DeviceDelegateWrongNumberOfStreamsConfigured = 43,
  kCrosHalV3DeviceDelegateFailedToGetDefaultRequestSettings = 44,
  kCrosHalV3BufferManagerHalRequestedTooManyBuffers = 45,
  kCrosHalV3BufferManagerFailedToCreateGpuMemoryBuffer = 46,
  kCrosHalV3BufferManagerFailedToMapGpuMemoryBuffer = 47,
  kCrosHalV3BufferManagerUnsupportedVideoPixelFormat = 48,
  kCrosHalV3BufferManagerFailedToDupFd = 49,
  kCrosHalV3BufferManagerFailedToWrapGpuMemoryHandle = 50,
  kCrosHalV3BufferManagerFailedToRegisterBuffer = 51,
  kCrosHalV3BufferManagerProcessCaptureRequestFailed = 52,
  kCrosHalV3BufferManagerInvalidPendingResultId = 53,
  kCrosHalV3BufferManagerReceivedDuplicatedPartialMetadata = 54,
  kCrosHalV3BufferManagerIncorrectNumberOfOutputBuffersReceived = 55,
  kCrosHalV3BufferManagerInvalidTypeOfOutputBuffersReceived = 56,
  kCrosHalV3BufferManagerReceivedMultipleResultBuffersForFrame = 57,
  kCrosHalV3BufferManagerUnknownStreamInCamera3NotifyMsg = 58,
  kCrosHalV3BufferManagerReceivedInvalidShutterTime = 59,
  kCrosHalV3BufferManagerFatalDeviceError = 60,
  kCrosHalV3BufferManagerReceivedFrameIsOutOfOrder = 61,
  kCrosHalV3BufferManagerFailedToUnwrapReleaseFenceFd = 62,
  kCrosHalV3BufferManagerSyncWaitOnReleaseFenceTimedOut = 63,
  kCrosHalV3BufferManagerInvalidJpegBlob = 64,
  kAndroidFailedToAllocate = 65,
  kAndroidFailedToStartCapture = 66,
  kAndroidFailedToStopCapture = 67,
  kAndroidApi1CameraErrorCallbackReceived = 68,
  kAndroidApi2CameraDeviceErrorReceived = 69,
  kAndroidApi2CaptureSessionConfigureFailed = 70,
  kAndroidApi2ImageReaderUnexpectedImageFormat = 71,
  kAndroidApi2ImageReaderSizeDidNotMatchImageSize = 72,
  kAndroidApi2ErrorRestartingPreview = 73,
  kAndroidScreenCaptureUnsupportedFormat = 74,
  kAndroidScreenCaptureFailedToStartCaptureMachine = 75,
  kAndroidScreenCaptureTheUserDeniedScreenCapture = 76,
  kAndroidScreenCaptureFailedToStartScreenCapture = 77,
  kWinDirectShowCantGetCaptureFormatSettings = 78,
  kWinDirectShowFailedToGetNumberOfCapabilities = 79,
  kWinDirectShowFailedToGetCaptureDeviceCapabilities = 80,
  kWinDirectShowFailedToSetCaptureDeviceOutputFormat = 81,
  kWinDirectShowFailedToConnectTheCaptureGraph = 82,
  kWinDirectShowFailedToPauseTheCaptureDevice = 83,
  kWinDirectShowFailedToStartTheCaptureDevice = 84,
  kWinDirectShowFailedToStopTheCaptureGraph = 85,
  kWinMediaFoundationEngineIsNull = 86,
  kWinMediaFoundationEngineGetSourceFailed = 87,
  kWinMediaFoundationFillPhotoCapabilitiesFailed = 88,
  kWinMediaFoundationFillVideoCapabilitiesFailed = 89,
  kWinMediaFoundationNoVideoCapabilityFound = 90,
  kWinMediaFoundationGetAvailableDeviceMediaTypeFailed = 91,
  kWinMediaFoundationSetCurrentDeviceMediaTypeFailed = 92,
  kWinMediaFoundationEngineGetSinkFailed = 93,
  kWinMediaFoundationSinkQueryCapturePreviewInterfaceFailed = 94,
  kWinMediaFoundationSinkRemoveAllStreamsFailed = 95,
  kWinMediaFoundationCreateSinkVideoMediaTypeFailed = 96,
  kWinMediaFoundationConvertToVideoSinkMediaTypeFailed = 97,
  kWinMediaFoundationSinkAddStreamFailed = 98,
  kWinMediaFoundationSinkSetSampleCallbackFailed = 99,
  kWinMediaFoundationEngineStartPreviewFailed = 100,
  kWinMediaFoundationGetMediaEventStatusFailed = 101,
  kMacSetCaptureDeviceFailed = 102,
  kMacCouldNotStartCaptureDevice = 103,
  kMacReceivedFrameWithUnexpectedResolution = 104,
  kMacUpdateCaptureResolutionFailed = 105,
  kMacDeckLinkDeviceIdNotFoundInTheSystem = 106,
  kMacDeckLinkErrorQueryingInputInterface = 107,
  kMacDeckLinkErrorCreatingDisplayModeIterator = 108,
  kMacDeckLinkCouldNotFindADisplayMode = 109,
  kMacDeckLinkCouldNotSelectTheVideoFormatWeLike = 110,
  kMacDeckLinkCouldNotStartCapturing = 111,
  kMacDeckLinkUnsupportedPixelFormat = 112,
  kMacAvFoundationReceivedAVCaptureSessionRuntimeErrorNotification = 113,
  kAndroidApi2ErrorConfiguringCamera = 114,
  kCrosHalV3DeviceDelegateFailedToFlush = 115,
  kMaxValue = 115
};

// WARNING: Do not change the values assigned to the entries. They are used for
// UMA logging.
enum class VideoCaptureFrameDropReason {
  kNone = 0,
  kDeviceClientFrameHasInvalidFormat = 1,
  kDeviceClientLibyuvConvertToI420Failed = 3,
  kV4L2BufferErrorFlagWasSet = 4,
  kV4L2InvalidNumberOfBytesInBuffer = 5,
  kAndroidThrottling = 6,
  kAndroidGetByteArrayElementsFailed = 7,
  kAndroidApi1UnexpectedDataLength = 8,
  kAndroidApi2AcquiredImageIsNull = 9,
  kWinDirectShowUnexpectedSampleLength = 10,
  kWinDirectShowFailedToGetMemoryPointerFromMediaSample = 11,
  kWinMediaFoundationReceivedSampleIsNull = 12,
  kWinMediaFoundationLockingBufferDelieveredNullptr = 13,
  kWinMediaFoundationGetBufferByIndexReturnedNull = 14,
  kBufferPoolMaxBufferCountExceeded = 15,
  kBufferPoolBufferAllocationFailed = 16,
  kMaxValue = 16
};

// Assert that the int:frequency mapping is correct.
static_assert(static_cast<int>(PowerLineFrequency::FREQUENCY_DEFAULT) == 0,
              "static_cast<int>(FREQUENCY_DEFAULT) must equal 0.");
static_assert(static_cast<int>(PowerLineFrequency::FREQUENCY_50HZ) == 50,
              "static_cast<int>(FREQUENCY_DEFAULT) must equal 50.");
static_assert(static_cast<int>(PowerLineFrequency::FREQUENCY_60HZ) == 60,
              "static_cast<int>(FREQUENCY_DEFAULT) must equal 60.");

// Some drivers use rational time per frame instead of float frame rate, this
// constant k is used to convert between both: A fps -> [k/k*A] seconds/frame.
const int kFrameRatePrecision = 10000;

}}} // namespace viziosdk::media::capture

