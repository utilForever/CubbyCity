// Copyright (c) 2019 Chris Ohk, Paul Kweon, Den So, Edward Sung

// We are making my contributions/submissions to this project solely in our
// personal capacity and are not conveying any rights to any intellectual
// property of any third parties.

#include <CubbyCity/Exporter/OBJExporter.hpp>

#include <fstream>
#include <sstream>

namespace CubbyCity
{
void OBJExporter::Save(const std::string& outputOBJ, bool splitMeshes,
                       std::vector<std::unique_ptr<PolygonMesh>>& meshes,
                       double offsetX, double offsetY, bool append,
                       bool normals)
{
    size_t maxIndex = 0;

    // Find max index from previously existing wavefront vertices
    std::ifstream inFile(outputOBJ.c_str(), std::ios::in);
    std::string token;

    if (inFile.good() && append)
    {
        while (!inFile.eof())
        {
            inFile >> token;
            if (token == "f")
            {
                std::string faceLine;
                getline(inFile, faceLine);

                for (char& ch : faceLine)
                {
                    if (ch == '/')
                    {
                        ch = ' ';
                    }
                }

                std::stringstream ss(faceLine);
                std::string faceToken;
                char* stop;

                for (int i = 0; i < 6; ++i)
                {
                    ss >> faceToken;
                    if (faceToken.find_first_not_of("\t\n ") !=
                        std::string::npos)
                    {
                        size_t index =
                            std::strtol(faceToken.c_str(), &stop, 10);
                        maxIndex = index > maxIndex ? index : maxIndex;
                    }
                }
            }
        }

        inFile.close();
    }

    // Save OBJ file
    std::ofstream file(outputOBJ);

    if (append)
    {
        file.seekp(std::ios_base::end);
    }

    if (file.is_open())
    {
        size_t nVertex = 0;
        size_t nTriangles = 0;

        file << "# exported with CubbyCity: "
                "https://github.com/utilForever/CubbyCity"
             << "\n";
        file << "\n";

        size_t indexOffset = maxIndex;

        if (splitMeshes)
        {
            int meshCnt = 0;

            for (const auto& mesh : meshes)
            {
                if (mesh->vertices.empty())
                {
                    continue;
                }

                file << "o mesh" << meshCnt++ << "\n";

                AddPositions(file, *mesh, offsetX, offsetY);
                nVertex += mesh->vertices.size();

                if (normals)
                {
                    AddNormals(file, *mesh);
                }

                AddFaces(file, *mesh, indexOffset, normals);
                nTriangles += mesh->indices.size() / 3;

                file << "\n";

                indexOffset += mesh->vertices.size();
            }
        }
        else
        {
            file << "o " << outputOBJ << "\n";

            for (const auto& mesh : meshes)
            {
                if (mesh->vertices.empty())
                {
                    continue;
                }

                AddPositions(file, *mesh, offsetX, offsetY);
                nVertex += mesh->vertices.size();
            }

            if (normals)
            {
                for (const auto& mesh : meshes)
                {
                    if (mesh->vertices.empty())
                    {
                        continue;
                    }

                    AddNormals(file, *mesh);
                }
            }

            for (const auto& mesh : meshes)
            {
                if (mesh->vertices.empty())
                {
                    continue;
                }

                AddFaces(file, *mesh, indexOffset, normals);
                indexOffset += mesh->vertices.size();
                nTriangles += mesh->indices.size() / 3;
            }
        }

        file.close();
    }
}

void OBJExporter::AddPositions(std::ostream& file, const PolygonMesh& mesh,
                               double offsetX, double offsetY)
{
    for (auto vertex : mesh.vertices)
    {
        file << "v " << vertex.position.x + offsetX + mesh.offset.x << " "
             << vertex.position.y + offsetY + mesh.offset.y << " "
             << vertex.position.z << "\n";
    }
}

void OBJExporter::AddNormals(std::ostream& file, const PolygonMesh& mesh)
{
    for (auto vertex : mesh.vertices)
    {
        file << "vn " << vertex.normal.x << " " << vertex.normal.y << " "
             << vertex.normal.z << "\n";
    }
}

void OBJExporter::AddFaces(std::ostream& file, const PolygonMesh& mesh,
                           size_t indexOffset, bool normals)
{
    for (int i = 0; i < mesh.indices.size(); i += 3)
    {
        file << "f " << mesh.indices[i] + indexOffset + 1
             << (normals
                     ? "//" + std::to_string(mesh.indices[i] + indexOffset + 1)
                     : "");
        file << " ";
        file << mesh.indices[i + 1] + indexOffset + 1
             << (normals ? "//" + std::to_string(mesh.indices[i + 1] +
                                                 indexOffset + 1)
                         : "");
        file << " ";
        file << mesh.indices[i + 2] + indexOffset + 1
             << (normals ? "//" + std::to_string(mesh.indices[i + 2] +
                                                 indexOffset + 1)
                         : "");
        file << "\n";
    }
}
}  // namespace CubbyCity