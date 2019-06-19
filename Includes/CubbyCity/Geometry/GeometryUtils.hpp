// Copyright (c) 2019 Chris Ohk, Paul Kweon, Den So, Edward Sung

// We are making my contributions/submissions to this project solely in our
// personal capacity and are not conveying any rights to any intellectual
// property of any third parties.

#ifndef CUBBYCITY_GEOMETRY_UTILS_HPP
#define CUBBYCITY_GEOMETRY_UTILS_HPP

#include <CubbyCity/Commons/Constants.hpp>

#include <glm/glm.hpp>

namespace CubbyCity
{
inline glm::dvec2 ConvertLonLatToMeters(const glm::dvec2 lonLat)
{
    glm::dvec2 meters;
    meters.x = lonLat.x * HALF_CIRCUMFERENCE * INV_180;
    meters.y = log(tan(MATH_PI * 0.25 + lonLat.y * MATH_PI * INV_360)) *
               static_cast<double>(RADIUS_EARTH);

    return meters;
}

inline glm::dvec2 ConvertPixelToMeters(const glm::dvec2 pixel, const int zoom,
                                       double invTileSize)
{
    glm::dvec2 meters;
    const double res = (2.0 * HALF_CIRCUMFERENCE * invTileSize) / (1 << zoom);
    meters.x = pixel.x * res - HALF_CIRCUMFERENCE;
    meters.y = pixel.y * res - HALF_CIRCUMFERENCE;

    return meters;
}

inline glm::dvec4 GetTileBounds(int x, int y, int z, double tileSize)
{
    return glm::dvec4(
        ConvertPixelToMeters({ x * tileSize, y * tileSize }, z, 1.0 / tileSize),
        ConvertPixelToMeters({ (x + 1) * tileSize, (y + 1) * tileSize }, z,
                             1.0 / tileSize));
}

inline bool IsWithinTileRange(const glm::vec2& pos)
{
    return pos.x >= -1.0 && pos.x <= 1.0 && pos.y >= -1.0 && pos.y <= 1.0;
}

inline void ComputeNormals(PolygonMesh& mesh)
{
    for (size_t i = 0; i < mesh.indices.size(); i += 3)
    {
        const int i1 = mesh.indices[i + 0];
        const int i2 = mesh.indices[i + 1];
        const int i3 = mesh.indices[i + 2];

        const glm::vec3& v1 = mesh.vertices[i1].position;
        const glm::vec3& v2 = mesh.vertices[i2].position;
        const glm::vec3& v3 = mesh.vertices[i3].position;

        const glm::vec3 d = glm::normalize(glm::cross(v2 - v1, v3 - v1));

        mesh.vertices[i1].normal += d;
        mesh.vertices[i2].normal += d;
        mesh.vertices[i3].normal += d;
    }

    for (auto& v : mesh.vertices)
    {
        v.normal = glm::normalize(v.normal);
    }
}

inline glm::dvec2 GetCentroid(const std::vector<std::vector<glm::dvec3>>& polygon)
{
    glm::dvec2 centroid;
    int n = 0;

    for (auto& line : polygon)
    {
        for (auto& point : line)
        {
            centroid.x += point.x;
            centroid.y += point.y;
            n++;
        }
    }

    if (n == 0)
    {
        return centroid;
    }

    centroid /= n;
    return centroid;
}
}  // namespace CubbyCity

#endif  // CUBBYCITY_GEOMETRY_UTILS_HPP