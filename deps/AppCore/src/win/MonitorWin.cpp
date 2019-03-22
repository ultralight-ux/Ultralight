#include "MonitorWin.h"

namespace ultralight {

MonitorWin::MonitorWin(WindowsUtil* util) : util_(util) {
  monitor_ = MonitorFromPoint({ 1,1 }, MONITOR_DEFAULTTONEAREST);
}

double MonitorWin::scale() const {
  return util_->GetMonitorDPI(monitor_);
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
