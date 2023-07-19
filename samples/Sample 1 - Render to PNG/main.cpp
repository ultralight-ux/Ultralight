#include <Ultralight/Ultralight.h>
#include <AppCore/AppCore.h>
#include <iostream>
#include <string>
#include <memory>
#include <thread>
#include <chrono>

using namespace ultralight;

///
///  Welcome to Sample 1!
///
///  In this sample we'll load a local HTML file and render it to a PNG.
///  
///  Quick overview of the steps we'll cover:
/// 
///   1. Define our Platform handlers.
///   2. Create the Renderer.
///   3. Create our View.
///   4. Load a local file into the View.
///   5. Wait for it to load using our own main loop.
///   6. Render the View.
///   7. Get the rendered Bitmap and save it to a PNG.
///
class MyApp : public LoadListener,
              public Logger {
  RefPtr<Renderer> renderer_;
  RefPtr<View> view_;
  bool done_ = false;
public:
  MyApp() {

    ///
    /// Setup our config.
    /// 
    /// @note:
    ///   We don't set any config options in this sample but you could set your own options here.
    /// 
    Config config;

    ///
    /// Pass our configuration to the Platform singleton so that the library can use it.
    ///
    /// The Platform singleton can be used to define various platform-specific properties and
    /// handlers such as file system access, font loaders, and the gpu driver.
    ///
    Platform::instance().set_config(config);
    
    ///
    /// We must provide our own Platform API handlers since we're not using App::Create().
    /// 
    /// The Platform API handlers we can set are:
    /// 
    ///  - Platform::set_logger          (empty, optional)
    ///  - Platform::set_gpu_driver      (empty, optional)
    ///  - Platform::set_font_loader     (empty, **required**)
    ///  - Platform::set_file_system     (empty, **required**)
    ///  - Platform::set_clipboard       (empty, optional)
    ///  - Platform::set_surface_factory (defaults to BitmapSurfaceFactory, **required**)
    /// 
    /// The only Platform API handlers we are required to provide are file system and font loader.
    /// 
    /// In this sample we will use AppCore's font loader and file system via
    /// GetPlatformFontLoader() and GetPlatformFileSystem() respectively.
    ///
    /// You can replace these with your own implementations later.
    ///
    Platform::instance().set_font_loader(GetPlatformFontLoader());

    ///
    /// Use AppCore's file system singleton to load file:/// URLs from the OS.
    ///
    /// You could replace this with your own to provide your own file loader (useful if you need to
    /// bundle encrypted / compressed HTML assets).
    ///
    Platform::instance().set_file_system(GetPlatformFileSystem("./assets/"));

    ///
    /// Register our MyApp instance as a logger so we can handle the library's LogMessage() event
    /// below in case we encounter an error.
    ///
    Platform::instance().set_logger(this);

    ///
    /// Create our Renderer (you should only create this once per application).
    /// 
    /// The Renderer singleton maintains the lifetime of the library and is required before
    /// creating any Views. It should outlive any Views.
    ///
    /// You should set up the Platform singleton before creating this.
    ///
    renderer_ = Renderer::Create();

    ///
    /// Create our View.
    ///
    /// Views are sized containers for loading and displaying web content.
    /// 
    /// Let's set a 2x DPI scale and disable GPU acceleration so we can render to a bitmap.
    ///
    ViewConfig view_config;
    view_config.initial_device_scale = 2.0;
    view_config.is_accelerated = false;

    view_ = renderer_->CreateView(1600, 800, view_config, nullptr);

    ///
    /// Register our MyApp instance as a LoadListener so we can handle the View's OnFinishLoading
    /// event below.
    ///
    view_->set_load_listener(this);

    ///
    /// Load a local HTML file into the View (uses the file system defined above).
    ///
    /// @note:
    ///   This operation may not complete immediately-- we will call Renderer::Update continuously
    ///   and wait for the OnFinishLoading event before rendering our View.
    ///
    /// Views can also load remote URLs, try replacing the code below with:
    ///
    ///    view_->LoadURL("https://en.wikipedia.org");
    ///
    view_->LoadURL("file:///page.html");
  }

  virtual ~MyApp() {
    view_ = nullptr;
    renderer_ = nullptr;
  }

  void Run() {
    LogMessage(LogLevel::Info, "Starting Run(), waiting for page to load...");

    ///
    /// Continuously update until OnFinishLoading() is called below (which sets done = true).
    ///
    /// @note:
    ///   Calling Renderer::Update handles any pending network requests, resource loads, and 
    ///   JavaScript timers.
    ///
    do {
      renderer_->Update();
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    } while (!done_);

    ///
    /// Render our View.
    /// 
    /// @note:
    ///   Calling Renderer::Render will render any dirty Views to their respective Surfaces.
    /// 
    renderer_->Render();

    ///
    /// Get our View's rendering surface and cast it to BitmapSurface.
    ///
    /// BitmapSurface is the default Surface implementation, you can provide your own via
    /// Platform::set_surface_factory.
    ///
    BitmapSurface* bitmap_surface = (BitmapSurface*)view_->surface();
    
    ///
    /// Get the underlying bitmap.
    ///
    RefPtr<Bitmap> bitmap = bitmap_surface->bitmap();
    
    ///
    /// Write our bitmap to a PNG in the current working directory.
    ///
    bitmap->WritePNG("result.png");
    
    LogMessage(LogLevel::Info, "Saved a render of our page to result.png.");
  }

  ///
  /// Inherited from LoadListener, this is called when a View finishes loading a page into a frame.
  ///
  virtual void OnFinishLoading(ultralight::View* caller, uint64_t frame_id, bool is_main_frame,
                               const String& url) override {
    ///
    /// Our page is done when the main frame finishes loading.
    ///
    if (is_main_frame) {
      LogMessage(LogLevel::Info, "Our page has loaded!");

      ///
      /// Set our done flag to true to exit the Run loop.
      ///
      done_ = true;
    }
  }

  ///
  /// Inherited from Logger, this is called when the library wants to print a message to the log.
  ///
  virtual void LogMessage(LogLevel log_level, const String& message) override {
    std::cout << "> " << message.utf8().data() << std::endl << std::endl;
  }
};

int main() {
  MyApp app;
  app.Run();

  return 0;
}
