// Copyright (c) 2019 Chris Ohk, Paul Kweon, Den So, Edward Sung

// We are making my contributions/submissions to this project solely in our
// personal capacity and are not conveying any rights to any intellectual
// property of any third parties.

#ifndef CUBBYCITY_DOWNLOADER_H
#define CUBBYCITY_DOWNLOADER_H

#include <string>

namespace CubbyCity
{
class Downloader
{
 public:
    virtual bool DownloadData(std::string& out, const std::string& url) = 0;
};
}  // namespace CubbyCity

#endif