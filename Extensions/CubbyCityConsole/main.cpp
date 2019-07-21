// Copyright (c) 2019 Chris Ohk, Paul Kweon, Den So, Edward Sung

// We are making my contributions/submissions to this project solely in our
// personal capacity and are not conveying any rights to any intellectual
// property of any third parties.

#include <CubbyCity/Programs/Program.hpp>
#include <CubbyCity/Programs/ProgramConfig.hpp>

#include <Clara/include/clara.hpp>

#include <iostream>

using namespace CubbyCity;

inline std::string ToString(const clara::Opt& opt)
{
    std::ostringstream oss;
    oss << (clara::Parser() | opt);
    return oss.str();
}

inline std::string ToString(const clara::Parser& p)
{
    std::ostringstream oss;
    oss << p;
    return oss.str();
}

int main(int argc, char* argv[])
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
    config.terrain = false;
    config.buildings = true;
    config.roads = false;
    config.pedestal = false;
    config.normals = false;
    config.splitMesh = false;
    config.append = false;

    bool showHelp = false;

    // Parsing
    auto parser =
        clara::Help(showHelp) |
        clara::Opt(config.tileX, "tileX")["-x"]["--tileX"]("x value of tile");

    auto result = parser.parse(clara::Args(argc, argv));
    if (!result)
    {
        std::cerr << "Error in command line: " << result.errorMessage() << '\n';
        exit(EXIT_FAILURE);
    }

    if (showHelp)
    {
        std::cout << ToString(parser) << '\n';
        exit(EXIT_SUCCESS);
    }

    Program program(config);
    program.Process();

    return 0;
}