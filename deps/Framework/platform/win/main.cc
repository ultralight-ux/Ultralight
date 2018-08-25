#include <Windows.h>
#include "MainLoopWin.h"

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
  UNREFERENCED_PARAMETER(hPrevInstance);
  UNREFERENCED_PARAMETER(lpCmdLine);

  // Initialize with defaults, application may override these before creating Window.
  framework::PlatformWindowConfig config;
  config.width = 800;
  config.height = 600;
  config.fullscreen = false;
  config.hInstance = hInstance;

  framework::MainLoopWin main_loop(config);
  main_loop.Run();

  return 0;
}