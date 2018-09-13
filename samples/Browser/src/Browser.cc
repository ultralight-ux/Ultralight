#include "Browser.h"
#include <Ultralight/platform/Platform.h>
#include <Ultralight/platform/Config.h>
#include <Ultralight/Renderer.h>

Browser::Browser(framework::Window& window) : window_(window) {
  bool enable_vsync = true;
  window_.set_listener(this);
  gpu_context_ = framework::GPUContext::Create(window, enable_vsync);

  // Setup our Platform API handlers
  ultralight::Platform& platform = ultralight::Platform::instance();

  ultralight::Config config;
  config.face_winding = gpu_context_->face_winding();
  config.device_scale_hint = window_.scale();
  config.use_distance_field_fonts = window_.scale() != 1.0f; // Only use SDF fonts for high-DPI
  config.use_distance_field_paths = true;

#if defined(FRAMEWORK_PLATFORM_MAC)
  // Use @resource_path on macOS to use the App Bundle's resource directory
  const char* asset_dir = "@resource_path";
#else
  const char* asset_dir = "assets/";
#endif

  platform.set_config(config);
  platform.set_gpu_driver(gpu_context_->driver());
  platform.set_font_loader(framework::CreatePlatformFontLoader());
  platform.set_file_system(framework::CreatePlatformFileSystem(asset_dir));

  // Create the Renderer
  renderer_ = ultralight::Renderer::Create();

  // Create the UI
  ui_.reset(new UI(*renderer_.get(), platform.gpu_driver(), window_));
}

Browser::~Browser() {
  window_.set_listener(nullptr);

  // Destroy the UI
  ui_.reset();

  // Destroy our Platform handlers
  ultralight::Platform& platform = ultralight::Platform::instance();
  delete platform.file_system();
  platform.set_file_system(nullptr);
  delete platform.font_loader();
  platform.set_font_loader(nullptr);

  // Destroy GPUContext and reset associated GPU driver
  gpu_context_.reset();
  platform.set_gpu_driver(nullptr);
}

void Browser::Update() {
  renderer_->Update();
  auto& driver = *gpu_context_->driver();
    driver.BeginSynchronize();
  renderer_->Render();

  
  if (driver.HasCommandsPending()) {
    gpu_context_->BeginDrawing();
    driver.DrawCommandList();
    ui_->Draw();
    gpu_context_->PresentFrame();
    gpu_context_->EndDrawing();
  }
}

void Browser::OnKeyEvent(const ultralight::KeyEvent& evt) {
  ui_->FireKeyEvent(evt);
}

void Browser::OnMouseEvent(const ultralight::MouseEvent& evt) {
  ui_->FireMouseEvent(evt);
}

void Browser::OnScrollEvent(const ultralight::ScrollEvent& evt) {
  ui_->FireScrollEvent(evt);
}

void Browser::OnResize(int width, int height) {
  gpu_context_->Resize(width, height);
  ui_->Resize(width, height);
}

namespace framework {

std::unique_ptr<Window> Application::InitWindow(PlatformWindowConfig& config) {
#ifdef FRAMEWORK_PLATFORM_GLFW
  config.width = 1024;
  config.height = 768;
  config.title = "Ultralight - Browser Sample - GLFW";
#elif FRAMEWORK_PLATFORM_WIN
  config.width = 1024;
  config.height = 768;
  config.fullscreen = false;
#endif
  return Window::Create(config);
}

std::unique_ptr<Application> Application::Create(Window& window) {
  return std::unique_ptr<Application>(new Browser(window));
}

}  // namespace framework
