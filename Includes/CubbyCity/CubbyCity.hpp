// Copyright (c) 2019 Chris Ohk, Paul Kweon, Den So, Edward Sung

// We are making my contributions/submissions to this project solely in our
// personal capacity and are not conveying any rights to any intellectual
// property of any third parties.

#ifndef CUBBYCITY_HPP
#define CUBBYCITY_HPP

#include <CubbyCity/Geometry/Tile.hpp>

#include <string>

namespace CubbyCity
{
class CubbyCity
{
 public:
    void Process();

 private:
    std::string GetVectorTileURL(const Tile& tile,
                                 const std::string& apiKey) const;
    std::string GetTerrainURL(const Tile& tile,
                              const std::string& apiKey) const;
};
}  // namespace CubbyCity

#endif  // CUBBYCITY_HPP