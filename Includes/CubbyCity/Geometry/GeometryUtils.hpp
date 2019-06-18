// Copyright (c) 2019 Chris Ohk, Paul Kweon, Den So, Edward Sung

// We are making my contributions/submissions to this project solely in our
// personal capacity and are not conveying any rights to any intellectual
// property of any third parties.

#ifndef CUBBYCITY_GEOMETRY_UTILS_HPP
#define CUBBYCITY_GEOMETRY_UTILS_HPP

#include <CubbyCity/Commons/Constants.hpp>

#include <glm/glm.hpp>

namespace CubbyCity
{
inline glm::dvec2 ConvertLonLatToMeters(const glm::dvec2 lonLat)
{
    glm::dvec2 meters;
    meters.x = lonLat.x * HALF_CIRCUMFERENCE * INV_180;
    meters.y = log(tan(MATH_PI * 0.25 + lonLat.y * MATH_PI * INV_360)) *
               static_cast<double>(RADIUS_EARTH);

    return meters;
}

inline glm::dvec2 ConvertPixelToMeters(const glm::dvec2 pixel, const int zoom,
                                       double invTileSize)
{
    glm::dvec2 meters;
    double res = (2.0 * HALF_CIRCUMFERENCE * invTileSize) / (1 << zoom);
    meters.x = pixel.x * res - HALF_CIRCUMFERENCE;
    meters.y = pixel.y * res - HALF_CIRCUMFERENCE;

    return meters;
}

inline glm::dvec4 GetTileBounds(int x, int y, int z, double tileSize)
{
    return glm::dvec4(
        ConvertPixelToMeters({ x * tileSize, y * tileSize }, z, 1.0 / tileSize),
        ConvertPixelToMeters({ (x + 1) * tileSize, (y + 1) * tileSize }, z,
                             1.0 / tileSize));
}

inline bool IsWithinTileRange(const glm::vec2& pos)
{
    return pos.x >= -1.0 && pos.x <= 1.0 && pos.y >= -1.0 && pos.y <= 1.0;
}
}  // namespace CubbyCity

#endif  // CUBBYCITY_GEOMETRY_UTILS_HPP