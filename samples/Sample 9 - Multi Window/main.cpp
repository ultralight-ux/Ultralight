#include <AppCore/App.h>
#include <AppCore/Window.h>
#include <AppCore/Overlay.h>
#include <AppCore/JSHelpers.h>
#include <memory>

using namespace ultralight;

const char* htmlString();

///
/// Welcome to Sample 9!
///
/// In this sample we'll demonstrate how to spawn multiple windows using the AppCore API.
///
/// 

class HTMLWindow : public WindowListener,
                   public ViewListener {
  RefPtr<Window> window_;
  RefPtr<Overlay> overlay_;
public:
  HTMLWindow(const char* title, const char* url, int x, int y, int width, int height) {
    window_ = Window::Create(App::instance()->main_monitor(), width, height, false,
      kWindowFlags_Titled | kWindowFlags_Resizable | kWindowFlags_Hidden);
    window_->MoveTo(x, y);
    window_->SetTitle(title);
    window_->Show();
    window_->set_listener(this);

    overlay_ = Overlay::Create(window_, window_->width(), window_->height(), 0, 0);
    overlay_->view()->LoadURL(url);
    overlay_->view()->set_view_listener(this);
  }

  inline RefPtr<View> view() { return overlay_->view(); }
  inline RefPtr<Window> window() { return window_; }
  inline RefPtr<Overlay> overlay() { return overlay_; }

  virtual void OnClose(ultralight::Window* window) override {
    // We quit the application when any of the windows are closed.
    App::instance()->Quit();
  }

  virtual void OnResize(ultralight::Window* window, uint32_t width, uint32_t height) override {
    overlay_->Resize(width, height);
  }

  virtual void OnChangeCursor(ultralight::View* caller, ultralight::Cursor cursor) override {
    window_->SetCursor(cursor);
  }
};

class EditorListener {
public:
  EditorListener() {}
  virtual ~EditorListener() {}
  virtual void OnUpdateEditor(const ultralight::String& content) = 0;
};

class EditorWindow : public HTMLWindow,
                     public LoadListener {
  EditorListener* editor_listener_ = nullptr;
public:
  EditorWindow(const char* title, const char* url, int x, int y, int width, int height) 
    : HTMLWindow(title, url, x, y, width, height) {
    view()->set_load_listener(this);
  }

  void set_editor_listener(EditorListener* listener) { editor_listener_ = listener; }

  EditorListener* editor_listener() { return editor_listener_; }

  ///
  /// Inherited from LoadListener, called when the page has finished parsing
  /// the document.
  ///
  /// We perform all our JavaScript initialization here.
  ///
  virtual void OnDOMReady(ultralight::View* caller, uint64_t frame_id, bool is_main_frame,
    const String& url) override {
    ///
    /// Set our View's JSContext as the one to use in subsequent JSHelper calls
    ///
    RefPtr<JSContext> context = caller->LockJSContext();
    SetJSContext(context->ctx());

    ///
    /// Get the global object (this would be the "window" object in JS)
    ///
    JSObject global = JSGlobalObject();

    ///
    /// Bind EditorWindow::UpdateEditor to the JavaScript function named "UpdateEditor".
    ///
    /// You can get/set properties of JSObjects by using the [] operator with
    /// the following types as potential property values:
    ///  - JSValue
    ///      Represents a JavaScript value, eg String, Object, Function, etc.
    ///  - JSCallback 
    ///      Typedef of std::function<void(const JSObject&, const JSArgs&)>)
    ///  - JSCallbackWithRetval 
    ///      Typedef of std::function<JSValue(const JSObject&, const JSArgs&)>)
    ///
    /// We use the BindJSCallbackWithRetval macro to bind our C++ class member
    /// function to our JavaScript callback.
    ///
    global["UpdateEditor"] = BindJSCallbackWithRetval(&EditorWindow::UpdateEditor);

    caller->EvaluateScript("var content = editor.getSession().getValue(); UpdateEditor(content);");
  }

  ///
  /// Our native JavaScript callback. This function will be called from
  /// JavaScript by calling UpdateEditor(). We bind the callback within
  /// the DOMReady callback defined below.
  ///
  JSValue UpdateEditor(const JSObject& thisObject, const JSArgs& args) {
    if (editor_listener_ && args.size() == 1)
      editor_listener_->OnUpdateEditor(args[0].ToString());

    return JSValue();
  }
};

class MyApp : public EditorListener  {
  RefPtr<App> app_;
  std::unique_ptr<EditorWindow> editor_window_;
  std::unique_ptr<HTMLWindow> preview_window_;
public:
  MyApp() {
    ///
    /// Create our main App instance.
    ///
    /// The App class is responsible for the lifetime of the application
    /// and is required to create any windows.
    ///
    app_ = App::Create();

    editor_window_.reset(new EditorWindow("Ultralight Sample 9 - HTML Editor", "file:///editor.html", 50, 50, 600, 700));
    editor_window_->set_editor_listener(this);

    preview_window_.reset(new HTMLWindow("Ultralight Sample 9 - Live Preview", "file:///preview.html", 700, 50, 600, 700));
  }

  virtual ~MyApp() {}

  virtual void OnUpdateEditor(const ultralight::String& content) {
    RefPtr<JSContext> context = preview_window_->view()->LockJSContext();
    SetJSContext(context->ctx());
    JSObject global = JSGlobalObject();
    global["html_content"] = content;

    preview_window_->view()->EvaluateScript("document.open(); document.write(html_content); document.close();");
  }

  void Run() {
    app_->Run();
  }
};

int main() {
  MyApp app;
  app.Run();

  return 0;
}
