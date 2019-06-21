// Copyright (c) 2019 Chris Ohk, Paul Kweon, Den So, Edward Sung

// We are making my contributions/submissions to this project solely in our
// personal capacity and are not conveying any rights to any intellectual
// property of any third parties.

#ifndef CUBBYCITY_IEXPORTER_HPP
#define CUBBYCITY_IEXPORTER_HPP

#include <CubbyCity/Geometry/GeometryData.hpp>

#include <memory>
#include <string>
#include <vector>

namespace CubbyCity
{
class IExporter
{
 public:
    virtual ~IExporter() = default;

    virtual void Save(const std::string& outputOBJ, bool splitMeshes,
                      std::vector<std::unique_ptr<PolygonMesh>>& meshes,
                      double offsetX, double offsetY, bool append,
                      bool normals) = 0;
};
}  // namespace CubbyCity

#endif  // CUBBYCITY_IEXPORTER_HPP