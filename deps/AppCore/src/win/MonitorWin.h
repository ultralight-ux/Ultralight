#pragma once
#include <AppCore/Monitor.h>
#include <Windows.h>
#include "WindowsUtil.h"

namespace ultralight {
class MonitorWin : public Monitor {
public:
  MonitorWin(WindowsUtil* util);
  virtual ~MonitorWin() {}

  virtual double scale() const override;

  virtual uint32_t width() const override;

  virtual uint32_t height() const override;

protected:
  WindowsUtil* util_;
  HMONITOR monitor_;
};

}  // namespace ultralight
