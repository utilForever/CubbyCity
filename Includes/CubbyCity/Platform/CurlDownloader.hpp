// Copyright (c) 2019 Chris Ohk, Paul Kweon, Den So, Edward Sung

// We are making my contributions/submissions to this project solely in our
// personal capacity and are not conveying any rights to any intellectual
// property of any third parties.

#ifndef CUBBYCITY_CURL_DOWNLOADER_HPP
#define CUBBYCITY_CURL_DOWNLOADER_HPP

#include <CubbyCity/Platform/Downloader.hpp>

namespace CubbyCity
{
class CurlDownloader : public Downloader
{
 public:
    bool DownloadData(std::string& out, const std::string& url) override;

 private:
    static size_t WriteData(void* ptr, size_t size, size_t nmemb, void* stream);
};
}  // namespace CubbyCity

#endif  // CUBBYCITY_CURL_DOWNLOADER_HPP