// Copyright (c) 2019 Chris Ohk, Paul Kweon, Den So, Edward Sung

// We are making my contributions/submissions to this project solely in our
// personal capacity and are not conveying any rights to any intellectual
// property of any third parties.

#ifndef CUBBYCITY_DOWNLOADER_HPP
#define CUBBYCITY_DOWNLOADER_HPP

#include <string>

namespace CubbyCity
{
class Downloader
{
 public:
    virtual ~Downloader() = default;

    virtual bool DownloadData(std::string& out, const std::string& url) = 0;
};
}  // namespace CubbyCity

#endif  // CUBBYCITY_DOWNLOADER_HPP