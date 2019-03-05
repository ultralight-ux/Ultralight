#pragma once
#include <AppCore/App.h>
#include <AppCore/Window.h>
#include "../RefCountedImpl.h"
#include "MonitorWin.h"
#include "FontLoaderWin.h"
#include "FileSystemWin.h"
#include <vector>
#include <memory>

namespace ultralight {

class GPUContextD3D11;
class GPUDriverD3D11;

class AppWin : public App,
               public RefCountedImpl<AppWin>,
               public WindowListener {
public:
  // Inherited from WindowListener

  virtual void OnClose() override;

  virtual void OnResize(int width, int height) override;

  // Inherited from App

  virtual void set_listener(AppListener* listener) override { listener_ = listener; }

  virtual void set_window(Ref<Window> window) override;

  virtual RefPtr<Window> window() override { return window_; }

  virtual AppListener* listener() override { return listener_; }

  virtual bool is_running() const override { return is_running_; }

  virtual Monitor* main_monitor() override;

  virtual Ref<Renderer> renderer() override;

  virtual void Run() override;

  virtual void Quit() override;

  REF_COUNTED_IMPL(AppWin);

protected:
  AppWin();
  virtual ~AppWin();
  void Update();

  friend class App;
  
  DISALLOW_COPY_AND_ASSIGN(AppWin);

  bool is_running_ = false;
  AppListener* listener_ = nullptr;
  RefPtr<Renderer> renderer_;
  RefPtr<Window> window_;
  MonitorWin main_monitor_;
  std::unique_ptr<FontLoaderWin> font_loader_;
  std::unique_ptr<FileSystemWin> file_system_;
  std::unique_ptr<GPUContextD3D11> gpu_context_;
  std::unique_ptr<GPUDriverD3D11> gpu_driver_;
};

}  // namespace ultralight
