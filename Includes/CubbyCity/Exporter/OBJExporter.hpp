// Copyright (c) 2019 Chris Ohk, Paul Kweon, Den So, Edward Sung

// We are making my contributions/submissions to this project solely in our
// personal capacity and are not conveying any rights to any intellectual
// property of any third parties.

#ifndef CUBBYCITY_EXPORT_UTILS_HPP
#define CUBBYCITY_EXPORT_UTILS_HPP

#include <CubbyCity/Exporter/IExporter.hpp>

namespace CubbyCity
{
class OBJExporter : public IExporter
{
 public:
    void Save(const std::string& outputOBJ, bool splitMeshes,
              std::vector<std::unique_ptr<PolygonMesh>>& meshes, double offsetX,
              double offsetY, bool append, bool normals) override;

 private:
    void AddPositions(std::ostream& file, const PolygonMesh& mesh,
                      double offsetX, double offsetY);
    void AddNormals(std::ostream& file, const PolygonMesh& mesh);
    void AddFaces(std::ostream& file, const PolygonMesh& mesh,
                  size_t indexOffset, bool normals);
};
}  // namespace CubbyCity

#endif  // CUBBYCITY_EXPORT_UTILS_HPP