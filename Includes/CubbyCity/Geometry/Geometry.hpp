// Copyright (c) 2019 Chris Ohk, Paul Kweon, Den So, Edward Sung

// We are making my contributions/submissions to this project solely in our
// personal capacity and are not conveying any rights to any intellectual
// property of any third parties.

#ifndef CUBBYCITY_GEOMETRY_HPP
#define CUBBYCITY_GEOMETRY_HPP

#include <CubbyCity/Geometry/GeometryData.hpp>
#include <CubbyCity/Geometry/Tile.hpp>

#include <memory>
#include <unordered_map>
#include <vector>

namespace CubbyCity
{
class Geometry
{
 public:
    void ParseTiles(const std::string& tileX, const std::string& tileY,
                    int tileZ);
    void DownloadData(const std::string& apiKey, bool terrain,
                      float terrainExtrusionScale, bool buildings, bool roads);

    void AdjustTerrainEdges();

 private:
    std::tuple<int, int> ExtractTileRange(const std::string& range) const;

    std::vector<Tile> m_tiles;
    std::unordered_map<Tile, std::unique_ptr<HeightData>> m_heightData;
    std::unordered_map<Tile, std::unique_ptr<TileData>> m_vectorTileData;
};
}  // namespace CubbyCity

#endif  // CUBBYCITY_GEOMETRY_HPP