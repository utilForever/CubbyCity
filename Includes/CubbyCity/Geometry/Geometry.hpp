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
    Geometry() = default;
    Geometry(ProgramConfig config);

    void ParseTiles(const std::string& tileX, const std::string& tileY,
                    int tileZ);

    void DownloadData(const std::string& apiKey, bool terrain,
                      double terrainExtrusionScale, bool buildings, bool roads);

    void AdjustTerrainEdges();

    void BuildMeshes();

 private:
    void BuildTerrainMesh(const Tile& tile, const glm::dvec2& offset,
                          const std::unique_ptr<HeightData>& texData);

    void BuildPedestal(const Tile& tile, const glm::dvec2& offset,
                       const std::unique_ptr<HeightData>& texData);

    void BuildVectorTileMesh(const Tile& tile, const glm::dvec2& offset,
                             const std::unique_ptr<HeightData>& texData);

    void BuildBuildings(const std::vector<Polygon>& polygons,
                        std::unique_ptr<PolygonMesh>& mesh, const Tile& tile,
                        const std::unique_ptr<HeightData>& texData,
                        double minHeight, double height);

    void BuildingRoads(const std::vector<Line>& lines,
                       std::unique_ptr<PolygonMesh>& mesh, const Tile& tile,
                       const std::unique_ptr<HeightData>& texData) const;

    void ExportToFile();

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
    std::vector<std::unique_ptr<PolygonMesh>> m_meshes;
    std::unordered_map<Tile, std::unique_ptr<HeightData>> m_heightData;
    std::unordered_map<Tile, std::unique_ptr<TileData>> m_vectorTileData;

    ProgramConfig m_config;
};
}  // namespace CubbyCity

#endif  // CUBBYCITY_GEOMETRY_HPP