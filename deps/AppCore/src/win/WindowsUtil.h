#pragma once
#include <Windows.h>

// Enum from <ShellScalingAPI.h> (Shcore.lib)
typedef enum
{
  PROCESS_DPI_UNAWARE = 0,
  PROCESS_SYSTEM_DPI_AWARE = 1,
  PROCESS_PER_MONITOR_DPI_AWARE = 2
} PROCESS_DPI_AWARENESS;

// Enum from <ShellScalingAPI.h> (Shcore.lib)
typedef enum
{
  MDT_EFFECTIVE_DPI = 0,
  MDT_ANGULAR_DPI = 1,
  MDT_RAW_DPI = 2,
  MDT_DEFAULT = MDT_EFFECTIVE_DPI
} MONITOR_DPI_TYPE;

namespace ultralight {
class WindowsUtil {
public:
  WindowsUtil();
  ~WindowsUtil();

  void EnableDPIAwareness();

  double GetMonitorDPI(HMONITOR monitor);

protected:
  typedef HRESULT(WINAPI * FN_SetProcessDpiAwareness)(PROCESS_DPI_AWARENESS);
  typedef HRESULT(WINAPI * FN_GetDpiForMonitor)(HMONITOR, MONITOR_DPI_TYPE, UINT*, UINT*);
  FN_SetProcessDpiAwareness setProcessDpiAwareness_ = nullptr;
  FN_GetDpiForMonitor getDpiForMonitor_ = nullptr;
  HMODULE shcore_lib_ = nullptr;
};

}