#include <AppCore/CAPI.h>
#include <JavaScriptCore/JavaScript.h>

///
///  Welcome to Sample 6!
///
///  In this sample we'll demonstrate how to set up a simple JavaScript app
///  using only the C API.
///
///  We will bind a JavaScript function to a C callback and use it to display
///  a welcome message in our HTML view.
///
///  __About the C API__
///
///  The C API is useful for porting the library to other languages or using it
///  in scenarios that are unsuitable for C++.
///
///  Most of the C++ API functionality is currently available via the CAPI
///  headers with the exception of some of the Platform API.
///
///  JavaScriptCore's native API is already in C so we can use it directly.
///  
///  Both Ultralight and JavaScriptCore follow the same paradigm when it
///  comes to ownership/destruction: You should explicitly Destroy/Release
///  anything you Create.
///

/// Various globals
ULApp app = 0;
ULWindow window = 0;
ULOverlay overlay = 0;
ULView view = 0;

/// Forward declaration of our OnUpdate callback.
void OnUpdate(void* user_data);

/// Forward declaration of our OnResize callback.
void OnResize(void* user_data, unsigned int width, unsigned int height);

/// Forward declaration of our OnDOMReady callback.
void OnDOMReady(void* user_data, ULView caller, unsigned long long frame_id,
  bool is_main_frame, ULString url);

///
/// We set up our application here.
///
void Init() {
  ///
  /// Create default settings/config
  ///
  ULSettings settings = ulCreateSettings();
  ulSettingsSetForceCPURenderer(settings, true);
  ULConfig config = ulCreateConfig();

  ///
  /// Create our App
  ///
  app = ulCreateApp(settings, config);

  ///
  /// Register a callback to handle app update logic.
  ///
  ulAppSetUpdateCallback(app, OnUpdate, 0);

  ///
  /// Done using settings/config, make sure to destroy anything we create
  ///
  ulDestroySettings(settings);
  ulDestroyConfig(config);

  ///
  /// Create our window, make it 500x500 with a titlebar and resize handles.
  ///
  window = ulCreateWindow(ulAppGetMainMonitor(app), 500, 500, false,
    kWindowFlags_Titled | kWindowFlags_Resizable);

  ///
  /// Set our window title.
  ///
  ulWindowSetTitle(window, "Ultralight Sample 6 - Intro to C API");

  ///
  /// Register a callback to handle window resize.
  ///
  ulWindowSetResizeCallback(window, OnResize, 0);

  ///
  /// Tell our app to use this window as the main window.
  ///
  ulAppSetWindow(app, window);

  ///
  /// Create an overlay same size as our window at 0,0 (top-left) origin.
  /// Overlays also create an HTML view for us to display content in.
  ///
  /// **Note**:
  ///     Ownership of the view remains with the overlay since we don't
  ///     explicitly create it.
  ///
  overlay = ulCreateOverlay(window, ulWindowGetWidth(window), ulWindowGetHeight(window), 0, 0);
  
  ///
  /// Get the overlay's view.
  ///
  view = ulOverlayGetView(overlay);

  ///
  /// Register a callback to handle our view's DOMReady event. We will use this
  /// event to setup any JavaScript <-> C bindings and initialize our page.
  ///
  ulViewSetDOMReadyCallback(view, OnDOMReady, 0);

  ///
  /// Load a file from the FileSystem.
  ///
  ///  **IMPORTANT**: Make sure `file:///` has three (3) forward slashes.
  ///
  ///  **Note**: You can configure the base path for the FileSystem in the
  ///            Settings we passed to ulCreateApp earlier.
  ///
  ULString url = ulCreateString("file:///app.html");
  ulViewLoadURL(view, url);
  ulDestroyString(url);
}

///
/// This is called continuously from the app's main run loop. You should
/// update any app logic inside this callback.
///
void OnUpdate(void* user_data) {
  /// We don't use this in this tutorial, just here for example.
}

///
/// This is called whenever the window resizes. Width and height are in
/// DPI-independent logical coordinates (not pixels).
///
void OnResize(void* user_data, unsigned int width, unsigned int height) {
  ulOverlayResize(overlay, width, height);
}

///
/// This callback is bound to a JavaScript function on the page.
///
JSValueRef GetMessage(JSContextRef ctx, JSObjectRef function,
  JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[],
  JSValueRef* exception) {

  ///
  /// Create a JavaScript String from a C-string, initialize it with our
  /// welcome message.
  ///
  JSStringRef str = JSStringCreateWithUTF8CString("Hello from C!");

  ///
  /// Create a garbage-collected JSValue using the String we just created.
  ///
  ///  **Note**:
  ///    Both JSValueRef and JSObjectRef types are garbage-collected types.
  ///    (And actually, JSObjectRef is just a typedef of JSValueRef, they
  ///    share definitions).
  ///
  ///    The garbage collector in JavaScriptCore periodically scans the entire
  ///    stack to check if there are any active JSValueRefs, and marks those
  ///    with no references for destruction.
  ///
  ///    If you happen to store a JSValueRef/JSObjectRef in heap memory or
  ///    in memory unreachable by the stack-based garbage-collector, you should
  ///    explicitly call JSValueProtect() and JSValueUnprotect() on the
  ///    reference to ensure it is kept alive.
  ///
  JSValueRef value = JSValueMakeString(ctx, str);

  ///
  /// Release the string we created earlier (we only Release what we Create).
  ///
  JSStringRelease(str);

  return value;
}

///
/// This is called when the page has finished parsing the document and is ready
/// to execute scripts.
///
/// We will use this event to set up our JavaScript <-> C callback.
///
void OnDOMReady(void* user_data, ULView caller, unsigned long long frame_id,
                bool is_main_frame, ULString url) {
  ///
  /// Acquire the page's JavaScript execution context.
  ///
  /// This locks the JavaScript context so we can modify it safely on this
  /// thread, we need to unlock it when we're done via ulViewUnlockJSContext().
  ///
  JSContextRef ctx = ulViewLockJSContext(view);

  ///
  /// Create a JavaScript String containing the name of our callback.
  ///
  JSStringRef name = JSStringCreateWithUTF8CString("GetMessage");

  ///
  /// Create a garbage-collected JavaScript function that is bound to our
  /// native C callback 'GetMessage()'.
  ///
  JSObjectRef func = JSObjectMakeFunctionWithCallback(ctx, name, GetMessage);

  ///
  /// Store our function in the page's global JavaScript object so that it
  /// accessible from the page as 'GetMessage()'.
  ///
  /// The global JavaScript object is also known as 'window' in JS.
  ///
  JSObjectSetProperty(ctx, JSContextGetGlobalObject(ctx), name, func, 0, 0);

  ///
  /// Release the JavaScript String we created earlier.
  ///
  JSStringRelease(name);

  ///
  /// Unlock the JS context so other threads can modify JavaScript state.
  ///
  ulViewUnlockJSContext(view);
}

///
/// We tear down our application here.
///
void Shutdown() {
  ///
  /// Explicitly destroy everything we created in Init().
  ///
  ulDestroyOverlay(overlay);
  ulDestroyWindow(window);
  ulDestroyApp(app);
}

int main() {
  ///
  /// Initialize the app.
  ///
  Init();

  ///
  /// Run the app until the window is closed. (This is a modal operation)
  ///
  ulAppRun(app);

  ///
  /// Shutdown the app.
  ///
  Shutdown();

  return 0;
}
