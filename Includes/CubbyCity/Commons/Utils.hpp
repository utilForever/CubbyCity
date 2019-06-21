// Copyright (c) 2019 Chris Ohk, Paul Kweon, Den So, Edward Sung

// We are making my contributions/submissions to this project solely in our
// personal capacity and are not conveying any rights to any intellectual
// property of any third parties.

#ifndef CUBBYCITY_UTILS_HPP
#define CUBBYCITY_UTILS_HPP

#include <sstream>
#include <string>
#include <vector>

namespace CubbyCity
{
inline std::vector<std::string> SplitString(const std::string& s, const char delim)
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
}  // namespace CubbyCity

#endif  // CUBBYCITY_UTILS_HPP