#pragma once
#include <Ultralight/platform/FontLoader.h>
#include <Ultralight/platform/FileSystem.h>

namespace framework {
  ultralight::FontLoader* CreatePlatformFontLoader();
  ultralight::FileSystem* CreatePlatformFileSystem(const char* baseDir);
}

#ifdef FRAMEWORK_PLATFORM_WIN
#include "platform/win/PlatformWin.h"

namespace framework {
typedef PlatformWindowConfigWin PlatformWindowConfig;
typedef HWND PlatformWindowHandle;
}

#ifdef FRAMEWORK_DRIVER_D3D11
#include "platform/win/d3d11/PlatformD3D11.h"

namespace framework {
  typedef PlatformGPUContextD3D11 PlatformGPUContext;
}
#endif

#elif FRAMEWORK_PLATFORM_MAC
namespace framework {
class PlatformGPUContext;
struct PlatformWindowConfig;
struct PlatformWindowHandle;
}
#elif FRAMEWORK_PLATFORM_GLFW
struct GLFWwindow;
namespace framework {
class PlatformGPUContext;
struct PlatformWindowConfig {
  int width;
  int height;
  const char* title;
};
typedef GLFWwindow* PlatformWindowHandle;
}

#else
static_assert(false, "Undefined Framework Platform.");
#endif
