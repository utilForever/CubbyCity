// Copyright (c) 2019 Chris Ohk, Paul Kweon, Den So, Edward Sung

// We are making my contributions/submissions to this project solely in our
// personal capacity and are not conveying any rights to any intellectual
// property of any third parties.

#ifndef CUBBYCITY_UTILS_HPP
#define CUBBYCITY_UTILS_HPP

#include <sstream>
#include <string>
#include <tuple>
#include <vector>

namespace CubbyCity
{
inline std::vector<std::string> SplitString(const std::string& s, char delim)
{
    std::vector<std::string> elems;
    std::stringstream ss(s);
    std::string item;

    while (std::getline(ss, item, delim))
    {
        elems.emplace_back(item);
    }

    return elems;
}

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
}  // namespace CubbyCity

#endif  // CUBBYCITY_UTILS_HPP