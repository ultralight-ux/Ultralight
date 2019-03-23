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

uint32_t MonitorGLFW::width() const {
  const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
  return (uint32_t)mode->width;
}

uint32_t MonitorGLFW::height() const {
  const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
  return (uint32_t)mode->height;
}

}  // namespace ultralight
