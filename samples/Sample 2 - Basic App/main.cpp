#include <AppCore/App.h>
#include <AppCore/Window.h>
#include <AppCore/Overlay.h>

using namespace ultralight;

///
/// Welcome to Sample 2!
///
/// In this sample we'll introduce the AppCore API and use it to build a simple application that
/// creates a window and displays a local HTML file.
///
/// __What is AppCore?__
///
/// AppCore is an optional, high-performance, cross-platform application framework built on top of
/// the Ultralight renderer.
///
/// It can be used to create standalone, GPU-accelerated HTML applications that paint directly to
/// the native window's backbuffer using the best technology available on each platform (D3D,
/// Metal, OpenGL, etc.).
///
/// We will create the simplest possible AppCore application in this sample.
///

class MyApp : public WindowListener,
              public ViewListener {
  RefPtr<App> app_;
  RefPtr<Window> window_;
  RefPtr<Overlay> overlay_;
public:
  MyApp() {
    ///
    /// Create our main App instance.
    ///
    /// The App class is responsible for the lifetime of the application and is required to create
    /// any windows.
    ///
    app_ = App::Create();

    ///
    /// Create our Window.
    ///
    /// This command creates a native platform window and shows it immediately.
    /// 
    /// The window's size (900 by 600) is in virtual device coordinates, the actual size in pixels
    /// is automatically determined by the monitor's DPI.
    ///
    window_ = Window::Create(app_->main_monitor(), 900, 600, false, kWindowFlags_Titled);

    ///
    /// Set the title of our window.
    ///
    window_->SetTitle("Ultralight Sample 2 - Basic App");

    ///
    /// Create a web-content overlay that spans the entire window.
    ///
    /// You can create multiple overlays per window, each overlay has its own View which can be
    /// used to load and display web-content.
    ///
    /// AppCore automatically manages focus, keyboard/mouse input, and GPU painting for each active
    /// overlay. Destroying the overlay will remove it from the window.
    ///
    overlay_ = Overlay::Create(window_, window_->width(), window_->height(), 0, 0);

    ///
    /// Load a local HTML file into our overlay's View
    ///
    overlay_->view()->LoadURL("file:///page.html");

    ///
    /// Register our MyApp instance as a WindowListener so we can handle the Window's OnClose event
    /// below.
    ///
    window_->set_listener(this);

    ///
    /// Register our MyApp instance as a ViewListener so we can handle the View's OnChangeCursor
    /// event below.
    ///
    overlay_->view()->set_view_listener(this);
  }

  virtual ~MyApp() {}

  ///
  /// Inherited from WindowListener, called when the Window is closed.
  /// 
  /// We exit the application when the window is closed.
  ///
  virtual void OnClose(ultralight::Window* window) override {
    app_->Quit();
  }

  ///
  /// Inherited from WindowListener, called when the Window is resized.
  /// 
  /// (Not used in this sample)
  ///
  virtual void OnResize(ultralight::Window* window, uint32_t width, uint32_t height) override {}

  ///
  /// Inherited from ViewListener, called when the Cursor changes.
  ///
  virtual void OnChangeCursor(ultralight::View* caller, ultralight::Cursor cursor) override {
    window_->SetCursor(cursor);
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
