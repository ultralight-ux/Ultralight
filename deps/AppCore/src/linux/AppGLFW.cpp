#include "AppGLFW.h"
#include "gl/GPUContextGL.h"
#include "gl/GPUDriverGL.h"
#include <Ultralight/platform/Platform.h>
#include <Ultralight/platform/Config.h>
#include <GLFW/glfw3.h>
#include "WindowGLFW.h"
#include "FileSystemBasic.h"
#include <thread>
#include <chrono>

extern "C" {

static void GLFW_error_callback(int error, const char* description)
{
  fprintf(stderr, "GLFW Error: %s\n", description);
}

}

namespace ultralight {

AppGLFW::AppGLFW() {
  glfwSetErrorCallback(GLFW_error_callback);

  if (!glfwInit())
    exit(EXIT_FAILURE);

  main_monitor_.reset(new MonitorGLFW());

  Config config;
  config.device_scale_hint = main_monitor_->scale();
  config.face_winding = kFaceWinding_Clockwise;
  Platform::instance().set_config(config);

  file_system_.reset(new FileSystemBasic("assets/"));
  Platform::instance().set_file_system(file_system_.get());

  renderer_ = Renderer::Create();
}

AppGLFW::~AppGLFW() {
  glfwTerminate();
}

void AppGLFW::OnClose() {
}

void AppGLFW::OnResize(uint32_t width, uint32_t height) {
  if (gpu_context_)
    gpu_context_->Resize((int)width, (int)height);
}

void AppGLFW::set_window(Ref<Window> window) {
  window_ = window;
  
  WindowGLFW* win = static_cast<WindowGLFW*>(window_.get());
  gpu_context_.reset(new GPUContextGL(win->handle(), (float)win->scale(), true));
  Platform::instance().set_gpu_driver(gpu_context_->driver());
  
  win->set_app_listener(this);
}

Monitor* AppGLFW::main_monitor() {
  return main_monitor_.get();
}

Ref<Renderer> AppGLFW::renderer() {
  return *renderer_.get();
}

void AppGLFW::Run() {
  if (!window_) {
    std::printf("Forgot to call App::set_window before App::Run\n");
    exit(-1);
  }

  WindowGLFW* win = static_cast<WindowGLFW*>(window_.get());

  while (!glfwWindowShouldClose(win->handle())) {
    glfwPollEvents();
    Update();
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }

  glfwTerminate();
}

void AppGLFW::Quit() {
  is_running_ = false;
}

void AppGLFW::Update() {
  if (listener_)
    listener_->OnUpdate();

  renderer()->Update();

  GPUDriver* driver = gpu_context_->driver();

  driver->BeginSynchronize();
  renderer_->Render();
  driver->EndSynchronize();

  if (driver->HasCommandsPending()) {
    gpu_context_->BeginDrawing();
    driver->DrawCommandList();
    if (window_)
	    static_cast<WindowGLFW*>(window_.get())->Draw();
    gpu_context_->PresentFrame();
    gpu_context_->EndDrawing();
  }
}

static App* g_app_instance = nullptr;

Ref<App> App::Create() {
  g_app_instance = new AppGLFW();
  return AdoptRef(*g_app_instance);
}

App::~App() {
  g_app_instance = nullptr;
}

App* App::instance() {
  return g_app_instance;
}

}  // namespace ultralight
