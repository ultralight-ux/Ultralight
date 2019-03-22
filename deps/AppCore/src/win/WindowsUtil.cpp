//========================================================================
// Portions of the below file are under the following license:
//
// Copyright (c) 2002-2006 Marcus Geelnard
// Copyright (c) 2006-2016 Camilla Löwy <elmindreda@glfw.org>
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would
//    be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not
//    be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source
//    distribution.
//
//========================================================================

#include "WindowsUtil.h"

namespace ultralight {

// HACK: Define versionhelpers.h functions manually as MinGW lacks the header
static BOOL IsWindowsVersionOrGreater(WORD major, WORD minor, WORD sp);
#define IsWindowsXPOrGreater()                                 \
    IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WINXP),      \
                              LOBYTE(_WIN32_WINNT_WINXP), 0)
#define IsWindowsVistaOrGreater()                              \
    IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_VISTA),      \
                              LOBYTE(_WIN32_WINNT_VISTA), 0)
#define IsWindows7OrGreater()                                  \
    IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WIN7),       \
                              LOBYTE(_WIN32_WINNT_WIN7), 0)
#define IsWindows8OrGreater()                                  \
    IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WIN8),       \
                              LOBYTE(_WIN32_WINNT_WIN8), 0)
#define IsWindows8Point1OrGreater()                            \
    IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WINBLUE),    \
                              LOBYTE(_WIN32_WINNT_WINBLUE), 0)

// HACK: Define versionhelpers.h functions manually as MinGW lacks the header
BOOL IsWindowsVersionOrGreater(WORD major, WORD minor, WORD sp)
{
  OSVERSIONINFOEXW osvi = { sizeof(osvi), major, minor, 0, 0,{ 0 }, sp };
  DWORD mask = VER_MAJORVERSION | VER_MINORVERSION | VER_SERVICEPACKMAJOR;
  ULONGLONG cond = VerSetConditionMask(0, VER_MAJORVERSION, VER_GREATER_EQUAL);
  cond = VerSetConditionMask(cond, VER_MINORVERSION, VER_GREATER_EQUAL);
  cond = VerSetConditionMask(cond, VER_SERVICEPACKMAJOR, VER_GREATER_EQUAL);
  return VerifyVersionInfoW(&osvi, mask, cond);
}

WindowsUtil::WindowsUtil() {
  shcore_lib_ = LoadLibraryA("shcore.dll");
  if (shcore_lib_) {
    setProcessDpiAwareness_ = (FN_SetProcessDpiAwareness)
      GetProcAddress(shcore_lib_, "SetProcessDpiAwareness");
    getDpiForMonitor_ = (FN_GetDpiForMonitor)
      GetProcAddress(shcore_lib_, "GetDpiForMonitor");
  }
}

WindowsUtil::~WindowsUtil() {
  if (shcore_lib_)
    FreeLibrary(shcore_lib_);
}

void WindowsUtil::EnableDPIAwareness() {
  if (IsWindows8Point1OrGreater())
    setProcessDpiAwareness_(PROCESS_PER_MONITOR_DPI_AWARE);
  else if (IsWindowsVistaOrGreater())
    SetProcessDPIAware();
}

double WindowsUtil::GetMonitorDPI(HMONITOR monitor) {
  UINT xdpi, ydpi;

  if (IsWindows8Point1OrGreater()) {
    getDpiForMonitor_(monitor, MDT_EFFECTIVE_DPI, &xdpi, &ydpi);
  } else {
    const HDC dc = GetDC(NULL);
    xdpi = GetDeviceCaps(dc, LOGPIXELSX);
    ydpi = GetDeviceCaps(dc, LOGPIXELSY);
    ReleaseDC(NULL, dc);
  }

  // We only care about DPI in x-dimension right now
  return xdpi / 96.f;
}

}  // namespace ultralight
