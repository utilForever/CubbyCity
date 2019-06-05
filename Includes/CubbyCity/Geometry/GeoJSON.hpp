// Copyright (c) 2019 Chris Ohk, Paul Kweon, Den So, Edward Sung

// We are making my contributions/submissions to this project solely in our
// personal capacity and are not conveying any rights to any intellectual
// property of any third parties.

#ifndef CUBBYCITY_GEO_JSON_HPP
#define CUBBYCITY_GEO_JSON_HPP

#include <CubbyCity/Geometry/GeometryData.hpp>
#include <CubbyCity/Geometry/Tile.hpp>

#include <json/json.hpp>

namespace CubbyCity
{
class GeoJSON
{
 public:
    static bool ExtractPoint(const nlohmann::json::value_type& in, Point& out,
                             const Tile& tile, Point* last = nullptr);
    static void ExtractLine(const nlohmann::json::value_type& in, Line& out,
                            const Tile& tile);
    static void ExtractPolygon(const nlohmann::json::value_type& in,
                               Polygon& out, const Tile& tile);
    static void ExtractFeature(const nlohmann::json::value_type& in,
                               Feature& out, const Tile& tile);
    static void ExtractLayer(const nlohmann::json::value_type& in, Layer& out,
                             const Tile& tile);
};
}  // namespace CubbyCity

#endif  // CUBBYCITY_GEO_JSON_HPP