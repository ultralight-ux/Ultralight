#include <AppCore/App.h>
#include <AppCore/Window.h>
#include <AppCore/Overlay.h>

using namespace ultralight;

#define WINDOW_WIDTH    900
#define WINDOW_HEIGHT   600
#define LEFT_PANE_WIDTH 200

///
/// Welcome to Sample 3!
///
/// In this sample we'll continue working with the AppCore API and show how to make your app
/// responsive to changes in window size by updating the layout of multiple overlays.
///
/// We will create a window with two overlays-- a left pane with a fixed width and a right pane
/// that takes up the remaining width.
///
///    +----------------------------------------------------+
///    |               |                                    |
///    |               |                                    |
///    |               |                                    |
///    |               |                                    |
///    |               |                                    |
///    |   Left Pane   |             Right Pane             |
///    |    (200px)    |              (Fluid)               |
///    |               |                                    |
///    |               |                                    |
///    |               |                                    |
///    |               |                                    |
///    |               |                                    |
///    +----------------------------------------------------+
/// 
/// To respond to resize events, we'll attach a WindowListener to our window and re-calculate
/// layout of our overlays in the OnResize callback.
///

class MyApp : public WindowListener,
              public ViewListener {
  RefPtr<App> app_;
  RefPtr<Window> window_;
  RefPtr<Overlay> left_pane_;
  RefPtr<Overlay> right_pane_;
public:
  MyApp() {
    ///
    /// Create our main App instance.
    ///
    app_ = App::Create();

    ///
    /// Create a resizable window by passing by OR'ing our window flags with
    /// kWindowFlags_Resizable.
    ///
    window_ = Window::Create(app_->main_monitor(), WINDOW_WIDTH, WINDOW_HEIGHT, false,
        kWindowFlags_Titled | kWindowFlags_Resizable);

    ///
    /// Set the title of our window.
    ///
    window_->SetTitle("Ultralight Sample 3 - Resize Me!");

    ///
    /// Create the overlays for our left and right panes-- we don't care about their initial size
    /// and position because they'll be set when we call OnResize() below.
    ///
    left_pane_ = Overlay::Create(window_.get(), 100, 100, 0, 0);
    right_pane_ = Overlay::Create(window_.get(), 100, 100, 0, 0);

    ///
    /// Force a call to OnResize to perform initial layout and sizing of our left and right
    /// overlays.
    ///
    OnResize(window_.get(), window_->width(), window_->height());

    ///
    /// Load some HTML into our left and right overlays.
    ///
    left_pane_->view()->LoadURL("file:///sidebar.html");
    right_pane_->view()->LoadURL("file:///content.html");

    ///
    /// Register our MyApp instance as a WindowListener so we can handle the Window's OnResize
    /// event below.
    ///
    window_->set_listener(this);

    ///
    /// Register our MyApp instance as a ViewListener so we can handle the Views' OnChangeCursor
    /// event below.
    ///
    left_pane_->view()->set_view_listener(this);
    right_pane_->view()->set_view_listener(this);
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
  virtual void OnResize(ultralight::Window* window, uint32_t width, uint32_t height) override {
    uint32_t left_pane_width_px = window_->ScreenToPixels(LEFT_PANE_WIDTH);
    left_pane_->Resize(left_pane_width_px, height);

    // Calculate the width of our right pane (window width - left width)
    int right_pane_width = (int)width - left_pane_width_px;

    // Clamp our right pane's width to a minimum of 1
    right_pane_width = right_pane_width > 1 ? right_pane_width: 1;

    right_pane_->Resize((uint32_t)right_pane_width, height);

    left_pane_->MoveTo(0, 0);
    right_pane_->MoveTo(left_pane_width_px, 0);
  }

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
