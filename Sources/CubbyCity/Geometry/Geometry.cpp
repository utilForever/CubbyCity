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
        offset.x = static_cast<float>((tile.x - origin.x)) * 2.0f;
        offset.y = static_cast<float>(-(tile.y - origin.y)) * 2.0f;

        const auto& texData = m_heightData[tile];

        (void)offset;

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
                const glm::vec2 tilePosition =
                    glm::vec2(vertex.position.x, vertex.position.y);
                const float extrusion = SampleElevation(tilePosition, texData);

                // Scale the height within the tile scale
                vertex.position.z =
                    extrusion * static_cast<float>(tile.invScale);
            }

            // Compute faces normals
            if (config.normals)
            {
                ComputeNormals(*mesh);
            }

            mesh->offset = offset;
            meshes.push_back(std::move(mesh));
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
    std::vector<glm::vec4> vertices;
    std::vector<int> indices;

    int indexOffset = 0;

    const float ow = static_cast<float>(width) / static_cast<float>(nw);
    const float oh = static_cast<float>(height) / static_cast<float>(nh);
    glm::vec3 normal(0.0, 0.0, 1.0);

    if (flip)
    {
        normal *= -1.f;
    }

    for (int w = -nw * width; w <= (nw - 2) * width; w += 2 * width)
    {
        for (int h = -nh * height; h <= (nh - 2) * height; h += 2 * height)
        {
            const float dw = static_cast<float>(w) / static_cast<float>(2 * nw);
            const float dh = static_cast<float>(h) / static_cast<float>(2 * nh);

            const glm::vec3 v0(dw, dh + oh, 0.0);
            const glm::vec3 v1(dw, dh, 0.0);
            const glm::vec3 v2(dw + ow, dh, 0.0);
            const glm::vec3 v3(dw + ow, dh + oh, 0.0);

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

float Geometry::SampleElevation(
    glm::vec2 position, const std::unique_ptr<HeightData>& texData) const
{
    if (!texData)
    {
        return 0.0;
    }

    if (!IsWithinTileRange(position))
    {
        position = glm::clamp(position, glm::vec2(-1.0), glm::vec2(1.0));
    }

    // Normalize vertex coordinates into the texture coordinates range
    const float u =
        (position.x * 0.5f + 0.5f) * static_cast<float>(texData->width);
    float v = (position.y * 0.5f + 0.5f) * static_cast<float>(texData->height);

    // Flip v coordinate according to tile coordinates
    v = static_cast<float>(texData->height) - v;

    const float alpha = u - floor(u);
    const float beta = v - floor(v);

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
    const float s0 = texData->elevation[ii0][jj0];
    const float s1 = texData->elevation[ii0][jj1];
    const float s2 = texData->elevation[ii1][jj0];
    const float s3 = texData->elevation[ii1][jj1];

    // Sample the bilinear height from the elevation texture
    const float bilinearHeight = (1 - beta) * (1 - alpha) * s0 +
                                 (1 - beta) * alpha * s1 +
                                 beta * (1 - alpha) * s2 + alpha * beta * s3;

    return bilinearHeight;
}
}  // namespace CubbyCity