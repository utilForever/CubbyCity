// Copyright (c) 2019 Chris Ohk, Paul Kweon, Den So, Edward Sung

// We are making my contributions/submissions to this project solely in our
// personal capacity and are not conveying any rights to any intellectual
// property of any third parties.

#ifndef CUBBYCITY_DOWNLOAD_UTILS_HPP
#define CUBBYCITY_DOWNLOAD_UTILS_HPP

#include <CubbyCity/Geometry/Tile.hpp>

#include <string>

namespace CubbyCity
{
inline std::string GetVectorTileURL(const Tile& tile, const std::string& apiKey)
{
    return "https://tile.nextzen.org/tilezen/vector/v1/256/all/" +
           std::to_string(tile.z) + "/" + std::to_string(tile.x) + "/" +
           std::to_string(tile.y) + ".json?api_key=" + apiKey;
}

inline std::string GetTerrainURL(const Tile& tile, const std::string& apiKey)
{
    return "https://tile.nextzen.org/tilezen/terrain/v1/260/terrarium/" +
           std::to_string(tile.z) + "/" + std::to_string(tile.x) + "/" +
           std::to_string(tile.y) + ".png?api_key=" + apiKey;
}
}  // namespace CubbyCity

#endif  // CUBBYCITY_DOWNLOAD_UTILS_HPP