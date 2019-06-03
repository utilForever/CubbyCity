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

    Program program(config);
    program.Process();

    return 0;
}