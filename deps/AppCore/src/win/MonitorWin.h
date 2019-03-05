#pragma once
#include <AppCore/Monitor.h>
#include <Windows.h>

namespace ultralight {
class MonitorWin : public Monitor {
public:
  MonitorWin();
  virtual ~MonitorWin() {}

  virtual double scale() const override;

  virtual int width() const override;

  virtual int height() const override;

protected:
  HMONITOR monitor_;
};

}  // namespace ultralight
