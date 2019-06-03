// Copyright (c) 2019 Chris Ohk, Paul Kweon, Den So, Edward Sung

// We are making my contributions/submissions to this project solely in our
// personal capacity and are not conveying any rights to any intellectual
// property of any third parties.

#include <CubbyCity/Commons/Utils.hpp>
#include <CubbyCity/Programs/Program.hpp>

#include <vector>

namespace CubbyCity
{
Program::Program(ProgramConfig config) : m_config(std::move(config))
{
    // Do nothing
}

void Program::Process()
{
    // Parse tile
    std::vector<Tile> tiles;

    auto [startX, endX] = ExtractTileRange(m_config.tileX);
    auto [startY, endY] = ExtractTileRange(m_config.tileY);

    for (int x = startX; x <= endX; ++x)
    {
        for (int y = startY; y <= endY; ++y)
        {
            Tile t(x, y, m_config.tileZ);

            if (x == startX)
            {
                t.borders.set(Border::Left, true);
            }
            if (x == endX)
            {
                t.borders.set(Border::Right, true);
            }
            if (y == startY)
            {
                t.borders.set(Border::Top, true);
            }
            if (y == endY)
            {
                t.borders.set(Border::Bottom, true);
            }

            tiles.emplace_back(t);
        }
    }

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