#include "MainLoopWin.h"
#include "Windowsx.h"

namespace framework {

MainLoopWin::MainLoopWin(PlatformWindowConfig& config) : is_running_(false) {
  window_ = Application::InitWindow(config);
  if (window_)
    application_ = Application::Create(*window_.get());
}

void MainLoopWin::Run() {
  if (!application_)
    return;

  MSG msg = { 0 };
  is_running_ = true;
  while (WM_QUIT != msg.message && is_running_) {
    if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
    else {
      application_->Update();

      // Sleep a tiny bit to reduce CPU usage
      Sleep(1);
    }
  }
}

}  // namespace framework
