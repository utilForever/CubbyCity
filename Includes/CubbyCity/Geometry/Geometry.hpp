// Copyright (c) 2019 Chris Ohk, Paul Kweon, Den So, Edward Sung

// We are making my contributions/submissions to this project solely in our
// personal capacity and are not conveying any rights to any intellectual
// property of any third parties.

#ifndef CUBBYCITY_GEOMETRY_HPP
#define CUBBYCITY_GEOMETRY_HPP

#include <CubbyCity/Geometry/GeometryData.hpp>
#include <CubbyCity/Geometry/Tile.hpp>
#include <CubbyCity/Programs/ProgramConfig.hpp>

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
                      double terrainExtrusionScale, bool buildings, bool roads);

    void AdjustTerrainEdges();

    void BuildMeshes(const ProgramConfig& config);

 private:
    static void BuildPlane(std::vector<PolygonVertex>& outVertices,
                           std::vector<unsigned int>& outIndices, int width,
                           int height, int nw, int nh, bool flip = false);

    static void BuildPedestalPlanes(
        const Tile& tile, std::vector<PolygonVertex>& outVertices,
        std::vector<unsigned int>& outIndices,
        const std::unique_ptr<HeightData>& elevation, int subDiv,
        double pedestalHeight);

    static double BuildPolygonExtrusion(
        const Polygon& polygon, double minHeight, double height,
        std::vector<PolygonVertex>& outVertices,
        std::vector<unsigned int>& outIndices,
        const std::unique_ptr<HeightData>& elevation, double inverseTileScale);

    static void BuildPolygon(const Polygon& polygon, double height,
                             std::vector<PolygonVertex>& outVertices,
                             std::vector<unsigned int>& outIndices,
                             double centroidHeight, double inverseTileScale);

    static void AddPolygonPolylinePoint(Line& line, glm::dvec3 cur,
                                        glm::dvec3 next, glm::dvec3 last,
                                        double extrude, size_t lineDataSize,
                                        size_t i, bool forward);

    std::vector<Tile> m_tiles;
    std::vector<std::unique_ptr<PolygonMesh>> meshes;
    std::unordered_map<Tile, std::unique_ptr<HeightData>> m_heightData;
    std::unordered_map<Tile, std::unique_ptr<TileData>> m_vectorTileData;
};
}  // namespace CubbyCity

#endif  // CUBBYCITY_GEOMETRY_HPP