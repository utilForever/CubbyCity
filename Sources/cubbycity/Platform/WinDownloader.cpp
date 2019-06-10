// Copyright (c) 2019 Chris Ohk, Paul Kweon, Den So, Edward Sung

// We are making my contributions/submissions to this project solely in our
// personal capacity and are not conveying any rights to any intellectual
// property of any third parties.

#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING

#include <CubbyCity/Platform/WinDownloader.hpp>

#include <Windows.h>
#include <winhttp.h>

#include <codecvt>
#include <locale>
#include <vector>

#pragma comment(lib, "Winhttp.lib")

namespace CubbyCity
{
bool WinDownloader::DownloadData(std::string& out, const std::string& url)
{
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::vector<char> stream;

    URL_COMPONENTS uc;
    wchar_t scheme[256];
    wchar_t hostName[256];
    wchar_t userName[256];
    wchar_t password[256];
    wchar_t urlPath[1024];
    wchar_t extraInfo[256];

    uc.dwStructSize = sizeof(uc);
    uc.lpszScheme = scheme;
    uc.lpszHostName = hostName;
    uc.lpszUserName = userName;
    uc.lpszPassword = password;
    uc.lpszUrlPath = urlPath;
    uc.lpszExtraInfo = extraInfo;

    uc.dwSchemeLength = _countof(scheme);
    uc.dwHostNameLength = _countof(hostName);
    uc.dwUserNameLength = _countof(userName);
    uc.dwPasswordLength = _countof(password);
    uc.dwUrlPathLength = _countof(urlPath);
    uc.dwExtraInfoLength = _countof(extraInfo);

    const std::wstring wURL = converter.from_bytes(url);
    // WCHAR* wURL = new WCHAR[url.length() + 1];
    // MultiByteToWideChar(CP_UTF8, 0, url.c_str(), url.length(), wURL,
    //                    url.length() + 1);
    if (!WinHttpCrackUrl(wURL.c_str(), static_cast<DWORD>(wURL.size()),
                         ICU_ESCAPE, &uc))
    {
        return false;
    }

    DWORD flags = 0;
    switch (uc.nScheme)
    {
        case INTERNET_SCHEME_FTP:
            break;
        case INTERNET_SCHEME_HTTP:
            break;
        case INTERNET_SCHEME_HTTPS:
            flags |= WINHTTP_FLAG_SECURE;
            break;
        default:
            return false;
    }

    HINTERNET session =
        WinHttpOpen(L"CubbyCity", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
                    WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    if (session == nullptr)
    {
        return false;
    }

    HINTERNET hConnect = WinHttpConnect(session, uc.lpszHostName, uc.nPort, 0);
    if (hConnect == nullptr)
    {
        WinHttpCloseHandle(session);
        return false;
    }

    std::wstring objectName = uc.lpszUrlPath;
    std::wstring strExtraInfo = uc.lpszExtraInfo;
    if (!strExtraInfo.empty())
    {
        objectName = objectName + strExtraInfo;
    }

    HINTERNET hRequest = WinHttpOpenRequest(
        hConnect, L"GET", objectName.c_str(), nullptr, WINHTTP_NO_REFERER,
        WINHTTP_DEFAULT_ACCEPT_TYPES, flags);
    if (hRequest == nullptr)
    {
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(session);
        return false;
    }

    bool noError = WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS,
                                      0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0) != 0;
    noError = noError && (WinHttpReceiveResponse(hRequest, nullptr) != 0);
    wchar_t encoding[128] = { 0 };
    DWORD encodingLength = sizeof(encoding);
    if (WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_CONTENT_ENCODING, nullptr,
                            encoding, &encodingLength, nullptr))
    {
        encodingLength = encodingLength / 2;
        encoding[encodingLength] = 0;
    }

    if (noError)
    {
        DWORD dwSize;
        char buffer[1024];

        do
        {
            dwSize = sizeof(buffer);

            if (WinHttpReadData(hRequest, static_cast<LPVOID>(buffer), dwSize,
                                &dwSize))
            {
                size_t oldSize = stream.size();
                stream.resize(oldSize + dwSize);
                memcpy(stream.data() + oldSize, buffer, dwSize);
            }
            else
            {
                noError = false;
            }
        } while (noError && dwSize > 0);
    }

    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(session);

    out = std::string(stream.begin(), stream.end());

    return noError;
}
}  // namespace CubbyCity