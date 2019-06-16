// Copyright (c) 2019 Chris Ohk, Paul Kweon, Den So, Edward Sung

// We are making my contributions/submissions to this project solely in our
// personal capacity and are not conveying any rights to any intellectual
// property of any third parties.

#include <CubbyCity/Platform/CurlDownloader.hpp>

#include <curl/curl.h>

#include <sstream>

namespace CubbyCity
{
bool CurlDownloader::DownloadData(std::string& out, const std::string& url)
{
    std::stringstream stream;

    static bool curlInitialized = false;
    static CURL* curlHandle = nullptr;

    if (!curlInitialized)
    {
        curl_global_init(CURL_GLOBAL_DEFAULT);
        curlHandle = curl_easy_init();
        curlInitialized = true;

        // Set up curl to perform fetch
        curl_easy_setopt(curlHandle, CURLOPT_WRITEFUNCTION, WriteData);
        curl_easy_setopt(curlHandle, CURLOPT_HEADER, 0L);
        curl_easy_setopt(curlHandle, CURLOPT_VERBOSE, 0L);
        curl_easy_setopt(curlHandle, CURLOPT_ACCEPT_ENCODING, "gzip");
        curl_easy_setopt(curlHandle, CURLOPT_NOSIGNAL, 1L);
        curl_easy_setopt(curlHandle, CURLOPT_DNS_CACHE_TIMEOUT, -1);
    }

    curl_easy_setopt(curlHandle, CURLOPT_WRITEDATA, &stream);
    curl_easy_setopt(curlHandle, CURLOPT_URL, url.c_str());

    CURLcode result = curl_easy_perform(curlHandle);

    out = stream.str();
    return result == CURLE_OK && stream.rdbuf()->in_avail();
}

size_t CurlDownloader::WriteData(void* ptr, size_t size, const size_t nmemb,
                                 void* stream)
{
    static_cast<std::stringstream*>(stream)->write(reinterpret_cast<char*>(ptr),
                                                   size * nmemb);
    return size * nmemb;
}
}  // namespace CubbyCity