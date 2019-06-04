// Copyright (c) 2019 Chris Ohk, Paul Kweon, Den So, Edward Sung

// We are making my contributions/submissions to this project solely in our
// personal capacity and are not conveying any rights to any intellectual
// property of any third parties.

#ifndef CUBBYCITY_PROGRAM_HPP
#define CUBBYCITY_PROGRAM_HPP

#include <CubbyCity/Geometry/Tile.hpp>
#include <CubbyCity/Programs/ProgramConfig.hpp>

#include <string>

namespace CubbyCity
{
class Program
{
 public:
    Program(ProgramConfig config);

    void Process();

 private:
    ProgramConfig m_config;
};
}  // namespace CubbyCity

#endif  // CUBBYCITY_PROGRAM_HPP