// Copyright (c) 2019 Chris Ohk, Paul Kweon, Den So, Edward Sung

// We are making my contributions/submissions to this project solely in our
// personal capacity and are not conveying any rights to any intellectual
// property of any third parties.

#include <CubbyCity/Commons/Utils.hpp>
#include <CubbyCity/Exporter/OBJExporter.hpp>
#include <CubbyCity/Geometry/Geometry.hpp>
#include <CubbyCity/Geometry/TileUtils.hpp>
#include <CubbyCity/Platform/DownloadUtils.hpp>

#include <earcut.hpp/include/mapbox/earcut.hpp>

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
                            double terrainExtrusionScale, bool buildings,
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
                    const double h0 =
                        tileHeight0->elevation[tileHeight0->width - 1][y];
                    const double h1 = tileHeight1->elevation[0][y];
                    const double h = (h0 + h1) * 0.5;
                    tileHeight0->elevation[tileHeight0->width - 1][y] = h;
                    tileHeight1->elevation[0][y] = h;
                }
            }

            if (tileData0.first.y + 1 == tileData1.first.y &&
                tileData0.first.x == tileData1.first.x)
            {
                for (int x = 0; x < tileHeight0->width; ++x)
                {
                    const double h0 =
                        tileHeight0->elevation[x][tileHeight0->height - 1];
                    const double h1 = tileHeight1->elevation[x][0];
                    const double h = (h0 + h1) * 0.5;
                    tileHeight0->elevation[x][tileHeight0->height - 1] = h;
                    tileHeight1->elevation[x][0] = h;
                }
            }
        }
    }
}

