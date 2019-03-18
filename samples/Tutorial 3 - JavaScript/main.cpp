#include <AppCore/App.h>
#include <AppCore/Window.h>
#include <AppCore/Overlay.h>
#include <AppCore/JSHelpers.h>

using namespace ultralight;

#define WIDTH 300
#define HEIGHT 300

class MyApp : public WindowListener,
              public LoadListener {
  Ref<Overlay> overlay_;
public:
  MyApp(Ref<Window> window) 
    : overlay_(Overlay::Create(window, window->width(), window->height(), 0, 0)) {
    window->set_listener(this);
    overlay_->view()->set_load_listener(this);
    overlay_->view()->LoadHTML(R"(
      <div style="background-color: yellow; padding: 0.5em;" onclick="GetMessage();">Get the Secret Message!</div>
      <div id="message"></div>
    )");
  }

	virtual ~MyApp() {}

  void GetMessage(const JSObject& thisObject, const JSArgs& args) {
    JSEval("document.getElementById('message').innerHTML='Ultralight rocks!';");
  }

  // Inherited from WindowListener
	virtual void OnClose() {}
	virtual void OnResize(int width, int height) { overlay_->Resize(width, height); }

  // Inherited from LoadListener
  virtual void OnDOMReady(ultralight::View* caller) {
    SetJSContext(caller->js_context());
    JSObject global = JSGlobalObject();
    global["GetMessage"] = BindJSCallback(&MyApp::GetMessage);
  }
};

int main() {
  auto app = App::Create();
    
  auto window = Window::Create(app->main_monitor(), WIDTH, HEIGHT, false, 0);
  window->SetTitle("Tutorial 3");
  app->set_window(window);
    
  MyApp my_app(window);

  app->Run();

  return 0;
}
