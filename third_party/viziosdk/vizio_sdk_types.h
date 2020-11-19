//
// vizio_sdk_types.h
// © 2020 Vizio Services, All Rights Reserved
//

#pragma once

#include <chrono>

namespace viziosdk { 

//using high_res_time_point = std::chrono::time_point<std::chrono::high_resolution_clock>;
using high_res_time_point = std::chrono::time_point<std::chrono::system_clock>;
using duration_nanos = std::chrono::duration<long int, std::nano>;

} // namespace viziosdk
