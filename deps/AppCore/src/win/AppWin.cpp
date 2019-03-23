#include "AppWin.h"
#include <Windows.h>
#include "WindowWin.h"
#include "d3d11/GPUContextD3D11.h"
#include "d3d11/GPUDriverD3D11.h"
#include <Ultralight/platform/Platform.h>
#include <Ultralight/platform/Config.h>
#include <Shlwapi.h>
#include "WindowsUtil.h"
#include "MonitorWin.h"

namespace ultralight {

AppWin::AppWin() {
  windows_util_.reset(new WindowsUtil());
  windows_util_->EnableDPIAwareness();

  main_monitor_.reset(new MonitorWin(windows_util_.get()));

  Config config;
  config.device_scale_hint = main_monitor_->scale();
  config.face_winding = kFaceWinding_Clockwise;
  Platform::instance().set_config(config);

  font_loader_.reset(new FontLoaderWin());
  Platform::instance().set_font_loader(font_loader_.get());

  HMODULE hModule = GetModuleHandleW(NULL);
  WCHAR path[MAX_PATH];
  GetModuleFileNameW(hModule, path, MAX_PATH);
  PathRemoveFileSpecW(path);

  PathAppendW(path, L"assets");

  file_system_.reset(new FileSystemWin(path));
  Platform::instance().set_file_system(file_system_.get());

  renderer_ = Renderer::Create();
}

AppWin::~AppWin() {
}

void AppWin::OnClose() {
}

void AppWin::OnResize(uint32_t width, uint32_t height) {
  if (gpu_context_)
    gpu_context_->Resize((int)width, (int)height);
}

void AppWin::set_window(Ref<Window> window) {
  window_ = window;
  gpu_context_.reset(new GPUContextD3D11());
  WindowWin* win = static_cast<WindowWin*>(window_.get());
  if (!gpu_context_->Initialize(win->hwnd(), win->width(),
    win->height(), win->scale(), win->is_fullscreen(), true, false, 1)) {
    MessageBoxW(NULL, (LPCWSTR)L"Failed to initialize D3D11 context", (LPCWSTR)L"Notification", MB_OK);
    exit(-1);
  }

  gpu_driver_.reset(new GPUDriverD3D11(gpu_context_.get()));
  Platform::instance().set_gpu_driver(gpu_driver_.get());
  win->set_app_listener(this);
}

Monitor* AppWin::main_monitor() {
  return main_monitor_.get();
}

Ref<Renderer> AppWin::renderer() {
  return *renderer_.get();
}

void AppWin::Run() {
  if (!window_) {
    MessageBoxW(NULL, (LPCWSTR)L"Forgot to call App::set_window before App::Run", (LPCWSTR)L"Notification", MB_OK);
    exit(-1);
  }

  if (is_running_)
    return;

  MSG msg = { 0 };
  is_running_ = true;
  while (WM_QUIT != msg.message && is_running_) {
    if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
    else {
      Update();

      // Sleep a tiny bit to reduce CPU usage
      Sleep(1);
    }
  }
}

void AppWin::Quit() {
  is_running_ = false;
}

void AppWin::Update() {
  if (listener_)
    listener_->OnUpdate();

  renderer()->Update();

  gpu_driver_->BeginSynchronize();
  renderer_->Render();
  gpu_driver_->EndSynchronize();

  if (gpu_driver_->HasCommandsPending()) {
    gpu_context_->BeginDrawing();
    gpu_driver_->DrawCommandList();
    if (window_)
	    static_cast<WindowWin*>(window_.get())->Draw();
    gpu_context_->PresentFrame();
    gpu_context_->EndDrawing();
  }
}

static App* g_app_instance = nullptr;

Ref<App> App::Create() {
  g_app_instance = new AppWin();
  return AdoptRef(*g_app_instance);
}

App::~App() {
  g_app_instance = nullptr;
}

App* App::instance() {
  return g_app_instance;
}

}  // namespace ultralight
