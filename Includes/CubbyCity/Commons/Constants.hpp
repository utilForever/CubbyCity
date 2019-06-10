// Copyright (c) 2019 Chris Ohk, Paul Kweon, Den So, Edward Sung

// We are making my contributions/submissions to this project solely in our
// personal capacity and are not conveying any rights to any intellectual
// property of any third parties.

#ifndef CUBBYCITY_CONSTANTS_HPP
#define CUBBYCITY_CONSTANTS_HPP

namespace CubbyCity
{
constexpr double RADIUS_EARTH = 6378137.0;
constexpr double MATH_PI = 3.14159265358979323846;

constexpr static double INV_360 = 1.0 / 360.0;
constexpr static double INV_180 = 1.0 / 180.0;
constexpr static double HALF_CIRCUMFERENCE = MATH_PI * RADIUS_EARTH;
}  // namespace CubbyCity

#endif  // CUBBYCITY_CONSTANTS_HPP