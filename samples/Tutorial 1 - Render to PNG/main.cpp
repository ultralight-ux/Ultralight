#include <Ultralight/Ultralight.h>
#include <iostream>
#include <string>
#include <memory>

using namespace ultralight;

const char* htmlString();

///
///  Welcome to Tutorial 1!
///
///  In this tutorial we'll load a string of HTML and render it to a PNG.
///  
///  Since we're rendering offscreen and don't need to create any windows or
///  handle any user input, we won't be using AppCore and will instead be using
///  the Ultralight API directly with our own custom main loop.
///
///  Our main loop waits for the page to finish loading by subscribing to the
///  LoadListener interface, then calls Render() once and writes the result to
///  a PNG on disk.
///
///  Underneath the hood, Ultralight renders everything via the GPUDriver
///  interface. If you don't provide a GPUDriver interface to the Platform
///  singleton, a default offscreen OpenGL implementation is used which we'll
///  be taking advantage of in this tutorial.
///
///  **Note**:
///    If you are integrating Ultralight into a game that has an existing GPU
///    context, it's recommended to provide your own GPUDriver implementation
///    so that Ultralight can render directly to it instead. See the AppCore
///    repo on GitHub for stock implementations for D3D, OpenGL, and Metal.
///
class MyApp : public LoadListener {
  RefPtr<Renderer> renderer_;
  RefPtr<View> view_;
  bool done_ = false;
public:
  MyApp() {
    ///
    /// Setup our config. The config can be used to customize various
    /// options within the renderer and WebCore module.
    ///
    /// Our config uses 2x DPI scale and "Arial" as the default font.
    ///
    Config config;
    config.device_scale_hint = 2.0;
    config.font_family_standard = "Arial";

    ///
    /// Pass our configuration to the Platform singleton so that the library
    /// can use it.
    ///
    /// The Platform singleton can be used to define various platform-specific
    /// properties and handlers such as file system access, font loaders, and
    /// the gpu driver.
    ///
    /// If you don't use the AppCore module (like we're doing now), Ultralight
    /// provides a default offscreen OpenGL GPU driver and default font loader.
    ///
    /// We don't define a FileSystem handler to resolve `file:///` URLs since
    /// we aren't using any in this tutorial.
    ///
    Platform::instance().set_config(config);

    ///
    /// Create our Renderer (you should only create this once per application).
    /// 
    /// The Renderer singleton maintains the lifetime of the library and
    /// is required before creating any Views. It should outlive any Views.
    ///
    /// You should set up the Platform singleton before creating this.
    ///
    renderer_ = Renderer::Create();

    ///
    /// Create our View.
    ///
    /// Views are sized containers for loading and displaying web content.
    ///
    /// **Note**:
    ///   Note that width and height are in virtual page coordinates--
    ///   the window will be 400 by 400 pixels according to CSS/JavaScript
    ///   running on the page but our renderer is using 2x DPI scale in our
    ///   config so the output bitmap will actually be 800 by 800 pixels.
    ///
    view_ = renderer_->CreateView(400, 400, false);

    ///
    /// Register our MyApp instance as a load listener so we can handle the
    /// View's OnFinishLoading event below.
    ///
    view_->set_load_listener(this);

    ///
    /// Load a string of HTML into our View. (For code readability, the string
    /// is defined in the htmlString() function at the bottom of this file)
    ///
    /// **Note**:
    ///   This operation may not complete immediately-- we will call
    ///   Renderer::Update continuously and wait for the OnFinishLoading event
    ///   before rendering our View.
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
    /// Continuously update our Renderer until are done flag is set to true.
    ///
    /// **Note**:
    ///   Calling Renderer::Update handles any pending network requests,
    ///   resource loads, and JavaScript timers.
    ///
    while (!done_)
      renderer_->Update();

    std::cout << "Finished." << std::endl;
  }

  ///
  /// Inherited from LoadListener, this event is called when the View finishes
  /// loading a page into the main frame.
  ///
  virtual void OnFinishLoading(ultralight::View* caller) {
    std::cout << "Our page has loaded! Rendering it now..." << std::endl;

    ///
    /// Render all Views that need painting.
    /// 
    /// If you supplied your own GPUDriver, this function only renders each
    /// View to a GPU command list for drawing in your own engine.
    ///
    /// We are using the default offscreen OpenGL driver which automatically
    /// does this for us and blits the FBO to a bitmap (View::bitmap()).
    ///
    renderer_->Render();
    
    ///
    /// Get our View's bitmap and write it to a PNG in the current working
    /// directory.
    ///
    /// **Note**:
    ///   Views are re-rendered as needed in the offscreen driver. You can
    ///   check if a View's bitmap has changed since the last Render call
    ///   via View::is_bitmap_dirty().
    ///
    ///   Calling View::bitmap() to retrieve the bitmap afterwards will clear
    ///   the dirty state.
    ///
    view_->bitmap()->WritePNG("result.png");

    std::cout << "Saved a render of our page to result.png." << std::endl;

    ///
    /// Set our done flag to true to exit the Run loop.
    ///
    done_ = true;
  }
};

int main() {
  MyApp app;
  app.Run();

  return 0;
}

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
          }
          div {
            width: 350px;
            height: 350px;
            text-align: center;
            margin: 25px;
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