#pragma once
#include <memory>
#include <Framework/Platform.h>
#include <Framework/Window.h>
#include <Framework/Application.h>

namespace framework {

class MainLoopWin {
 public:
  MainLoopWin(PlatformWindowConfig& config);

  void Run();

 protected:
  bool is_running_;
  std::unique_ptr<Window> window_;
  std::unique_ptr<Application> application_;
};

}  // namespace framework