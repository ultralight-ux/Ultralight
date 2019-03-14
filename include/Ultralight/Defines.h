///
/// @file Defines.h
///
/// @brief Common platform definitions
///
/// @author
///
/// This file is a part of Ultralight, a fast, lightweight, HTML UI engine
///
/// Website: <http://ultralig.ht>
///
/// Copyright (C) 2019 Ultralight, Inc. All rights reserved.
///
#pragma once

// Needed for limit defines, like INTMAX_MAX, which is used by the std C++ library
#ifndef __STDC_LIMIT_MACROS
#define __STDC_LIMIT_MACROS
#endif

#include <cstdint>
#include <cstddef>
#include <limits.h>

#ifdef SWIG
#define UExport
#else

// Require C++11 Support
#if defined(_MSC_VER)
#   if _MSC_VER < 1800 
#       error This project needs at least Visual Studio 2013 to build
#   endif
#elif __cplusplus <= 199711L
#   error This project can only be compiled with a compiler that supports C++11
#endif


#if defined(__WIN32__) || defined(_WIN32)
#  if defined(ULTRALIGHT_IMPLEMENTATION)
#    define UExport __declspec(dllexport)
#  else
#    define UExport __declspec(dllimport)
#  endif
#define _thread_local __declspec(thread)
#ifndef _NATIVE_WCHAR_T_DEFINED
#define DISABLE_NATIVE_WCHAR_T
#endif
#else
#  define UExport __attribute__((visibility("default")))
#define _thread_local __thread
#endif

#endif

#define ULTRALIGHT_VERSION "1.0.0"

///
/// @mainpage Ultralight C++ API Reference
///
/// @section intro_sec Introduction
///
/// Hi there, welcome to the C++ API Reference for Ultralight!
///
/// Ultralight is a fast, lightweight HTML UI engine for desktop apps.
///
/// If this is your first time exploring the API, we recommend
/// starting with ultralight::Renderer and ultralight::View.
///
///
/// @section usefullinks_sec Useful Links
/// - Home:   <https://ultralig.ht> -- Get the latest binaries
/// - Docs:   <https://docs.ultralig.ht> -- API overview, code snippets, tutorials and more!
/// - Slack:  <http://chat.ultralig.ht/> -- Stuck? Have questions? Come chat with us!
/// - GitHub: <https://github.com/ultralight-ux/ultralight> -- Report issues and browse code
///
/// @section copyright_sec Copyright
/// Documentation is copyright (C) 2019 Ultralight, Inc. All rights reserved.
///
