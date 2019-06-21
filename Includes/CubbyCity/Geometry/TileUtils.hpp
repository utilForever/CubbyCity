// Copyright (c) 2019 Chris Ohk, Paul Kweon, Den So, Edward Sung

// We are making my contributions/submissions to this project solely in our
// personal capacity and are not conveying any rights to any intellectual
// property of any third parties.

#ifndef CUBBYCITY_TILE_UTILS_HPP
#define CUBBYCITY_TILE_UTILS_HPP

#include <CubbyCity/Commons/CommonUtils.hpp>

#include <glm/glm.hpp>

#include <vector>
#include <tuple>
#include <string>

namespace CubbyCity
{
inline std::tuple<int, int> ExtractTileRange(const std::string& range)
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

inline bool IsWithinTileRange(const glm::vec2& pos)
{
    return pos.x >= -1.0 && pos.x <= 1.0 && pos.y >= -1.0 && pos.y <= 1.0;
}
}  // namespace CubbyCity

#endif  // CUBBYCITY_TILE_UTILS_HPP