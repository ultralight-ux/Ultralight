// Copyright 2018 Ultralight, Inc. All rights reserved.
#pragma once

// Needed for limit defines, like INTMAX_MAX, which is used by the std C++ library
#ifndef __STDC_LIMIT_MACROS
#define __STDC_LIMIT_MACROS
#endif

#include <cstdint>
#include <cstddef>
#include <limits.h>

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

#define ULTRALIGHT_VERSION "0.9.1"
