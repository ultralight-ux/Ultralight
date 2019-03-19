#include <Ultralight/Ultralight.h>
#include <iostream>
#include <string>
#include <memory>

using namespace ultralight;

class MyApp : public LoadListener {
  RefPtr<Renderer> renderer_;
  RefPtr<View> view_;
  bool done_ = false;
public:
  MyApp() {
    ///
    /// Create our Renderer (you should only create this once per application)
    ///
    renderer_ = Renderer::Create();

    ///
    /// Create our View.
    ///
    view_ = renderer_->CreateView(200, 200, false);

    ///
    /// Register our MyApp instance as a load listener so we can handle the
    /// View's OnFinishLoading event below.
    ///
    view_->set_load_listener(this);

    ///
    /// Load a string of HTML.
    ///
    view_->LoadHTML("<h1>Hello!</h1><p>Welcome to Ultralight!</p>");
  }

  virtual ~MyApp() {}

  void Run() {
    ///
    /// Continuously update our Renderer until are done flag is set to true.
    ///
    /// @note Calling Renderer::Update handles any pending network requests,
    ///       resource loads, and JavaScript timers.
    ///
    while (!done_)
      renderer_->Update();
  }

  ///
  /// Inherited from LoadListener, this event is called when the View finishes
  /// loading a page into the main frame.
  ///
  virtual void OnFinishLoading(ultralight::View* caller) {
    ///
    /// Render all Views (the default GPUDriver paints each View to an
    /// offscreen Bitmap which you can access via View::Bitmap)
    ///
    renderer_->Render();
    
    ///
    /// Get our View's bitmap and write it to a PNG.
    ///
    view_->bitmap()->WritePNG("result.png");

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
