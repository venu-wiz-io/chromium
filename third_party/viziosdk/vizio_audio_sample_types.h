//
// vizio_audio_sample_types.h
// © 2020 Vizio Services, All Rights Reserved
//


#pragma once

#include <cmath>
#include <cstdint>
#include <limits>
#include <type_traits>


namespace viziosdk { namespace media { namespace capture {


// For float or double.
// See also the aliases for commonly used types at the bottom of this file.
template <typename SampleType>
class FloatSampleTypeTraits {
  static_assert(std::is_floating_point<SampleType>::value,
                "Template is only valid for float types.");

 public:
  using ValueType = SampleType;

  static constexpr SampleType kMinValue = -1.0f;
  static constexpr SampleType kMaxValue = +1.0f;
  static constexpr SampleType kZeroPointValue = 0.0f;

  static SampleType FromFloat(float source_value) {
    return From<float>(source_value);
  }
  static float ToFloat(SampleType source_value) {
    return To<float>(source_value);
  }
  static SampleType FromDouble(double source_value) {
    return From<double>(source_value);
  }
  static double ToDouble(SampleType source_value) {
    return To<double>(source_value);
  }

 private:
  template <typename FloatType>
  static SampleType From(FloatType source_value) {
    // Apply clipping (aka. clamping). These values are frequently sent to OS
    // level drivers that may not properly handle these values.
    if (std::isnan(source_value))
      return kZeroPointValue;
    if (source_value <= kMinValue)
      return kMinValue;
    if (source_value >= kMaxValue)
      return kMaxValue;
    return static_cast<SampleType>(source_value);
  }

  template <typename FloatType>
  static FloatType To(SampleType source_value) {
    return static_cast<FloatType>(source_value);
  }
};

// For uint8_t, int16_t, int32_t...
// See also the aliases for commonly used types at the bottom of this file.
template <typename SampleType>
class FixedSampleTypeTraits {
  static_assert(std::numeric_limits<SampleType>::is_integer,
                "Template is only valid for integer types.");

 public:
  using ValueType = SampleType;

  static constexpr SampleType kMinValue =
      std::numeric_limits<SampleType>::min();
  static constexpr SampleType kMaxValue =
      std::numeric_limits<SampleType>::max();
  static constexpr SampleType kZeroPointValue =
      (kMinValue == 0) ? (kMaxValue / 2 + 1) : 0;

  static SampleType FromFloat(float source_value) {
    return From<float>(source_value);
  }
  static float ToFloat(SampleType source_value) {
    return To<float>(source_value);
  }
  static SampleType FromDouble(double source_value) {
    return From<double>(source_value);
  }
  static double ToDouble(SampleType source_value) {
    return To<double>(source_value);
  }

 private:
  template <typename FloatType>
  struct ScalingFactors {

    static constexpr FloatType kForPositiveInput =
        static_cast<FloatType>(kMaxValue) -
        static_cast<FloatType>(kZeroPointValue);

    static constexpr FloatType kForNegativeInput =
        static_cast<FloatType>(kZeroPointValue) -
        static_cast<FloatType>(kMinValue);

    static constexpr FloatType kInverseForPositiveInput =
        1.0f / kForPositiveInput;

    static constexpr FloatType kInverseForNegativeInput =
        1.0f / kForNegativeInput;
  };

  template <typename FloatType>
  static SampleType From(FloatType source_value) {
    if (source_value < 0) {
      // Apply clipping (aka. clamping).
      if (source_value <= FloatSampleTypeTraits<float>::kMinValue)
        return kMinValue;

      return static_cast<SampleType>(
          (source_value * ScalingFactors<FloatType>::kForNegativeInput) +
          static_cast<FloatType>(kZeroPointValue));
    } else {
      // Apply clipping (aka. clamping).
      // As mentioned above, here we must include the case |source_value| == 1.
      if (source_value >= FloatSampleTypeTraits<float>::kMaxValue)
        return kMaxValue;
      return static_cast<SampleType>(
          (source_value * ScalingFactors<FloatType>::kForPositiveInput) +
          static_cast<FloatType>(kZeroPointValue));
    }
  }

  template <typename FloatType>
  static FloatType To(SampleType source_value) {
    FloatType offset_value =
        static_cast<FloatType>(source_value - kZeroPointValue);

    // We multiply with the inverse scaling factor instead of dividing by the
    // scaling factor, because multiplication performs faster than division
    // on many platforms.
    return (offset_value < 0.0f)
               ? (offset_value *
                  ScalingFactors<FloatType>::kInverseForNegativeInput)
               : (offset_value *
                  ScalingFactors<FloatType>::kInverseForPositiveInput);
  }
};

// Aliases for commonly used sample formats.
using Float32SampleTypeTraits = FloatSampleTypeTraits<float>;
using Float64SampleTypeTraits = FloatSampleTypeTraits<double>;
using UnsignedInt8SampleTypeTraits = FixedSampleTypeTraits<uint8_t>;
using SignedInt16SampleTypeTraits = FixedSampleTypeTraits<int16_t>;
using SignedInt32SampleTypeTraits = FixedSampleTypeTraits<int32_t>;

}}} // namespace viziosdk::media::capture

