// Copyright (c) 2019 Chris Ohk, Paul Kweon, Den So, Edward Sung

// We are making my contributions/submissions to this project solely in our
// personal capacity and are not conveying any rights to any intellectual
// property of any third parties.

#include <CubbyCity/Programs/Program.hpp>

#include <string>
#include <utility>

namespace CubbyCity
{
Program::Program(ProgramConfig config) : m_config(std::move(config))
{
    // Do nothing
}

void Program::Process()
{
    // Parse tile

    // Download data
}

std::string Program::GetVectorTileURL(const Tile& tile,
                                      const std::string& apiKey) const
{
    return "https://tile.nextzen.org/tilezen/vector/v1/256/all/" +
           std::to_string(tile.z) + "/" + std::to_string(tile.x) + "/" +
           std::to_string(tile.y) + ".json?api_key=" + apiKey;
}

std::string Program::GetTerrainURL(const Tile& tile,
                                   const std::string& apiKey) const
{
    return "https://tile.nextzen.org/tilezen/terrain/v1/260/terrarium/" +
           std::to_string(tile.z) + "/" + std::to_string(tile.x) + "/" +
           std::to_string(tile.y) + ".png?api_key=" + apiKey;
}

}  // namespace CubbyCity