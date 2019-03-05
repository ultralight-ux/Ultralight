#include <Framework/Window.h>
#include <Framework/Application.h>
#include <Framework/platform/glfw/PlatformGLFW.h>
#include <chrono>
#include <thread>

int main() {
  framework::PlatformWindowConfig config;
  config.width = 800;
  config.height = 600;
  config.title = "SET THIS TITLE IN Application::InitWindow()";

  auto window = framework::Application::InitWindow(config);
  if (!window)
    return -1;

  auto app = framework::Application::Create(*window.get());

  while (!glfwWindowShouldClose(window->handle())) {
    glfwPollEvents();
    app->Update();
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }

  app.reset();
  window.reset();

  glfwTerminate();

  return 0;
}