// Copyright (c) 2019 Chris Ohk, Paul Kweon, Den So, Edward Sung

// We are making my contributions/submissions to this project solely in our
// personal capacity and are not conveying any rights to any intellectual
// property of any third parties.

#include <CubbyCity/Commons/Utils.hpp>
#include <CubbyCity/Geometry/Geometry.hpp>
#include <CubbyCity/Platform/DownloadUtils.hpp>

namespace CubbyCity
{
void Geometry::ParseTiles(const std::string& tileX, const std::string& tileY,
                          int tileZ)
{
    auto [startX, endX] = ExtractTileRange(tileX);
    auto [startY, endY] = ExtractTileRange(tileY);

    for (int x = startX; x <= endX; ++x)
    {
        for (int y = startY; y <= endY; ++y)
        {
            Tile t(x, y, tileZ);

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

            m_tiles.emplace_back(t);
        }
    }
}

void Geometry::DownloadData(const std::string& apiKey, bool terrain,
                            float terrainExtrusionScale, bool buildings,
                            bool roads)
{
    for (auto& tile : m_tiles)
    {
        if (terrain)
        {
            std::string url = GetTerrainURL(tile, apiKey);
            auto textureData =
                DownloadHeightmapTile(url, terrainExtrusionScale);

            if (!textureData)
            {
                throw std::logic_error(
                    "Failed to download heightmap texture data");
            }

            m_heightData[tile] = std::move(textureData);
        }

        if (buildings || roads)
        {
            std::string url = GetVectorTileURL(tile, apiKey);
            auto tileData = DownloadTile(url, tile);

            if (!tileData)
            {
                throw std::logic_error("Failed to download vector tile data");
            }

            m_vectorTileData[tile] = std::move(tileData);
        }
    }
}

void Geometry::AdjustTerrainEdges()
{
    for (auto& tileData0 : m_heightData)
    {
        auto& tileHeight0 = tileData0.second;

        for (auto& tileData1 : m_heightData)
        {
            if (tileData0.first == tileData1.first)
            {
                continue;
            }

            auto& tileHeight1 = tileData1.second;

            if (tileData0.first.x + 1 == tileData1.first.x &&
                tileData0.first.y == tileData1.first.y)
            {
                for (int y = 0; y < tileHeight0->height; ++y)
                {
                    const float h0 =
                        tileHeight0->elevation[tileHeight0->width - 1][y];
                    const float h1 = tileHeight1->elevation[0][y];
                    const float h = (h0 + h1) * 0.5f;
                    tileHeight0->elevation[tileHeight0->width - 1][y] = h;
                    tileHeight1->elevation[0][y] = h;
                }
            }

            if (tileData0.first.y + 1 == tileData1.first.y &&
                tileData0.first.x == tileData1.first.x)
            {
                for (int x = 0; x < tileHeight0->width; ++x)
                {
                    const float h0 =
                        tileHeight0->elevation[x][tileHeight0->height - 1];
                    const float h1 = tileHeight1->elevation[x][0];
                    const float h = (h0 + h1) * 0.5f;
                    tileHeight0->elevation[x][tileHeight0->height - 1] = h;
                    tileHeight1->elevation[x][0] = h;
                }
            }
        }
    }
}

void Geometry::BuildMeshes(const ProgramConfig& config)
{
    glm::vec2 offset;
    Tile origin = m_tiles[0];

    // Build meshes for each of the tiles
    for (auto& tile : m_tiles)
    {
        offset.x = (tile.x - origin.x) * 2;
        offset.y = -(tile.y - origin.y) * 2;

        const auto& texData = m_heightData[tile];
    }
}

std::tuple<int, int> Geometry::ExtractTileRange(const std::string& range) const
{
    const std::vector<std::string> tilesRange = SplitString(range, '/');
    if (tilesRange.size() > 2 || tilesRange.empty())
    {
        throw std::invalid_argument("Bad tile parameter");
    }

    int start, end;

    if (tilesRange.size() == 2)
    {
        start = std::stoi(tilesRange[0]);
        end = std::stoi(tilesRange[1]);
    }
    else
    {
        start = end = std::stoi(tilesRange[0]);
    }

    if (end < start)
    {
        throw std::invalid_argument("Bad tile parameter");
    }

    return std::make_tuple(start, end);
}
}  // namespace CubbyCity