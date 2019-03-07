#include <AppCore/App.h>
#include <AppCore/Window.h>
#include <AppCore/Overlay.h>
#ifdef _WIN32
#include <Windows.h>
#endif

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

#ifdef _WIN32
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
#else
int main() {
#endif
  auto app = App::Create();
  app->set_window(Window::Create(app->main_monitor(), WIDTH, HEIGHT, false, 0));
  auto overlay = Overlay::Create(WIDTH, HEIGHT, 0, 0);

  app->window()->set_listener(new Listener(overlay.ptr()));
  app->window()->SetTitle("Hello!");

  overlay->view()->LoadHTML("Hello World!");

  app->Run();

  return 0;
}
