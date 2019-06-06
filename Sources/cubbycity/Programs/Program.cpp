// Copyright (c) 2019 Chris Ohk, Paul Kweon, Den So, Edward Sung

// We are making my contributions/submissions to this project solely in our
// personal capacity and are not conveying any rights to any intellectual
// property of any third parties.

#include <CubbyCity/Programs/Program.hpp>

namespace CubbyCity
{
Program::Program(ProgramConfig config) : m_config(std::move(config))
{
    // Do nothing
}

void Program::Process()
{
    m_geometry.ParseTiles(m_config.tileX, m_config.tileY, m_config.tileZ);
    m_geometry.DownloadData(m_config.apiKey, m_config.terrain,
                            m_config.terrainExtrusionScale, m_config.buildings,
                            m_config.roads);

    // Adjust terrain edges
    if (m_config.terrain)
    {
    }
}
}  // namespace CubbyCity