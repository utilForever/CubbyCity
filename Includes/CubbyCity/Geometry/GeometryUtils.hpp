// Copyright (c) 2019 Chris Ohk, Paul Kweon, Den So, Edward Sung

// We are making my contributions/submissions to this project solely in our
// personal capacity and are not conveying any rights to any intellectual
// property of any third parties.

#ifndef CUBBYCITY_GEOMETRY_UTILS_HPP
#define CUBBYCITY_GEOMETRY_UTILS_HPP

#include <CubbyCity/Commons/Constants.hpp>
#include <CubbyCity/Geometry/TileUtils.hpp>

#include <glm/glm.hpp>

#include <algorithm>
#include <memory>

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

inline double SampleElevation(glm::dvec2 position,
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

inline void ComputeNormals(PolygonMesh& mesh)
{
    for (size_t i = 0; i < mesh.indices.size(); i += 3)
    {
        const int i1 = mesh.indices[i + 0];
        const int i2 = mesh.indices[i + 1];
        const int i3 = mesh.indices[i + 2];

        const glm::dvec3& v1 = mesh.vertices[i1].position;
        const glm::dvec3& v2 = mesh.vertices[i2].position;
        const glm::dvec3& v3 = mesh.vertices[i3].position;

        const glm::dvec3 d = glm::normalize(glm::cross(v2 - v1, v3 - v1));

        mesh.vertices[i1].normal += d;
        mesh.vertices[i2].normal += d;
        mesh.vertices[i3].normal += d;
    }

    for (auto& v : mesh.vertices)
    {
        v.normal = glm::normalize(v.normal);
    }
}

inline glm::dvec2 GetCentroid(
    const std::vector<std::vector<glm::dvec3>>& polygon)
{
    glm::dvec2 centroid{};
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

inline glm::dvec3 GetPerp(const glm::dvec3& v)
{
    return glm::normalize(glm::dvec3(-v.y, v.x, 0.0));
}

inline glm::dvec3 ComputeMiterVector(const glm::dvec3& d0, const glm::dvec3& d1,
                                     const glm::dvec3& n0, const glm::dvec3& n1)
{
    glm::dvec3 miter = glm::normalize(n0 + n1);
    const double miterl2 = glm::dot(miter, miter);

    if (miterl2 < std::numeric_limits<double>::epsilon())
    {
        miter = glm::dvec3(n1.y - n0.y, n0.x - n1.x, 0.0);
    }
    else
    {
        double theta = atan2(d1.y, d1.x) - atan2(d0.y, d0.x);
        if (theta < 0.0)
        {
            theta += 2 * MATH_PI;
        }

        miter *= 1.f / std::max<double>(sin(theta * 0.5), EPSILON);
    }

    return miter;
}
}  // namespace CubbyCity

#endif  // CUBBYCITY_GEOMETRY_UTILS_HPP