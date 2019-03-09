#pragma once
#include <AppCore/Monitor.h>

typedef struct GLFWmonitor GLFWmonitor;

namespace ultralight {

class MonitorGLFW : public Monitor {
public:
  MonitorGLFW();
  virtual ~MonitorGLFW() {}

  virtual double scale() const;

  virtual int width() const;

  virtual int height() const;
protected:
  GLFWmonitor* monitor_;
};

}  // namespace ultralight