void Geometry::BuildMeshes(const ProgramConfig& config)
{
    glm::dvec2 offset;
    const Tile origin = m_tiles[0];

    // Build meshes for each of the tiles
    for (auto& tile : m_tiles)
    {
        offset.x = (tile.x - origin.x) * 2.0;
        offset.y = -(tile.y - origin.y) * 2.0;

        const auto& texData = m_heightData[tile];

        // Build terrain mesh
        if (config.terrain)
        {
            // Extract a plane geometry for terrain mesh
            auto mesh = std::unique_ptr<PolygonMesh>(new PolygonMesh);
            BuildPlane(mesh->vertices, mesh->indices, 2, 2,
                       config.terrainSubdivision, config.terrainSubdivision);

            // Build terrain mesh extrusion, with bilinear height sampling
            for (auto& vertex : mesh->vertices)
            {
                const glm::dvec2 tilePosition =
                    glm::dvec2(vertex.position.x, vertex.position.y);
                const double extrusion = SampleElevation(tilePosition, texData);

                // Scale the height within the tile scale
                vertex.position.z = extrusion * tile.invScale;
            }

            // Compute faces normals
            if (config.normals)
            {
                ComputeNormals(*mesh);
            }

            mesh->offset = offset;
            meshes.push_back(std::move(mesh));

            // Build pedestal
            if (config.pedestal)
            {
                auto ground = std::unique_ptr<PolygonMesh>(new PolygonMesh);
                auto wall = std::unique_ptr<PolygonMesh>(new PolygonMesh);

                BuildPlane(ground->vertices, ground->indices, 2, 2,
                           config.terrainSubdivision, config.terrainSubdivision,
                           true);

                for (auto& vertex : ground->vertices)
                {
                    vertex.position.z = config.pedestalHeight * tile.invScale;
                }

                BuildPedestalPlanes(tile, wall->vertices, wall->indices,
                                    texData, config.terrainSubdivision,
                                    config.pedestalHeight);

                ground->offset = offset;
                meshes.push_back(std::move(ground));
                wall->offset = offset;
                meshes.push_back(std::move(wall));
            }
        }

        // Build vector tile mesh
        if (config.buildings || config.roads)
        {
            const auto& data = m_vectorTileData[tile];
            if (!data)
            {
                continue;
            }

            const static std::string keyHeight("height");
            const static std::string keyMinHeight("min_height");

            for (const auto& layer : data->layers)
            {
                for (auto feature : layer.features)
                {
                    if (texData && layer.name != "buildings" &&
                        layer.name != "roads")
                    {
                        continue;
                    }

                    auto itHeight = feature.props.numericProps.find(keyHeight);
                    auto itMinHeight =
                        feature.props.numericProps.find(keyMinHeight);
                    double scale =
                        tile.invScale * config.buildingsExtrusionScale;
                    double height = 0.0;
                    double minHeight = 0.0;

                    if (layer.name == "buildings")
                    {
                        height = config.buildingsHeight * tile.invScale;
                    }

                    if (itHeight != feature.props.numericProps.end())
                    {
                        height = itHeight->second * scale;
                    }

                    if (texData && layer.name != "roads" && height == 0.0)
                    {
                        continue;
                    }

                    if (itMinHeight != feature.props.numericProps.end())
                    {
                        minHeight = itMinHeight->second * scale;
                    }

                    auto mesh = std::unique_ptr<PolygonMesh>(new PolygonMesh);

                    if (config.buildings)
                    {
                        for (const Polygon& polygon : feature.polygons)
                        {
                            double centroidHeight = 0.0;
                            if (minHeight != height)
                            {
                                centroidHeight = BuildPolygonExtrusion(
                                    polygon, minHeight, height, mesh->vertices,
                                    mesh->indices, texData, tile.invScale);
                            }

                            BuildPolygon(polygon, height, mesh->vertices,
                                         mesh->indices, centroidHeight,
                                         tile.invScale);
                        }
                    }

                    if (config.roads)
                    {
                        for (Line& line : feature.lines)
                        {
                            Polygon polygon;
                            double extrude =
                                config.roadsExtrusionWidth * tile.invScale;
                            polygon.emplace_back();
                            Line& polygonLine = polygon.back();

                            if (line.size() == 2)
                            {
                                glm::dvec3 cur = line[0];
                                glm::dvec3 next = line[1];
                                glm::dvec3 n0 = GetPerp(next - cur);

                                polygonLine.push_back(cur - n0 * extrude);
                                polygonLine.push_back(cur + n0 * extrude);
                                polygonLine.push_back(next + n0 * extrude);
                                polygonLine.push_back(next - n0 * extrude);
                            }
                            else
                            {
                                glm::dvec3 last = line[0];
                                for (size_t i = 1; i < line.size() - 1; ++i)
                                {
                                    glm::dvec3 cur = line[i];
                                    glm::dvec3 next = line[i + 1];
                                    AddPolygonPolylinePoint(
                                        polygonLine, cur, next, last, extrude,
                                        line.size(), i, true);
                                    last = cur;
                                }

                                last = line[line.size() - 1];
                                for (int i = static_cast<int>(line.size()) - 2;
                                     i > 0; --i)
                                {
                                    glm::dvec3 cur = line[i];
                                    glm::dvec3 next = line[i - 1];
                                    AddPolygonPolylinePoint(
                                        polygonLine, cur, next, last, extrude,
                                        line.size(), i, false);
                                    last = cur;
                                }
                            }

                            if (polygonLine.size() < 4)
                            {
                                continue;
                            }

                            int count = 0;
                            for (size_t i = 0; i < polygonLine.size(); i++)
                            {
                                size_t j = (i + 1) % polygonLine.size();
                                size_t k = (i + 2) % polygonLine.size();
                                double z =
                                    (polygonLine[j].x - polygonLine[i].x) *
                                        (polygonLine[k].y - polygonLine[j].y) -
                                    (polygonLine[j].y - polygonLine[i].y) *
                                        (polygonLine[k].x - polygonLine[j].x);

                                if (z < 0)
                                {
                                    count--;
                                }
                                else if (z > 0)
                                {
                                    count++;
                                }
                            }

                            if (count > 0)
                            {
                                // CCW
                                std::reverse(polygonLine.begin(),
                                             polygonLine.end());
                            }

                            // Close the polygon
                            polygonLine.push_back(polygonLine[0]);

                            size_t vertexOffset = mesh->vertices.size();

                            if (config.roadsHeight > 0)
                            {
                                BuildPolygonExtrusion(
                                    polygon, 0.0,
                                    config.roadsHeight * tile.invScale,
                                    mesh->vertices, mesh->indices, nullptr,
                                    tile.invScale);
                            }

                            BuildPolygon(polygon,
                                         config.roadsHeight * tile.invScale,
                                         mesh->vertices, mesh->indices, 0.0f,
                                         tile.invScale);

                            if (texData)
                            {
                                for (auto it =
                                         mesh->vertices.begin() + vertexOffset;
                                     it != mesh->vertices.end(); ++it)
                                {
                                    it->position.z +=
                                        SampleElevation(
                                            glm::vec2(it->position.x,
                                                      it->position.y),
                                            texData) *
                                        tile.invScale;
                                }
                            }
                        }

                        if (config.normals && config.terrain)
                        {
                            ComputeNormals(*mesh);
                        }
                    }

                    // Add local mesh offset
                    mesh->offset = offset;
                    meshes.push_back(std::move(mesh));
                }
            }
        }
    }

    std::string outFile;

    if (!config.fileName.empty())
    {
        outFile = config.fileName;
    }
    else
    {
        outFile = std::to_string(origin.x) + "." + std::to_string(origin.y) +
                  "." + std::to_string(origin.z);
    }

    std::string outputOBJ = outFile + ".obj";

    // Save output OBJ file
    OBJExporter exporter;
    exporter.Save(outputOBJ, config.splitMesh, meshes, config.offsetX,
                  config.offsetY, config.append, config.normals);
}

