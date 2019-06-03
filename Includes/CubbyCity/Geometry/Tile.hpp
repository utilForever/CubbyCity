// Copyright (c) 2019 Chris Ohk, Paul Kweon, Den So, Edward Sung

// We are making my contributions/submissions to this project solely in our
// personal capacity and are not conveying any rights to any intellectual
// property of any third parties.

#ifndef CUBBYCITY_TILE_HPP
#define CUBBYCITY_TILE_HPP

#include <CubbyCity/Geometry/GeometryUtils.hpp>

#include <glm/glm.hpp>

#include <bitset>

namespace CubbyCity
{
enum Border
{
    Left,
    Right,
    Top,
    Bottom
};

struct Tile
{
    Tile(int _x, int _y, int _z) : x(_x), y(_y), z(_z)
    {
        glm::dvec4 bounds = GetTileBounds(x, y, z, 256.0);
        tileOrigin = glm::dvec2(0.5 * (bounds.x + bounds.z),
                                -0.5 * (bounds.y + bounds.w));
        double scale = 0.5 * glm::abs(bounds.x - bounds.z);
        invScale = 1.0 / scale;
        borders = 0;
    }

    int x;
    int y;
    int z;

    std::bitset<4> borders;

    double invScale = 0.0;
    glm::dvec2 tileOrigin;
};
}  // namespace CubbyCity

#endif  // CUBBYCITY_TILE_HPP