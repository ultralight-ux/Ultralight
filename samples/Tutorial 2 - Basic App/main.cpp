#include <AppCore/App.h>
#include <AppCore/Window.h>
#include <AppCore/Overlay.h>

using namespace ultralight;

#define WIDTH 300
#define HEIGHT 300

class Listener : public WindowListener {
    Overlay* overlay_;
public:
    Listener(Overlay* overlay) : overlay_(overlay) {}
	virtual ~Listener() {}
	virtual void OnClose() {}
	virtual void OnResize(int width, int height) { overlay_->Resize(width, height); }
	virtual void OnChangeScale(double scale) {}
};

int main() {
  auto app = App::Create();
    
  auto window = Window::Create(app->main_monitor(), WIDTH, HEIGHT, false, 0);
  window->SetTitle("Hello!");
    
  auto overlay = Overlay::Create(window, WIDTH, HEIGHT, 0, 0);
  window->set_listener(new Listener(overlay.ptr()));
  overlay->view()->LoadHTML("Hello World!");

  app->set_window(window);
  app->Run();

  return 0;
}
