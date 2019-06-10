// Copyright (c) 2019 Chris Ohk, Paul Kweon, Den So, Edward Sung

// We are making my contributions/submissions to this project solely in our
// personal capacity and are not conveying any rights to any intellectual
// property of any third parties.

#ifndef CUBBYCITY_WIN_DOWNLOADER_HPP
#define CUBBYCITY_WIN_DOWNLOADER_HPP

#include <CubbyCity/Platform/Downloader.hpp>

namespace CubbyCity
{
class WinDownloader : public Downloader
{
 public:
    bool DownloadData(std::string& out, const std::string& url) override;
};
}  // namespace CubbyCity

#endif  // CUBBYCITY_WIN_DOWNLOADER_HPP