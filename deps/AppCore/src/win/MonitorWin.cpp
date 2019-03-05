#include "MonitorWin.h"
#include <ShellScalingAPI.h>

namespace ultralight {

MonitorWin::MonitorWin() {
  HRESULT hr = SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);

  if (hr != S_OK) {
    MessageBoxW(NULL, (LPCWSTR)L"SetProcessDpiAwareness failed", (LPCWSTR)L"Notification", MB_OK);
  }

  monitor_ = MonitorFromPoint({ 1,1 }, MONITOR_DEFAULTTONEAREST);
}

double MonitorWin::scale() const {
  UINT dpix, dpiy;
  HRESULT hr = GetDpiForMonitor(monitor_, MDT_EFFECTIVE_DPI, &dpix, &dpiy);

  if (hr != S_OK) {
    MessageBoxW(NULL, (LPCWSTR)L"GetDpiForMonitor failed", (LPCWSTR)L"Notification", MB_OK);
    return 1.0;
  }

  return (double)dpix / 96.0;
}

int MonitorWin::width() const {
  MONITORINFO info;
  info.cbSize = sizeof(info);
  if (!GetMonitorInfo(monitor_, &info))
    MessageBoxW(NULL, (LPCWSTR)L"GetMonitorInfo failed", (LPCWSTR)L"Notification", MB_OK);

  return abs(info.rcMonitor.left - info.rcMonitor.right);
}

int MonitorWin::height() const {
  MONITORINFO info;
  info.cbSize = sizeof(info);
  if (!GetMonitorInfo(monitor_, &info))
    MessageBoxW(NULL, (LPCWSTR)L"GetMonitorInfo failed", (LPCWSTR)L"Notification", MB_OK);

  return abs(info.rcMonitor.top - info.rcMonitor.bottom);
}

}  // namespace ultralight
