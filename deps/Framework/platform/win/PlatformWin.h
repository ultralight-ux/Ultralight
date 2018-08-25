#pragma once
#include <Windows.h>

namespace framework {

struct PlatformWindowConfigWin {
  HINSTANCE hInstance;
  int width;
  int height;
  bool fullscreen;
  WNDPROC wndProc;
};

}  // namespace framework
