// Copyright (c) 2019 Chris Ohk, Paul Kweon, Den So, Edward Sung

// We are making my contributions/submissions to this project solely in our
// personal capacity and are not conveying any rights to any intellectual
// property of any third parties.

#include <CubbyCity/Platform/WindowsDownloader.hpp>

#include <iostream>

using namespace CubbyCity;

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        return EXIT_FAILURE;
    }

    WindowsDownloader downloader;
    std::string out;
    std::string url =
        "https://tile.nextzen.org/tilezen/vector/v1/512/all/16/19293/"
        "24641.json?api_key=" +
        std::string(argv[1]);

    downloader.DownloadData(out, url);

    std::cout << out << '\n';

    return 0;
}