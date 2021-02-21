//
// vizio_uvc_capture_delegate.h
// © 2020 Vizio Services, All Rights Reserved
//

#pragma once

#include <iosfwd>
#include <string>

#include "vizio_video_rect.h"

namespace viziosdk { namespace media { namespace capture {

// A size has width and height values.
class Size {
 public:
  constexpr Size() : width_(0), height_(0) {}
  constexpr Size(int width, int height)
      : width_(width < 0 ? 0 : width), height_(height < 0 ? 0 : height) {}

  constexpr int width() const { return width_; }
  constexpr int height() const { return height_; }

  void set_width(int width) { width_ = width < 0 ? 0 : width; }
  void set_height(int height) { height_ = height < 0 ? 0 : height; }

  // This call will CHECK if the area of this size would overflow int.
  int GetArea() const;
  // Returns a checked numeric representation of the area.
//  base::CheckedNumeric<int> GetCheckedArea() const;

  void SetSize(int width, int height) {
    set_width(width);
    set_height(height);
  }

  void Enlarge(int grow_width, int grow_height);

  void SetToMin(const Size& other);
  void SetToMax(const Size& other);

  bool IsEmpty() const { return !width() || !height(); }

  std::string ToString() const;

 private:
  int width_;
  int height_;
};

inline bool operator==(const Size& lhs, const Size& rhs) {
  return lhs.width() == rhs.width() && lhs.height() == rhs.height();
}

inline bool operator!=(const Size& lhs, const Size& rhs) {
  return !(lhs == rhs);
}

// This is declared here for use in gtest-based unit tests but is defined in
// the //ui/gfx:test_support target. Depend on that to use this in your unit
// test. This should not be used in production code - call ToString() instead.
void PrintTo(const Size& size, ::std::ostream* os);

// Helper methods to scale a gfx::Size to a new gfx::Size.
Size ScaleToCeiledSize(const Size& size,
                                       float x_scale,
                                       float y_scale);
Size ScaleToCeiledSize(const Size& size, float scale);
Size ScaleToFlooredSize(const Size& size,
                                        float x_scale,
                                        float y_scale);
Size ScaleToFlooredSize(const Size& size, float scale);
Size ScaleToRoundedSize(const Size& size,
                                        float x_scale,
                                        float y_scale);
Size ScaleToRoundedSize(const Size& size, float scale);

}}} // namespace viziosdk::media::capture

