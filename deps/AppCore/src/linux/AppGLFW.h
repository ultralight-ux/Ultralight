#pragma once
#include <AppCore/App.h>
#include <AppCore/Window.h>
#include "../RefCountedImpl.h"
#include "MonitorGLFW.h"
#include <vector>
#include <memory>

namespace ultralight {

class GPUContextGL;
class GPUDriverGL;
class FileSystemBasic;

class AppGLFW : public App,
                public RefCountedImpl<AppGLFW>,
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

  REF_COUNTED_IMPL(AppGLFW);

protected:
  AppGLFW();
  virtual ~AppGLFW();
  void Update();

  friend class App;
  
  DISALLOW_COPY_AND_ASSIGN(AppGLFW);

  bool is_running_ = false;
  AppListener* listener_ = nullptr;
  RefPtr<Renderer> renderer_;
  RefPtr<Window> window_;
  std::unique_ptr<MonitorGLFW> main_monitor_;
  std::unique_ptr<GPUContextGL> gpu_context_;
  std::unique_ptr<FileSystemBasic> file_system_;
};

}  // namespace ultralight
