// Copyright (c) 2019 Chris Ohk, Paul Kweon, Den So, Edward Sung

// We are making my contributions/submissions to this project solely in our
// personal capacity and are not conveying any rights to any intellectual
// property of any third parties.

#ifndef CUBBYCITY_DOWNLOAD_UTILS_HPP
#define CUBBYCITY_DOWNLOAD_UTILS_HPP

#define STB_IMAGE_IMPLEMENTATION

#include <CubbyCity/Geometry/GeoJSON.hpp>
#include <CubbyCity/Geometry/Tile.hpp>
#include <CubbyCity/Platform/WinDownloader.hpp>

#include <stb/stb_image.h>
#include <json/json.hpp>

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

inline std::unique_ptr<HeightData> DownloadHeightmapTile(const std::string& url,
                                                         float extrusionScale)
{
    WinDownloader downloader;
    std::string out;

    if (downloader.DownloadData(out, url))
    {
        int width, height, comp;

        // Decode texture PNG
        const auto* pngData =
            reinterpret_cast<const unsigned char*>(out.c_str());
        unsigned char* pixels =
            stbi_load_from_memory(pngData, static_cast<int>(out.length()),
                                  &width, &height, &comp, STBI_rgb_alpha);

        if (comp != STBI_rgb_alpha)
        {
            return nullptr;
        }

        std::unique_ptr<HeightData> data = std::make_unique<HeightData>();

        data->elevation.resize(height);
        for (int i = 0; i < height; ++i)
        {
            data->elevation[i].resize(width);
        }

        data->width = width;
        data->height = height;

        unsigned char* pixel = pixels;
        for (int i = 0; i < width * height; ++i, pixel += 4)
        {
            const float red = *(pixel + 0);
            const float green = *(pixel + 1);
            const float blue = *(pixel + 2);

            // Decode the elevation packed data from color component
            const float elevation = (red * 256 + green + blue / 256) - 32768;

            const int y = i / height;
            const int x = i % width;

            assert(x >= 0 && x <= width && y >= 0 && y <= height);

            data->elevation[x][y] = elevation * extrusionScale;
        }

        return data;
    }

    return nullptr;
}

inline std::unique_ptr<TileData> DownloadTile(const std::string& url,
                                              const Tile& tile)
{
    WinDownloader downloader;
    std::string out;

    if (downloader.DownloadData(out, url))
    {
        // Parse written data into a JSON object
        nlohmann::json j = nlohmann::json::parse(out);

        if (j.is_null())
        {
            return nullptr;
        }

        std::unique_ptr<TileData> data = std::make_unique<TileData>();
        for (auto layer = j.begin(); layer != j.end(); ++layer)
        {
            data->layers.emplace_back(std::string(layer.key()));
            GeoJSON::ExtractLayer(layer.value(), data->layers.back(), tile);
        }

        return data;
    }

    return nullptr;
}
}  // namespace CubbyCity

#endif  // CUBBYCITY_DOWNLOAD_UTILS_HPP