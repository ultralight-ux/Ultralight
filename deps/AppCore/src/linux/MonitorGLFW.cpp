#include "MonitorGLFW.h"
#include <GLFW/glfw3.h>

namespace ultralight {

MonitorGLFW::MonitorGLFW() {
  monitor_ = glfwGetPrimaryMonitor();
}

double MonitorGLFW::scale() const {
  float xscale, yscale;
  glfwGetMonitorContentScale(monitor_, &xscale, &yscale);
  return (double)xscale;
}

int MonitorGLFW::width() const {
  const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
  return mode->width;
}

int MonitorGLFW::height() const {
  const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
  return mode->height;
}

}  // namespace ultralight
