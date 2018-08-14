#pragma once
#include <memory>
#include "Platform.h"
#include "Window.h"
#include <Ultralight/KeyEvent.h>
#include <Ultralight/MouseEvent.h>
#include <Ultralight/ScrollEvent.h>

namespace framework {

class Application {
public:
  virtual ~Application() {}

  static std::unique_ptr<Window> InitWindow(PlatformWindowConfig& config);

  static std::unique_ptr<Application> Create(Window& window);

  virtual void Update() = 0;
};

}  // namespace framework
