// Copyright (c) 2019 Chris Ohk, Paul Kweon, Den So, Edward Sung

// We are making my contributions/submissions to this project solely in our
// personal capacity and are not conveying any rights to any intellectual
// property of any third parties.

#ifndef CUBBYCITY_MACROS_HPP
#define CUBBYCITY_MACROS_HPP

#if defined(_WIN32) || defined(_WIN64)
#define CUBBYCITY_WINDOWS
#elif defined(__APPLE__)
#define CUBBYCITY_APPLE
#ifndef CUBBYCITY_IOS
#define CUBBYCITY_MACOSX
#endif
#elif defined(linux) || defined(__linux__)
#define CUBBYCITY_LINUX
#endif

#endif  // CUBBYCITY_MACROS_HPP