// Copyright (c) 2019 Chris Ohk, Paul Kweon, Den So, Edward Sung

// We are making my contributions/submissions to this project solely in our
// personal capacity and are not conveying any rights to any intellectual
// property of any third parties.

#include <CubbyCity/Geometry/GeoJSON.hpp>

namespace CubbyCity
{
bool GeoJSON::ExtractPoint(const nlohmann::json::value_type& in, Point& out,
                           const Tile& tile, Point* last)
{
    glm::dvec2 pos = ConvertLonLatToMeters(
        glm::dvec2(in[0].get<double>(), in[1].get<double>()));
    out.x = (pos.x - tile.tileOrigin.x) * tile.invScale;
    out.y = (pos.y - tile.tileOrigin.y) * tile.invScale;

    return !(last && glm::length(out - *last) < 1e-5f);
}

void GeoJSON::ExtractLine(const nlohmann::json::value_type& in, Line& out,
                          const Tile& tile)
{
    for (const auto& points : in)
    {
        out.emplace_back();
        if (out.size() > 1)
        {
            if (!ExtractPoint(points, out.back(), tile, &out[out.size() - 2]))
            {
                out.pop_back();
            }
        }
        else
        {
            ExtractPoint(points, out.back(), tile);
        }
    }
}

void GeoJSON::ExtractPolygon(const nlohmann::json::value_type& in, Polygon& out,
                             const Tile& tile)
{
    for (const auto& lines : in)
    {
        out.emplace_back();
        ExtractLine(lines, out.back(), tile);
    }
}

void GeoJSON::ExtractFeature(const nlohmann::json::value_type& in, Feature& out,
                             const Tile& tile)
{
    const nlohmann::json::value_type& properties = in["properties"];

    for (const auto& property : properties.items())
    {
        const auto& member = property.key();
        const nlohmann::json::value_type& prop = properties[member];

        if (member == "height")
        {
            out.props.numericProps[member] = prop.get<double>();
        }
        else if (member == "min_height")
        {
            out.props.numericProps[member] = prop.get<double>();
        }
    }

    // Copy geometry into tile data
    const nlohmann::json::value_type& geometry = in["geometry"];
    const nlohmann::json::value_type& coords = geometry["coordinates"];
    const std::string& geometryType = geometry["type"].get<std::string>();

    if (geometryType == "Point")
    {
        out.geometryType = GeometryType::Points;
        out.points.emplace_back();
        if (!ExtractPoint(coords, out.points.back(), tile))
        {
            out.points.pop_back();
        }
    }
    else if (geometryType == "MultiPoint")
    {
        out.geometryType = GeometryType::Points;
        for (const auto& pointCoords : coords)
        {
            if (!ExtractPoint(pointCoords, out.points.back(), tile))
            {
                out.points.pop_back();
            }
        }
    }
    else if (geometryType == "LineString")
    {
        out.geometryType = GeometryType::Lines;
        out.lines.emplace_back();
        ExtractLine(coords, out.lines.back(), tile);
    }
    else if (geometryType == "MultiLineString")
    {
        out.geometryType = GeometryType::Lines;
        for (const auto& lineCoords : coords)
        {
            out.lines.emplace_back();
            ExtractLine(lineCoords, out.lines.back(), tile);
        }
    }
    else if (geometryType == "Polygon")
    {
        out.geometryType = GeometryType::Polygons;
        out.polygons.emplace_back();
        ExtractPolygon(coords, out.polygons.back(), tile);
    }
    else if (geometryType == "MultiPolygon")
    {
        out.geometryType = GeometryType::Polygons;
        for (const auto& polyCoords : coords)
        {
            out.polygons.emplace_back();
            ExtractPolygon(polyCoords, out.polygons.back(), tile);
        }
    }
}

void GeoJSON::ExtractLayer(const nlohmann::json::value_type& in, Layer& out,
                           const Tile& tile)
{
    const nlohmann::json::value_type& features = in["features"];
    if (features.is_null())
    {
        return;
    }

    for (const auto& feature : features)
    {
        out.features.emplace_back();
        ExtractFeature(feature, out.features.back(), tile);
    }
}
}  // namespace CubbyCity