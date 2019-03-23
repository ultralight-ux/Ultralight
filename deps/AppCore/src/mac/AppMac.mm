#include "AppMac.h"
#include <Ultralight/platform/Platform.h>
#include <Ultralight/platform/Config.h>
#import <Cocoa/Cocoa.h>
#import "AppDelegate.h"
#import "WindowMac.h"
#import "metal/GPUContextMetal.h"
#import "metal/GPUDriverMetal.h"
#include "FileSystemMac.h"

namespace ultralight {

AppMac::AppMac() {
  [NSApplication sharedApplication];
    
  AppDelegate *appDelegate = [[AppDelegate alloc] init];
  [NSApp setDelegate:appDelegate];

  Config config;
  config.device_scale_hint = main_monitor_.scale();
  config.face_winding = kFaceWinding_Clockwise;
  Platform::instance().set_config(config);

  file_system_.reset(new FileSystemMac("@resource_path"));
  Platform::instance().set_file_system(file_system_.get());

  renderer_ = Renderer::Create();
}

AppMac::~AppMac() {
}

void AppMac::OnClose() {
}

void AppMac::OnResize(uint32_t width, uint32_t height) {
  if (gpu_context_)
    gpu_context_->Resize((int)width, (int)height);
}

void AppMac::set_window(Ref<Window> window) {
  window_ = window;
    
  WindowMac* win = static_cast<WindowMac*>(window_.get());
  gpu_context_.reset(new GPUContextMetal(win->view(), win->width(), win->height(), win->scale(), win->is_fullscreen(), true));
  Platform::instance().set_gpu_driver(gpu_context_->driver());
  win->set_app_listener(this);
}

Monitor* AppMac::main_monitor() {
  return &main_monitor_;
}

Ref<Renderer> AppMac::renderer() {
  return *renderer_.get();
}

void AppMac::Run() {
  if (!window_) {
    NSAlert *alert = [[NSAlert alloc] init];
    [alert setMessageText:@"Forgot to call App::set_window before App::Run"];
    [alert runModal];
    exit(-1);
  }

  if (is_running_)
    return;
    
  [NSApp run];
}

void AppMac::Quit() {
  [NSApp terminate:nil];
}

void AppMac::Update() {
  if (listener_)
    listener_->OnUpdate();

  renderer()->Update();

  gpu_context_->driver()->BeginSynchronize();
  renderer_->Render();
  gpu_context_->driver()->EndSynchronize();

  if (gpu_context_->driver()->HasCommandsPending()) {
    gpu_context_->BeginDrawing();
    gpu_context_->driver()->DrawCommandList();
    if (window_)
	    static_cast<WindowMac*>(window_.get())->Draw();
    gpu_context_->PresentFrame();
    gpu_context_->EndDrawing();
  }
}

static App* g_app_instance = nullptr;

Ref<App> App::Create() {
  g_app_instance = (App*)new AppMac();
  return AdoptRef(*g_app_instance);
}

App::~App() {
  g_app_instance = nullptr;
}

App* App::instance() {
  return g_app_instance;
}

}  // namespace ultralight
