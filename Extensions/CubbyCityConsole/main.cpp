// Copyright (c) 2019 Chris Ohk, Paul Kweon, Den So, Edward Sung

// We are making my contributions/submissions to this project solely in our
// personal capacity and are not conveying any rights to any intellectual
// property of any third parties.

#include <CubbyCity/Programs/Program.hpp>
#include <CubbyCity/Programs/ProgramConfig.hpp>

using namespace CubbyCity;

int main()
{
    ProgramConfig config;
    config.apiKey = "ZhvAFy8gTZSk6sPnAn_KwA";
    config.tileX = "19294";
    config.tileY = "24642";
    config.tileZ = 16;
    config.terrainSubdivision = 64;
    config.terrainExtrusionScale = 1.0;
    config.buildingsHeight = 0.0;
    config.buildingsExtrusionScale = 1.0;
    config.roadsHeight = 1.0;
    config.roadsExtrusionWidth = 5.0;
    config.pedestalHeight = 0.0;
    config.offsetX = 0.0;
    config.offsetY = 0.0;
    config.terrain = true;
    config.buildings = true;
    config.roads = true;
    config.pedestal = true;
    config.normals = true;
    config.splitMesh = true;
    config.append = true;

    Program program(config);
    program.Process();

    return 0;
}