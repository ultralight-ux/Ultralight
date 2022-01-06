#include <Ultralight/Ultralight.h>
#include <Ultralight/platform/Logger.h>
#include <AppCore/Platform.h>
#include <iostream>
#include <string>
#include <memory>
#include <thread>
#include <chrono>

using namespace ultralight;

const char* htmlString();

///
///  Welcome to Sample 1!
///
///  In this sample we'll load a string of HTML and render it to a PNG.
///  
///  Since we're rendering offscreen and don't need to create any windows or handle any user input,
///  we won't be using App::Create() and will instead be using the Ultralight API directly with our
///  own custom main loop.
///
///  Our main loop waits for the page to finish loading by subscribing to the LoadListener
///  interface then writes the rendering surface to a PNG on disk.
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
    /// Pass our configuration to the Platform singleton so that the library
    /// can use it.
    ///
    /// The Platform singleton can be used to define various platform-specific
    /// properties and handlers such as file system access, font loaders, and
    /// the gpu driver.
    ///
    Platform::instance().set_config(config);
    
    ///
    /// Since we're not using App::Create(), we must provide our own Platform API handlers.
    /// 
    /// The Platform API handlers we can set are:
    /// 
    ///  - Platform::set_logger          (empty, optional)
    ///  - Platform::set_gpu_driver      (empty, optional)
    ///  - Platform::set_font_loader     (empty, **required**)
    ///  - Platform::set_file_system     (empty, optional)
    ///  - Platform::set_clipboard       (empty, optional)
    ///  - Platform::set_surface_factory (defaults to BitmapSurfaceFactory, **required**)
    /// 
    /// The only Platform API handler we are required to provide is a font loader.
    /// 
    /// In this sample we will use AppCore's font loader singleton via GetPlatformFontLoader()
    /// which loads fonts from the OS.
    ///
    /// You could replace this with your own loader if you wanted to bundle your own fonts.
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
    /// Our view config uses 2x DPI scale and "Arial" as the default font.
    /// 
    /// We make sure GPU acceleration is disabled so we can render to an offscreen bitmap.
    ///
    ViewConfig view_config;
    view_config.initial_device_scale = 2.0;
    view_config.font_family_standard = "Arial";
    view_config.is_accelerated = false;

    view_ = renderer_->CreateView(1600, 1600, view_config, nullptr);

    ///
    /// Register our MyApp instance as a LoadListener so we can handle the View's OnFinishLoading
    /// event below.
    ///
    view_->set_load_listener(this);

    ///
    /// Load a string of HTML into our View. (For code readability, the string is defined in the
    /// htmlString() function at the bottom of this file)
    ///
    /// @note:
    ///   This operation may not complete immediately-- we will call Renderer::Update continuously
    ///   and wait for the OnFinishLoading event before rendering our View.
    ///
    /// Views can also load remote URLs, try replacing the code below with:
    ///
    ///    view_->LoadURL("https://en.wikipedia.org");
    ///
    view_->LoadHTML(htmlString());
  }

  virtual ~MyApp() {
    view_ = nullptr;
    renderer_ = nullptr;
  }

  void Run() {
    std::cout << "Starting Run(), waiting for page to load..." << std::endl;

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
    
    std::cout << "Saved a render of our page to result.png." << std::endl;

    std::cout << "Finished." << std::endl;
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
      std::cout << "Our page has loaded!" << std::endl;

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
    std::cout << message.utf8().data() << std::endl << std::endl;
  }
};

int main() {
  MyApp app;
  app.Run();

  return 0;
}

///
/// Our HTML string to load into the View.
/// 
const char* htmlString() {
  return R"(
    <html>
      <head>
        <style type="text/css">
          body {
            margin: 0;
            padding: 0;
            overflow: hidden;
            color: black;
            font-family: Arial;
            background: linear-gradient(-45deg, #acb4ff, #f5d4e2);
            display: flex;
            justify-content: center;
            align-items: center;
          }
          div {
            width: 350px;
            height: 350px;
            text-align: center;
            border-radius: 25px;
            background: linear-gradient(-45deg, #e5eaf9, #f9eaf6);
            box-shadow: 0 7px 18px -6px #8f8ae1;
          }
          h1 {
            padding: 1em;
          }
          p {
            background: white;
            padding: 2em;
            margin: 40px;
            border-radius: 25px;
          }
        </style>
      </head>
      <body>
        <div>
          <h1>Hello World!</h1>
          <p>Welcome to Ultralight!</p>
        </div>
      </body>
    </html>
    )";
}
