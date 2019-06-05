// Copyright (c) 2019 Chris Ohk, Paul Kweon, Den So, Edward Sung

// We are making my contributions/submissions to this project solely in our
// personal capacity and are not conveying any rights to any intellectual
// property of any third parties.

#include <CubbyCity/Commons/Utils.hpp>
#include <CubbyCity/Geometry/GeometryData.hpp>
#include <CubbyCity/Platform/DownloadUtils.hpp>
#include <CubbyCity/Programs/Program.hpp>

#include <memory>
#include <unordered_map>
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
    std::unordered_map<Tile, std::unique_ptr<HeightData>> heightData;
    std::unordered_map<Tile, std::unique_ptr<TileData>> vectorTileData;

    for (auto& tile : tiles)
    {
        if (m_config.terrain)
        {
            std::string url = GetTerrainURL(tile, m_config.apiKey);
            auto textureData =
                DownloadHeightmapTile(url, m_config.terrainExtrusionScale);

            if (!textureData)
            {
                throw std::logic_error(
                    "Failed to download heightmap texture data");
            }

            heightData[tile] = std::move(textureData);
        }

        if (m_config.buildings || m_config.roads)
        {
            std::string url = GetVectorTileURL(tile, m_config.apiKey);
            auto tileData = DownloadTile(url, tile);

            if (!tileData)
            {
                throw std::logic_error("Failed to download vector tile data");
            }

            vectorTileData[tile] = std::move(tileData);
        }
    }

    // Adjust terrain edges
    if (m_config.terrain)
    {
    }
}
}  // namespace CubbyCity