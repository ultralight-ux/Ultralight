#pragma once
#include "Defines.h"
#include <Ultralight/RefPtr.h>
#include <Ultralight/Renderer.h>

namespace ultralight {

class Monitor;
class Window;

class AExport AppListener {
public:
  virtual ~AppListener() {}

  virtual void OnUpdate() {}
};

class AExport App : public RefCounted {
public:
  static Ref<App> Create();

  static App* instance();

  virtual void set_window(Ref<Window> window) = 0;

  virtual RefPtr<Window> window() = 0;

  virtual void set_listener(AppListener* listener) = 0;

  virtual AppListener* listener() = 0;

  virtual bool is_running() const = 0;

  virtual Monitor* main_monitor() = 0;

  virtual Ref<Renderer> renderer() = 0;

  virtual void Run() = 0;

  virtual void Quit() = 0;

protected:
  virtual ~App();
};

}  // namespace ultralight