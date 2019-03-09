#include "GPUContextGL.h"
#include "GPUDriverGL.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace ultralight {

GPUContextGL::GPUContextGL(GLFWwindow* window, float scale, bool enable_vsync) : window_(window) {
  glfwMakeContextCurrent(window_);
  gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
  glfwSwapInterval(enable_vsync ? 1 : 0);
  driver_.reset(new ultralight::GPUDriverGL(scale));
}

void GPUContextGL::PresentFrame() {
  glfwSwapBuffers(window_);
}

void GPUContextGL::Resize(int width, int height) {
  //ultralight::GPUDriverGL* driver_gl = static_cast<ultralight::GPUDriverGL*>(driver_.get());
  //driver_gl->ResizeViewport(width, height);
}

}  // namespace ultralight
