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

double Geometry::SampleElevation(glm::dvec2 position,
                                 const std::unique_ptr<HeightData>& texData)
{
    if (!texData)
    {
        return 0.0;
    }

    if (!IsWithinTileRange(position))
    {
        position = glm::clamp(position, glm::dvec2(-1.0), glm::dvec2(1.0));
    }

    // Normalize vertex coordinates into the texture coordinates range
    const double u = (position.x * 0.5 + 0.5) * texData->width;
    double v = (position.y * 0.5 + 0.5) * texData->height;

    // Flip v coordinate according to tile coordinates
    v = static_cast<double>(texData->height) - v;

    const double alpha = u - floor(u);
    const double beta = v - floor(v);

    int ii0 = static_cast<int>(floor(u));
    int jj0 = static_cast<int>(floor(v));
    int ii1 = ii0 + 1;
    int jj1 = jj0 + 1;

    // Clamp on borders
    ii0 = std::min(ii0, texData->width - 1);
    jj0 = std::min(jj0, texData->height - 1);
    ii1 = std::min(ii1, texData->width - 1);
    jj1 = std::min(jj1, texData->height - 1);

    // Sample four corners of the current texel
    const double s0 = texData->elevation[ii0][jj0];
    const double s1 = texData->elevation[ii0][jj1];
    const double s2 = texData->elevation[ii1][jj0];
    const double s3 = texData->elevation[ii1][jj1];

    // Sample the bilinear height from the elevation texture
    const double bilinearHeight = (1 - beta) * (1 - alpha) * s0 +
                                  (1 - beta) * alpha * s1 +
                                  beta * (1 - alpha) * s2 + alpha * beta * s3;

    return bilinearHeight;
}
}  // namespace CubbyCity