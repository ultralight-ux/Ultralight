#include <AppCore/App.h>
#include <AppCore/Window.h>
#include <AppCore/Overlay.h>
#ifdef _WIN32
#include <Windows.h>
#endif

using namespace ultralight;

#define WIDTH 300
#define HEIGHT 300

RefPtr<Overlay> overlay;

class Listener : public WindowListener {
public:
	Listener() {}
	virtual ~Listener() {}
	virtual void OnClose() {}
	virtual void OnResize(int width, int height) { overlay->Resize(width, height); }
	virtual void OnChangeScale(double scale) {}
};

#ifdef _WIN32
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
#else
int main() {
#endif
  auto app = App::Create();
  app->set_window(Window::Create(app->main_monitor(), WIDTH, HEIGHT, false, 0));
  overlay = Overlay::Create(WIDTH, HEIGHT, 0, 0);

  app->window()->set_listener(new Listener());

  const wchar_t* txt = L"Chinese: 中國很棒.<br/>Korean: 한국은 시원합니다.<br/>Japanese: 日本はナンバーワン.";
  String16 str(txt, wcslen(txt));
  overlay->view()->LoadHTML(str);
  //overlay->view()->LoadHTML("Hello World!");

  app->Run();

  return 0;
}