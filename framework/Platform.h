#pragma once
#include <Ultralight/platform/FontLoader.h>
#include <Ultralight/platform/FileSystem.h>

namespace framework {
  ultralight::FontLoader* CreatePlatformFontLoader();
  ultralight::FileSystem* CreatePlatformFileSystem(const char* baseDir);
}

#ifdef FRAMEWORK_PLATFORM_WIN
#include "../win/PlatformWin.h"

namespace framework {
typedef PlatformGPUContextD3D11 PlatformGPUContext;
typedef PlatformWindowConfigWin PlatformWindowConfig;
typedef HWND PlatformWindowHandle;
}

#elif FRAMEWORK_PLATFORM_MAC
namespace framework {
class PlatformGPUContext;
struct PlatformWindowConfig;
struct PlatformWindowHandle;
}

#elif FRAMEWORK_PLATFORM_LINUX

#else
static_assert(false, "Undefined Framework Platform.");
#endif