void Geometry::BuildPlane(std::vector<PolygonVertex>& outVertices,
                          std::vector<unsigned int>& outIndices, int width,
                          int height, int nw, int nh, bool flip)
{
    std::vector<glm::dvec4> vertices;
    std::vector<int> indices;

    int indexOffset = 0;

    const double ow = static_cast<double>(width) / nw;
    const double oh = static_cast<double>(height) / nh;
    glm::dvec3 normal(0.0, 0.0, 1.0);

    if (flip)
    {
        normal *= -1.f;
    }

    for (int w = -nw * width; w <= (nw - 2) * width; w += 2 * width)
    {
        for (int h = -nh * height; h <= (nh - 2) * height; h += 2 * height)
        {
            const double dw = static_cast<double>(w) / (2 * nw);
            const double dh = static_cast<double>(h) / (2 * nh);

            const glm::dvec3 v0(dw, dh + oh, 0.0);
            const glm::dvec3 v1(dw, dh, 0.0);
            const glm::dvec3 v2(dw + ow, dh, 0.0);
            const glm::dvec3 v3(dw + ow, dh + oh, 0.0);

            outVertices.push_back({ v0, normal });
            outVertices.push_back({ v1, normal });
            outVertices.push_back({ v2, normal });
            outVertices.push_back({ v3, normal });

            if (!flip)
            {
                outIndices.push_back(indexOffset + 0);
                outIndices.push_back(indexOffset + 1);
                outIndices.push_back(indexOffset + 2);
                outIndices.push_back(indexOffset + 0);
                outIndices.push_back(indexOffset + 2);
                outIndices.push_back(indexOffset + 3);
            }
            else
            {
                outIndices.push_back(indexOffset + 0);
                outIndices.push_back(indexOffset + 2);
                outIndices.push_back(indexOffset + 1);
                outIndices.push_back(indexOffset + 0);
                outIndices.push_back(indexOffset + 3);
                outIndices.push_back(indexOffset + 2);
            }

            indexOffset += 4;
        }
    }
}

