#include <AppCore/App.h>
#include <AppCore/Window.h>
#include <AppCore/Overlay.h>
#include <AppCore/JSHelpers.h>

using namespace ultralight;

class MyApp : public LoadListener {
  RefPtr<Overlay> overlay_;
public:
  MyApp(Ref<Window> win) {
    ///
    /// Create our Overlay, use the same dimensions as our Window.
    ///
    overlay_ = Overlay::Create(win, win->width(), win->height(), 0, 0);

    ///
    /// Register our MyApp instance as a load listener so we can handle the
    /// page's DOMReady event below.
    ///
    overlay_->view()->set_load_listener(this);

    ///
    /// Load a string of HTML (we're using a C++11 Raw String Literal)
    ///
    overlay_->view()->LoadHTML(R"(
      <html>
        <head>
          <style type="text/css">
            body { font-family: Arial; text-align: center; }
          </style>
        </head>
        <body>
          <button onclick="GetMessage();">Get the Secret Message!</button>
          <div id="message"></div>
        </body>
      </html>
    )");
  }

  virtual ~MyApp() {}

  ///
  /// Our native JavaScript callback. This function will be called from
  /// JavaScript by calling GetMessage(). We bind the callback within
  /// the DOMReady callback defined below.
  ///
  void GetMessage(const JSObject& thisObject, const JSArgs& args) {
    ///
    /// We display the secret message by executing a string of JavaScript.
    ///
    JSEval("document.getElementById('message').innerHTML='Ultralight rocks!';");
  }

  ///
  /// Inherited from LoadListener, called when the page has finished parsing
  /// the document and is ready to execute scripts.
  ///
  /// We perform all our JavaScript initialization here.
  ///
  virtual void OnDOMReady(ultralight::View* caller) {
    ///
    /// Set our View's JSContext as the one to use in subsequent JSHelper calls
    ///
    SetJSContext(caller->js_context());

    ///
    /// Get the global object (this would be the "window" object in JS)
    ///
    JSObject global = JSGlobalObject();

    ///
    /// Bind MyApp::GetMessage to the JavaScript function named "GetMessage"
    ///
    global["GetMessage"] = BindJSCallback(&MyApp::GetMessage);
  }
};

int main() {
  ///
  /// Create our main App instance.
  ///
  auto app = App::Create();
    
  ///
  /// Create our Window using default window flags.
  ///
  auto window = Window::Create(app->main_monitor(), 300, 300, false, kWindowFlags_Titled);

  ///
  /// Set our window title.
  ///
  window->SetTitle("Tutorial 4 - JavaScript");

  ///
  /// Bind our App's main window.
  ///
  /// @note This MUST be done before creating any overlays or calling App::Run
  ///
  app->set_window(window);
    
  ///
  /// Create our MyApp instance (creates overlays and handles all logic).
  ///
  MyApp my_app(window);

  ///
  /// Run our main loop.
  ///
  app->Run();

  return 0;
}
