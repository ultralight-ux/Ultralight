#include <AppCore/App.h>
#include <AppCore/Window.h>
#include <AppCore/Overlay.h>

using namespace ultralight;

const char* htmlString_LeftPane();
const char* htmlString_RightPane();

#define WINDOW_WIDTH    900
#define WINDOW_HEIGHT   600
#define LEFT_PANE_WIDTH 200

///
/// Welcome to Sample 3!
///
/// In this sample we'll continue working with the AppCore API and show
/// how to make your app responsive to changes in window size by updating the
/// layout of multiple overlays.
///
/// We will create a window with two overlays-- a left pane with a fixed width
/// and a right pane that takes up the remaining width.
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
/// To respond to resize events, we'll attach a WindowListener to our window
/// and re-calculate layout of our overlays in the OnResize callback.
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
    window_ = Window::Create(app_->main_monitor(), WINDOW_WIDTH, WINDOW_HEIGHT,
      false, kWindowFlags_Titled | kWindowFlags_Resizable);

    ///
    /// Set the title of our window.
    ///
    window_->SetTitle("Ultralight Sample 3 - Resize Me!");

    ///
    /// Tell our app to use 'window' as our main window.
    ///
    /// This call is required before creating any overlays or calling App::Run
    ///
    app_->set_window(*window_.get());

    ///
    /// Create the overlays for our left and right panes-- we don't care about
    /// their initial size and position because they'll be set when we call
    /// OnResize() below.
    ///
    left_pane_ = Overlay::Create(*window_.get(), 1, 1, 0, 0);
    right_pane_ = Overlay::Create(*window_.get(), 1, 1, 0, 0);

    ///
    /// Force a call to OnResize to perform initial layout and sizing of our
    /// left and right overlays.
    ///
    OnResize(window_->width(), window_->height());

    ///
    /// Load some HTML into our left and right overlays.
    ///
    left_pane_->view()->LoadHTML(htmlString_LeftPane());
    right_pane_->view()->LoadHTML(htmlString_RightPane());

    ///
    /// Register our MyApp instance as a WindowListener so we can handle the
    /// Window's OnResize event below.
    ///
    window_->set_listener(this);

    ///
    /// Register our MyApp instance as a ViewListener so we can handle the
    /// Views' OnChangeCursor event below.
    ///
    left_pane_->view()->set_view_listener(this);
    right_pane_->view()->set_view_listener(this);
  }

  virtual ~MyApp() {}

  ///
  /// Inherited from WindowListener, not used.
  ///
  virtual void OnClose() override {}

  ///
  /// Inherited from WindowListener, called when the Window is resized.
  ///
  virtual void OnResize(uint32_t width, uint32_t height) override {
    uint32_t left_pane_width_px = window_->DeviceToPixels(LEFT_PANE_WIDTH);
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
  virtual void OnChangeCursor(ultralight::View* caller,
    Cursor cursor) {
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

const char* htmlString_LeftPane() {
  return R"(
<html>
  <head>
    <style type="text/css">
    * { -webkit-user-select: none; }
    body { 
      background: white;
      color: #7e7f8e;
      fill: #7e7f8e;
      font-family: -apple-system, 'Segoe UI', Ubuntu, Arial, sans-serif;
      margin: 8px 0;
    }
    li {
      list-style-type: none;
      padding: 0.4em 0;
      transition: padding 0.12s ease-out;
    }
    li:hover {
      padding-left: 6px;
      
    }
    li.disabled {
      color: #c0c0c8;
      fill: #c0c0c8;
    }
    li.active {
      border-left: 6px solid #44ddd5;
    }
    h3 {
      margin-top: 24px;
      margin-left: 20px;
    }
    li svg {
      margin-bottom: -0.1em;
      margin-right: 0.5em;
      margin-left: 20px;
    }
    li.active svg {
      margin-left: 14px;
    }
    ul {
      padding-left: 0em;
    }
    </style>
  </head>
  <body>
    <!-- SVG Icons from https://www.flaticon.com/packs/basic-ui-4 -->
    <svg width="0" height="0" xmlns="http://www.w3.org/2000/svg">
      <symbol id="dashboard" viewBox="0 0 512 512">
        <path d="m197.332031 170.667969h-160c-20.585937 0-37.332031-16.746094-37.332031-37.335938v-96c0-20.585937
        16.746094-37.332031 37.332031-37.332031h160c20.589844 0 37.335938 16.746094 37.335938 37.332031v96c0
        20.589844-16.746094 37.335938-37.335938 37.335938zm-160-138.667969c-2.941406 0-5.332031 2.390625-5.332031
        5.332031v96c0 2.945313 2.390625 5.335938 5.332031 5.335938h160c2.945313 0 5.335938-2.390625
        5.335938-5.335938v-96c0-2.941406-2.390625-5.332031-5.335938-5.332031zm0 0"/>
        <path d="m197.332031 512h-160c-20.585937 0-37.332031-16.746094-37.332031-37.332031v-224c0-20.589844
        16.746094-37.335938 37.332031-37.335938h160c20.589844 0 37.335938 16.746094 37.335938 37.335938v224c0
        20.585937-16.746094 37.332031-37.335938 37.332031zm-160-266.667969c-2.941406 0-5.332031 2.390625-5.332031
        5.335938v224c0 2.941406 2.390625 5.332031 5.332031 5.332031h160c2.945313 0 5.335938-2.390625
        5.335938-5.332031v-224c0-2.945313-2.390625-5.335938-5.335938-5.335938zm0 0"/>
        <path d="m474.667969 512h-160c-20.589844 0-37.335938-16.746094-37.335938-37.332031v-96c0-20.589844
        16.746094-37.335938 37.335938-37.335938h160c20.585937 0 37.332031 16.746094 37.332031 37.335938v96c0
        20.585937-16.746094 37.332031-37.332031 37.332031zm-160-138.667969c-2.945313 0-5.335938 2.390625-5.335938
        5.335938v96c0 2.941406 2.390625 5.332031 5.335938 5.332031h160c2.941406 0 5.332031-2.390625
        5.332031-5.332031v-96c0-2.945313-2.390625-5.335938-5.332031-5.335938zm0 0"/>
        <path d="m474.667969 298.667969h-160c-20.589844 0-37.335938-16.746094-37.335938-37.335938v-224c0-20.585937
        16.746094-37.332031 37.335938-37.332031h160c20.585937 0 37.332031 16.746094 37.332031 37.332031v224c0
        20.589844-16.746094 37.335938-37.332031 37.335938zm-160-266.667969c-2.945313 0-5.335938 2.390625-5.335938
        5.332031v224c0 2.945313 2.390625 5.335938 5.335938 5.335938h160c2.941406 0 5.332031-2.390625
        5.332031-5.335938v-224c0-2.941406-2.390625-5.332031-5.332031-5.332031zm0 0"/>
      </symbol>
      <symbol id="profile" viewBox="0 0 512 512">
        <path d="M437.02,330.98c-27.883-27.882-61.071-48.523-97.281-61.018C378.521,243.251,404,198.548,404,148
        C404,66.393,337.607,0,256,0S108,66.393,108,148c0,50.548,25.479,95.251,64.262,121.962
        c-36.21,12.495-69.398,33.136-97.281,61.018C26.629,379.333,0,443.62,0,512h40c0-119.103,96.897-216,216-216s216,96.897,216,216
        h40C512,443.62,485.371,379.333,437.02,330.98z M256,256c-59.551,0-108-48.448-108-108S196.449,40,256,40
        c59.551,0,108,48.448,108,108S315.551,256,256,256z"/>
      </symbol>
      <symbol id="favorites" viewBox="0 0 512 512">
        <path d="m114.59375 491.140625c-5.609375
        0-11.179688-1.75-15.933594-5.1875-8.855468-6.417969-12.992187-17.449219-10.582031-28.09375l32.9375-145.089844-111.703125-97.960937c-8.210938-7.167969-11.347656-18.519532-7.976562-28.90625
        3.371093-10.367188 12.542968-17.707032 23.402343-18.710938l147.796875-13.417968
        58.433594-136.746094c4.308594-10.046875 14.121094-16.535156 25.023438-16.535156 10.902343 0 20.714843
        6.488281 25.023437 16.511718l58.433594 136.769532 147.773437 13.417968c10.882813.980469 20.054688 8.34375
        23.425782 18.710938 3.371093 10.367187.253906 21.738281-7.957032 28.90625l-111.703125 97.941406 32.9375
        145.085938c2.414063 10.667968-1.726562 21.699218-10.578125 28.097656-8.832031 6.398437-20.609375
        6.890625-29.910156 1.300781l-127.445312-76.160156-127.445313 76.203125c-4.308594 2.558594-9.109375
        3.863281-13.953125 3.863281zm141.398438-112.875c4.84375 0 9.640624 1.300781 13.953124 3.859375l120.277344
        71.9375-31.085937-136.941406c-2.21875-9.746094 1.089843-19.921875
        8.621093-26.515625l105.472657-92.5-139.542969-12.671875c-10.046875-.917969-18.6875-7.234375-22.613281-16.492188l-55.082031-129.046875-55.148438
        129.066407c-3.882812 9.195312-12.523438 15.511718-22.546875 16.429687l-139.5625 12.671875
        105.46875 92.5c7.554687 6.613281 10.859375 16.769531 8.621094 26.539062l-31.0625 136.9375
        120.277343-71.914062c4.308594-2.558594 9.109376-3.859375 13.953126-3.859375zm-84.585938-221.847656s0
        .023437-.023438.042969zm169.128906-.0625.023438.042969c0-.023438 0-.023438-.023438-.042969zm0 0"/>
      </symbol>
      <symbol id="documents" viewBox="0 0 512 512">
        <path d="m410.667969 405.332031h-245.335938c-32.363281 0-58.664062-26.300781-58.664062-58.664062v-288c0-32.363281 
        26.300781-58.667969 58.664062-58.667969h181.503907c21.058593 0 41.6875 8.535156 56.554687 23.445312l42.496094
        42.496094c15.125 15.125 23.445312 35.222656 23.445312 56.574219v224.152344c0 32.363281-26.300781 58.664062-58.664062
        58.664062zm-245.335938-373.332031c-14.699219 0-26.664062 11.96875-26.664062 26.667969v288c0 14.699219
        11.964843 26.664062 26.664062 26.664062h245.335938c14.699219 0 26.664062-11.964843
        26.664062-26.664062v-224.152344c0-12.820313-4.992187-24.871094-14.058593-33.941406l-42.496094-42.496094c-8.9375-8.957031-21.289063-14.078125-33.941406-14.078125zm0 0"/>
        <path d="m314.667969 512h-256c-32.363281 0-58.667969-26.304688-58.667969-58.667969v-341.332031c0-32.363281
        26.304688-58.667969 58.667969-58.667969h10.664062c8.832031 0 16 7.167969 16 16s-7.167969 16-16
        16h-10.664062c-14.699219 0-26.667969 11.96875-26.667969 26.667969v341.332031c0 14.699219 11.96875
        26.667969 26.667969 26.667969h256c14.699219 0 26.664062-11.96875 26.664062-26.667969v-10.664062c0-8.832031
        7.167969-16 16-16s16 7.167969 16 16v10.664062c0 32.363281-26.300781 58.667969-58.664062 58.667969zm0 0"/>
        <path d="m368 181.332031h-160c-8.832031 0-16-7.167969-16-16s7.167969-16 16-16h160c8.832031 0 16 7.167969
        16 16s-7.167969 16-16 16zm0 0"/><path d="m368 245.332031h-160c-8.832031 0-16-7.167969-16-16s7.167969-16
        16-16h160c8.832031 0 16 7.167969 16 16s-7.167969 16-16 16zm0 0"/><path d="m368 309.332031h-160c-8.832031
        0-16-7.167969-16-16s7.167969-16 16-16h160c8.832031 0 16 7.167969 16 16s-7.167969 16-16 16zm0 0"/>
      </symbol>
      <symbol id="help" viewBox="0 0 512 512">
        <path d="m277.332031 384c0 11.78125-9.550781 21.332031-21.332031 21.332031s-21.332031-9.550781-21.332031-21.332031
        9.550781-21.332031 21.332031-21.332031 21.332031 9.550781 21.332031 21.332031zm0 0"/>
        <path d="m256 512c-141.164062 0-256-114.835938-256-256s114.835938-256 256-256 256 114.835938 256 256-114.835938
        256-256 256zm0-480c-123.519531 0-224 100.480469-224 224s100.480469 224 224 224 224-100.480469
        224-224-100.480469-224-224-224zm0 0"/><path d="m256 314.667969c-8.832031 0-16-7.167969-16-16v-21.546875c0-20.308594
        12.886719-38.507813 32.042969-45.269532 25.492187-8.980468 42.625-36.140624 42.625-55.851562
        0-32.363281-26.304688-58.667969-58.667969-58.667969s-58.667969 26.304688-58.667969 58.667969c0
        8.832031-7.167969 16-16 16s-16-7.167969-16-16c0-49.984375 40.664063-90.667969 90.667969-90.667969s90.667969
        40.683594 90.667969 90.667969c0 35.585938-28.097657 73.367188-63.980469 86.039062-6.398438 2.238282-10.6875
        8.316407-10.6875 15.101563v21.527344c0 8.832031-7.167969 16-16 16zm0 0"/>
      </symbol>
    </svg>
    <h3>Sidebar</h3>
    <ul>
      <li class="active"><svg width="16" height="16"><use xlink:href="#dashboard" /></svg>Dashboard</li>
      <li class="disabled"><svg width="16" height="16"><use xlink:href="#profile" /></svg>Profile</li>
      <li class="disabled"><svg width="16" height="16"><use xlink:href="#favorites" /></svg>Favorites</li>
      <li class="disabled"><svg width="16" height="16"><use xlink:href="#documents" /></svg>Documents</li>
      <li class="disabled"><svg width="16" height="16"><use xlink:href="#help" /></svg>Help</li>
    </ul>
  </body>
</html>
)";
}

const char* htmlString_RightPane() {
  return R"(
<html>
  <head>
    <style type="text/css">
    * { -webkit-user-select: none; }
    body { 
      background: #f3f5f7;
      color: #878a8d;
      font-family: -apple-system, 'Segoe UI', Ubuntu, 'Open Sans', Arial, 
                   sans-serif;
    }
    #divider {
      z-index: 100;
      position: fixed;
      width: 12px;
      height: 100%;
      left: 0;
      top: 0;
      background: linear-gradient(90deg, rgba(0, 0, 0, 0.25), rgba(0, 0, 0, 0.01));
    }
    h3 {
      margin-left: 10px;
      margin-top: 32px;
    }
    #message {
      font-weight: 600;
      margin: 0;
      width: 14em;
      height: 1.6em;
      border-left: 6px solid #44ddd5;
      border-radius: 5px;
      background: white;
      text-align: center;
      padding: 2em;
      box-shadow: 0 30px 35px -20px #e1e2e5;
      position: absolute;
      top: 50%;
      left: 50%;
      transform: translate(-50%, -50%);
      transition: top 0.15s ease-out, box-shadow 0.15s ease-out;
    }
    #message:hover {
      top: 49%;
      box-shadow: 0 35px 35px -20px #d3d5d9;
    }
    </style>
  </head>
  <body>
    <div id="divider"></div>
    <div id="contents">
      <h3>Dashboard</h3>
      <div id="message">
        Welcome to the Dashboard!
      </div>
    </div>
  </body>
</html>
)";
}