void Geometry::BuildPedestalPlanes(const Tile& tile,
                                   std::vector<PolygonVertex>& outVertices,
                                   std::vector<unsigned int>& outIndices,
                                   const std::unique_ptr<HeightData>& elevation,
                                   int subDiv, double pedestalHeight)
{
    double offset = 1.0 / subDiv;
    auto vertexDataOffset = static_cast<unsigned int>(outVertices.size());

    for (size_t i = 0; i < tile.borders.size(); ++i)
    {
        if (!tile.borders[i])
        {
            continue;
        }

        for (int x = -subDiv; x < subDiv; x += 1)
        {
            double dx = static_cast<double>(x) / subDiv;

            static const glm::dvec3 upVector(0.0, 0.0, 1.0);
            glm::dvec3 v0, v1;

            if (i == Border::Right)
            {
                v0 = glm::dvec3(1.0, dx + offset, 0.0);
                v1 = glm::dvec3(1.0, dx, 0.0);
            }

            if (i == Border::Left)
            {
                v0 = glm::dvec3(-1.0, dx + offset, 0.0);
                v1 = glm::dvec3(-1.0, dx, 0.0);
            }

            if (i == Border::Top)
            {
                v0 = glm::dvec3(dx + offset, 1.0, 0.0);
                v1 = glm::dvec3(dx, 1.0, 0.0);
            }

            if (i == Border::Bottom)
            {
                v0 = glm::dvec3(dx + offset, -1.0, 0.0);
                v1 = glm::dvec3(dx, -1.0, 0.0);
            }

            glm::dvec3 normalVector;

            normalVector = glm::cross(upVector, v0 - v1);
            normalVector = glm::normalize(normalVector);

            double h0 = SampleElevation(glm::dvec2(v0.x, v0.y), elevation);
            double h1 = SampleElevation(glm::dvec2(v1.x, v1.y), elevation);

            v0.z = h0 * tile.invScale;
            outVertices.push_back({ v0, normalVector });
            v1.z = h1 * tile.invScale;
            outVertices.push_back({ v1, normalVector });
            v0.z = pedestalHeight * tile.invScale;
            outVertices.push_back({ v0, normalVector });
            v1.z = pedestalHeight * tile.invScale;
            outVertices.push_back({ v1, normalVector });

            if (i == Border::Right || i == Border::Bottom)
            {
                outIndices.push_back(vertexDataOffset + 0);
                outIndices.push_back(vertexDataOffset + 1);
                outIndices.push_back(vertexDataOffset + 2);
                outIndices.push_back(vertexDataOffset + 1);
                outIndices.push_back(vertexDataOffset + 3);
                outIndices.push_back(vertexDataOffset + 2);
            }
            else
            {
                outIndices.push_back(vertexDataOffset + 0);
                outIndices.push_back(vertexDataOffset + 2);
                outIndices.push_back(vertexDataOffset + 1);
                outIndices.push_back(vertexDataOffset + 1);
                outIndices.push_back(vertexDataOffset + 2);
                outIndices.push_back(vertexDataOffset + 3);
            }

            vertexDataOffset += 4;
        }
    }
}

