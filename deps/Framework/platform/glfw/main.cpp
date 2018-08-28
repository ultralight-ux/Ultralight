#include <Framework/Window.h>
#include <Framework/Application.h>
#include <Framework/platform/glfw/PlatformGLFW.h>

int main() {
  framework::PlatformWindowConfig config;
  config.width = 800;
  config.height = 600;

  auto window = framework::Application::InitWindow(config);
  if (!window)
    return -1;

  auto app = framework::Application::Create(*window.get());

  while (!glfwWindowShouldClose(window->handle())) {
    app->Update();
    glfwPollEvents();
  }

  app.reset();
  window.reset();

  glfwTerminate();

  return 0;
}