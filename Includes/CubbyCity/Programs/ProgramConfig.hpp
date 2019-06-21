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

    int terrainSubdivision;
    double terrainExtrusionScale;
    double buildingsHeight;
    double buildingsExtrusionScale;
    double roadsHeight;
    double roadsExtrusionWidth;
    double pedestalHeight;

    std::string fileName;
    double offsetX;
    double offsetY;

    bool terrain;
    bool buildings;
    bool roads;
    bool pedestal;
    bool normals;
    bool splitMesh;
    bool append;
};
}  // namespace CubbyCity

#endif  // CUBBYCITY_PROGRAM_CONFIG_HPP