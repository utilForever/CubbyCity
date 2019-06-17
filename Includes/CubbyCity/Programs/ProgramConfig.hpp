// Copyright (c) 2019 Chris Ohk, Paul Kweon, Den So, Edward Sung

// We are making my contributions/submissions to this project solely in our
// personal capacity and are not conveying any rights to any intellectual
// property of any third parties.

#ifndef CUBBYCITY_PROGRAM_CONFIG_HPP
#define CUBBYCITY_PROGRAM_CONFIG_HPP

#include <string>

namespace CubbyCity
{
struct ProgramConfig
{
    std::string apiKey;

    std::string tileX;
    std::string tileY;
    int tileZ;

    bool terrain;
    int terrainSubdivision;
    float terrainExtrusionScale;

    bool buildings;
    bool roads;
};
}  // namespace CubbyCity

#endif  // CUBBYCITY_PROGRAM_CONFIG_HPP