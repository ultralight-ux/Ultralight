#include <AppCore/App.h>
#include <AppCore/Window.h>
#include <AppCore/Overlay.h>
#include <AppCore/JSHelpers.h>

using namespace ultralight;

///
///  Welcome to Sample 5!
///
///  In this sample we'll show how to load assets from the local file system.
///
///  __FileSystem API__
///
///  Ultralight uses the FileSystem interface (part of the Platform API) to load file:/// URLs and
///  handle JavaScript FileSystem API requests.
///
///  AppCore provides default FileSystem implementations for Windows, macOS, and Linux
///  out-of-the-box. 
///
///   * On Windows/Linux, it will use ./assets/ as the base file path.
///   * On macOS, it will use the app bundle's @resource_path
///
///  You can configure the base file system path by passing your own Settings to App::Create.
///
///  When building your application, it's best to use a CMake script to copy these assets to your
///  application's output folder/bundle. Look at the CMakeLists.txt of this sample for an example.
///
///  __Custom FileSystem API__
///
///  You can provide your own FileSystem implementation in case you need greater control over file
///  loading. This is useful if you're using your own data format and/or need to encrypt or
///  compress resources.
///
///  See <Ultralight/platform/FileSystem.h> for more information.
///
class MyApp : public WindowListener {
  RefPtr<App> app_;
  RefPtr<Window> window_;
  RefPtr<Overlay> overlay_;
public:
  MyApp() {
    ///
    /// Create our main App instance.
    ///
    app_ = App::Create();

    ///
    /// Create our Window using default window flags.
    ///
    window_ = Window::Create(app_->main_monitor(), 450, 700, false, kWindowFlags_Titled);

    ///
    /// Set the title of our window.
    ///
    window_->SetTitle("Ultralight Sample 5 - File Loading");

    ///
    /// Create an Overlay using the same dimensions as our Window.
    ///
    overlay_ = Overlay::Create(window_, window_->width(), window_->height(), 0, 0);

    ///
    /// Load a file from the FileSystem.
    ///
    ///  **IMPORTANT**: Make sure `file:///` has three (3) forward slashes.
    ///
    overlay_->view()->LoadURL("file:///app.html");
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

  void Run() {
    app_->Run();
  }
};

int main() {
  MyApp app;
  app.Run();

  return 0;
}
