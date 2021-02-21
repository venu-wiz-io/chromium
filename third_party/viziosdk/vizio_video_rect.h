//
// vizio_video_capture_device_client.h
// © 2020 Vizio Services, All Rights Reserved
//

#pragma once

// Defines a simple integer rectangle class.  The containment semantics
// are array-like; that is, the coordinate (x, y) is considered to be
// contained by the rectangle, but the coordinate (x + width, y) is not.
// The class will happily let you create malformed rectangles (that is,
// rectangles with negative width and/or height), but there will be assertions
// in the operations (such as Contains()) to complain in this case.


#include <cmath>
#include <iosfwd>
#include <string>

namespace viziosdk { namespace media { namespace capture {

//class Insets;

class Rect {
};


}}} // namespace viziosdk::media::capture

