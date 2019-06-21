// Copyright (c) 2019 Chris Ohk, Paul Kweon, Den So, Edward Sung

// We are making my contributions/submissions to this project solely in our
// personal capacity and are not conveying any rights to any intellectual
// property of any third parties.

#ifndef CUBBYCITY_GEOMETRY_DATA_HPP
#define CUBBYCITY_GEOMETRY_DATA_HPP

#include <glm/glm.hpp>

#include <map>
#include <string>
#include <utility>
#include <vector>

namespace CubbyCity
{
using Point = glm::dvec3;
using Line = std::vector<Point>;
using Polygon = std::vector<Line>;

enum class GeometryType
{
    Unknown,
    Points,
    Lines,
    Polygons
};

struct Properties
{
    std::map<std::string, double> numericProps;
};

struct Feature
{
    GeometryType geometryType = GeometryType::Polygons;

    std::vector<Point> points;
    std::vector<Line> lines;
    std::vector<Polygon> polygons;

    Properties props;
};

struct Layer
{
    explicit Layer(const std::string& _name) : name(std::move(_name))
    {
        // Do nothing
    }

    std::string name;
    std::vector<Feature> features;
};

struct TileData
{
    std::vector<Layer> layers;
};

struct HeightData
{
    int width;
    int height;
    std::vector<std::vector<double>> elevation;
};

struct PolygonVertex
{
    glm::dvec3 position;
    glm::dvec3 normal;
};

struct PolygonMesh
{
    std::vector<unsigned int> indices;
    std::vector<PolygonVertex> vertices;
    glm::dvec2 offset;
};
}  // namespace CubbyCity

#endif  // CUBBYCITY_GEOMETRY_DATA_HPP