double Geometry::BuildPolygonExtrusion(
    const Polygon& polygon, double minHeight, double height,
    std::vector<PolygonVertex>& outVertices,
    std::vector<unsigned int>& outIndices,
    const std::unique_ptr<HeightData>& elevation, double inverseTileScale)
{
    auto vertexDataOffset = static_cast<unsigned int>(outVertices.size());
    const glm::dvec3 upVector(0.0, 0.0, 1.0);
    double minZ = 0.0;
    double cz = 0.0;

    // Compute min and max height of the polygon
    if (elevation)
    {
        // The polygon centroid height
        cz = SampleElevation(GetCentroid(polygon), elevation);
        minZ = std::numeric_limits<float>::max();

        for (auto& line : polygon)
        {
            for (const auto& point : line)
            {
                double pz =
                    SampleElevation(glm::dvec2(point.x, point.y), elevation);

                minZ = std::min(minZ, pz);
            }
        }
    }

    for (auto& line : polygon)
    {
        const size_t lineSize = line.size();

        outVertices.reserve(outVertices.size() + lineSize * 4);
        outIndices.reserve(outIndices.size() + lineSize * 6);

        for (size_t i = 0; i < lineSize - 1; i++)
        {
            glm::dvec3 a(line[i]);
            glm::dvec3 b(line[i + 1]);

            if (a == b)
            {
                continue;
            }

            glm::dvec3 normalVector = glm::cross(upVector, b - a);
            normalVector = glm::normalize(normalVector);

            a.z = height + cz * inverseTileScale;
            outVertices.push_back({ a, normalVector });
            b.z = height + cz * inverseTileScale;
            outVertices.push_back({ b, normalVector });
            a.z = minHeight + minZ * inverseTileScale;
            outVertices.push_back({ a, normalVector });
            b.z = minHeight + minZ * inverseTileScale;
            outVertices.push_back({ b, normalVector });

            outIndices.push_back(vertexDataOffset + 0);
            outIndices.push_back(vertexDataOffset + 1);
            outIndices.push_back(vertexDataOffset + 2);
            outIndices.push_back(vertexDataOffset + 1);
            outIndices.push_back(vertexDataOffset + 3);
            outIndices.push_back(vertexDataOffset + 2);

            vertexDataOffset += 4;
        }
    }

    return cz;
}

void Geometry::ParseTiles(const std::string& tileX, const std::string& tileY,
    int tileZ)
{
}

void Geometry::BuildPolygon(const Polygon& polygon, double height,
                            std::vector<PolygonVertex>& outVertices,
                            std::vector<unsigned int>& outIndices,
                            double centroidHeight, double inverseTileScale)
{
    std::vector<std::vector<std::array<double, 2>>> poly;
    for (auto& line : polygon)
    {
        std::vector<std::array<double, 2>> points;
        points.reserve(line.size());

        for (const auto& point : line)
        {
            points.push_back({ point.x, point.y });
        }

        poly.push_back(points);
    }
    mapbox::detail::Earcut<unsigned int> earcut;
    earcut(poly);

    const auto vertexDataOffset = static_cast<unsigned int>(outVertices.size());

    if (earcut.indices.empty())
    {
        return;
    }

    if (vertexDataOffset == 0)
    {
        outIndices = std::move(earcut.indices);
    }
    else
    {
        outIndices.reserve(outIndices.size() + earcut.indices.size());

        for (auto i : earcut.indices)
        {
            outIndices.push_back(vertexDataOffset + i);
        }
    }

    static glm::vec3 normal(0.0, 0.0, 1.0);

    outVertices.reserve(outVertices.size() + earcut.vertices);

    centroidHeight *= inverseTileScale;

    for (const auto& line : polygon)
    {
        for (const auto& vertex : line)
        {
            const glm::dvec2 position(vertex[0], vertex[1]);
            const glm::dvec3 coord(position.x, position.y,
                                   height + centroidHeight);
            outVertices.push_back({ coord, normal });
        }
    }
}

void Geometry::AddPolygonPolylinePoint(Line& line, glm::dvec3 cur,
                                       glm::dvec3 next, glm::dvec3 last,
                                       double extrude, size_t lineDataSize,
                                       size_t i, bool forward)
{
    glm::dvec3 n0 = GetPerp(cur - last);
    glm::dvec3 n1 = GetPerp(next - cur);
    bool right = glm::cross(n1, n0).z > 0.0;

    if ((i == 1 && forward) || (i == lineDataSize - 2 && !forward))
    {
        line.push_back(last + n0 * extrude);
        line.push_back(last - n0 * extrude);
    }

    if (right)
    {
        glm::dvec3 d0 = glm::normalize(last - cur);
        glm::dvec3 d1 = glm::normalize(next - cur);
        glm::dvec3 miter = ComputeMiterVector(d0, d1, n0, n1);
        line.push_back(cur - miter * extrude);
    }
    else
    {
        line.push_back(cur - n0 * extrude);
        line.push_back(cur - n1 * extrude);
    }
}
}  // namespace CubbyCity