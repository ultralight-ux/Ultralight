#include <AppCore/App.h>
#include <AppCore/Window.h>
#include <AppCore/Overlay.h>

using namespace ultralight;

const char* htmlString();

///
/// Welcome to Sample 2!
///
/// In this sample we'll introduce the AppCore API and use it to build a simple application that
/// creates a window and displays a string of HTML.
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
    /// Load a string of HTML into our overlay's View
    ///
    overlay_->view()->LoadHTML(htmlString());

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

const char* htmlString() {
  return R"(
<html>
  <head>
    <style type="text/css">
    * { -webkit-user-select: none; }
    body { 
      overflow: hidden;
      margin: 0;
      padding: 0;
      background-color: #e0e3ed;
      background: linear-gradient(-45deg, #e0e3ed, #f7f9fc);
      width: 900px;
      height: 600px;
      font-family: -apple-system, 'Segoe UI', Ubuntu, Arial, sans-serif;
    }
    h2, h3 {
      margin: 0;
    }
    div {
      padding: 35px;
      margin: 10px;
      height: 510px;
      width: 360px;
    }
    p, li { 
      font-size: 1em;
    }
    #leftPane {
      float: left;
      color: #858998;
      padding: 85px 65px;
      height: 410px;
      width: 300px;
    }
    #leftPane p {
      color: #858998;
    }
    #rightPane {
      border-radius: 15px;
      background-color: white;
      float: right;
      color: #22283d;
      box-shadow: 0 7px 24px -6px #aaacb8;
    }
    #rightPane li, #rightPane p {
      color: #7e7f8e;
      font-size: 0.9em;
    }
    #rightPane li {
      list-style-type: none;
      padding: 0.6em 0;
      border-radius: 20px;
      margin: 0;
      padding-left: 1em;
      cursor: pointer;
    }
    #rightPane li:hover {
      background-color: #f4f6fb;
    }
    li:before {
      content: '';
      display:inline-block; 
      height: 18; 
      width: 18;
      margin-bottom: -5px; 
      margin-right: 1em;
      background-image: url("data:image/svg+xml;utf8,<svg xmlns=\
'http://www.w3.org/2000/svg' width='18' height='18' viewBox='-2 -2 27 27'>\
<path stroke='%23dbe2e7' stroke-width='2' fill='white' d='M12 0c-6.627 0-12 \
5.373-12 12s5.373 12 12 12 12-5.373 12-12-5.373-12-12-12z'/></svg>");
    }
    li.checked:before {
      background-image: url("data:image/svg+xml;utf8,<svg xmlns=\
'http://www.w3.org/2000/svg' width='18' height='18' viewBox='0 0 24 24'><path \
fill='%2334d7d6' d='M12 0c-6.627 0-12 5.373-12 12s5.373 12 12 12 12-5.373 \
12-12-5.373-12-12-12zm-1.25 17.292l-4.5-4.364 1.857-1.858 2.643 2.506 \
5.643-5.784 1.857 1.857-7.5 7.643z'/></svg>");
    }
    #rightPane h5 {
      border-bottom: 1px solid #eceef0;
      padding-bottom: 9px;
      margin-bottom: 1em;
      margin-top: 3em;
    }
    #rightPane h5 {
      padding-left: 1em;
    }
    #rightPane ul {
      padding-left: 0;
    }
    </style>
    <script>
      window.onload = function() {
        var listItems = document.getElementsByTagName('li');
        for(var i = 0; i < listItems.length; i++) {
          listItems[i].onclick = function() {
            this.classList.toggle('checked');
          }
        }
      }
  </script>
  </head>
  <body>
    <div id="leftPane">
      <h2>My Planner App</h2>
      <p>Welcome to Ultralight Tutorial 2!</p>
    </div>
    <div id="rightPane">
      <h3>Upcoming Tasks</h3>
      <p>Click a task to mark it as completed.</p>
      <h5>Today</h5>
      <ul>
        <li class="checked">Create layout for initial mockup</li>
        <li class="checked">Select icons for mobile interface</li>
        <li class="checked">Discussions regarding new sorting algorithm</li>
        <li class="checked">Call with automotive clients</li>
        <li>Create quote for the Tesla remodel</li>
      </ul>
      <h5>Upcoming</h5>
      <ul>
        <li>Plan itinerary for conference</li>
        <li>Discuss desktop workflow optimizations</li>
        <li>Performance improvement analysis</li>
      </ul>
    </div>
  </body>
</html>
)";
}
