# Ultralight

__Welcome to the Ultralight Universal SDK!__

This package contains everything you need to start building cross-platform HTML apps.

__What is Ultralight?__

Ultralight is a lightweight, pure-GPU, HTML rendering engine for native apps. Our aim is to provide support for the majority of the HTML5/CSS/JavaScript spec while still being as lightweight (in binary size and memory usage) as possible.

The motivation for this project stemmed from the observation that Chromium and other layout engines have become suboptimal for use in desktop app UI due to a separate set of design goals (running untrusted code, favoring performance at the cost of memory, the need to support every web platform feature under the sun, etc.).

Most native apps also need finer control over low-level platform functionality (such as file system, rendering, clipboard, etc.). Ultralight aims to not just be lightweight, but to offer native app developers much deeper integration with the underlying HTML engine.

Ultralight is also pure-GPU, meaning that all rendering (text, shadows, images, CSS transforms), is done via the GPU. The renderer emits abstract GPU commands (see the GPUDriver interface) which can then be handled by whichever graphics API you prefer (we provide stock implementations for D3D11, OpenGL, and Metal as of this writing).

__Cross-Platform AppCore Runtime__

On top of Ultralight, we've built an additional layer called AppCore that handles window creation, native event loops, native graphics API drivers, and more.

AppCore is intended to be used by those intending to build a standalone HTML-based desktop app and will eventually offer an API similar to Electron.

## Useful Links

| Link              | URL                                 |
| ----------------- | ----------------------------------- |
| __Slack Channel__ | <https://chat.ultralig.ht>          |
| __Twitter__       | <https://twitter.com/ultralight_ux> |
| __API Docs__      | <https://ultralig.ht/api/1_0/>      |


# Table of Contents

 - [Getting Started](#getting-started)
 	- [Build Requirements](#build-requirements)
 - [Building Sample Projects](#building-sample-projects)
 	- [Building Samples with CMake (All Platforms)](#building-samples-with-cmake-all-platforms)
        - [Building 64-bit on Windows](#building-64-bit-on-windows)
 - [Using the C++ API](#using-the-c-api)
 	- [Compiler / Linker Flags](#compiler--linker-flags)
 		- [Setting Your Include Directories](#setting-your-include-directories)
 		- [Linking to the Library (Windows / MSVC)](#linking-to-the-library-windows--msvc)
 		- [Linking to the Library (Linux)](#linking-to-the-library-linux)
 		- [Linking to the Library (macOS)](#linking-to-the-library-macos)
 	- [API Headers](#api-headers)
 	- [Platform Handlers](#platform-handlers)
 		- [Config](#config)
 		- [GPUDriver](#gpudriver)
 		- [FontLoader](#fontloader)
 		- [FileSystem](#filesystem)
  	- [Rendering](#rendering)
 		- [Creating the Renderer](#creating-the-renderer)
 		- [Updating the Renderer](#updating-the-renderer)
 		- [Drawing View Overlays](#drawing-view-overlays)
 	- [Managing Views](#managing-views)
 		- [Creating Views](#creating-views)
 		- [Loading Content](#loading-content)
 		- [Passing Mouse / Keyboard Input](#passing-mouse--keyboard-input)
 		- [Handling View Events](#handling-view-events)
 			- [ViewListener Interface](#viewlistener-interface)
 			- [LoadListener Interface](#loadlistener-interface)
 	- [JavaScript Integration](#javascript-integration)
		- [JavaScriptCore API](#javascriptcore-api)
		- [Set the JSContext](#set-the-jscontext)
		- [DOMReady Event](#domready-event)
		- [Evaluating Scripts](#evaluating-scripts)
		- [Calling JS Functions from C++](#calling-js-functions-from-c)
		- [Calling C++ Functions from JS](#calling-c-functions-from-js)
 - [Using the AppCore Code](#using-the-appcore-code)

# Getting Started

Before you get started, you will need the following on each platform:

## Build Requirements

Common requirements for all platforms 

 - CMake 2.8.12 or later
 - Compiler with C++11 or later

# Building Sample Projects

## Building Samples with CMake (All Platforms)

To generate projects for your platform and build, run the following from this directory:

```
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

### Building 64-bit on Windows

If you run the command `cmake ..` without any generators on Windows, it will usually select the default 32-bit Visual Studio version you have installed. To generate projects for 64-bit on Windows you will need to explicitly tell CMake to use the `x64` platform.

```
mkdir build64
cd build64
cmake .. -DCMAKE_GENERATOR_PLATFORM=x64
cmake --build . --config Release
```

### Running the Samples

On __macOS__ and __Linux__ the projects will be built to:

```
/build/samples/Browser/
```

On __Windows__ the projects will be built to:

```
/build/samples/Browser/$(Configuration)
```

# Using the C++ API

## Compiler / Linker Flags

### Setting your Include Directories

To use Ultralight in your C++ code, simply add the following directory to your project's include directories (replace `$(ULTRLIGHT_SDK_ROOT)` with the actual path you've placed the SDK):

```
$(ULTRALIGHT_SDK_ROOT)/include/
```

### Linking to the Library (Windows / MSVC)

In Visual Studio, go to __Linker &rarr; General &rarr; Additional Library Directories__ in your project's properties and set one of the following:

> For __Win32 / x86__ Platforms:
> 
> ```
> $(ULTRALIGHT_SDK_ROOT)/lib/win/x86/
> ```

__OR__

> For __x64__ Platforms:
>
> ```
> $(ULTRALIGHT_SDK_ROOT)/lib/win/x64/
> ```

Then, go to __Linker &rarr; Input &rarr; Additional Dependencies__ and add the following:

```
Ultralight.lib
UltralightCore.lib
WebCore.lib
AppCore.lib
```

> *__Note__: `AppCore.lib` is optional, only link if you use the AppCore API headers..*

### Linking to the Library (Linux)

First, copy the shared libraries in `$(ULTRALIGHT_SDK_ROOT)/bin/linux` to your OS's standard library directory.

Then, add the following to your Makefile's `LDFLAGS`:

```
-lUltralight -lUltralightCore -lWebCore -lAppCore
```

> *__Note__: `-lAppCore` is optional, only link if you use the AppCore API headers..*

### Linking to the Library (macOS)

Within XCode, select your target and go to __General &rarr; Linked Frameworks and Libraries__ and add the following:

```
libUltralightCore.dylib
libUltralight.dylib
libWebCore.dylib
libAppCore.dylib
```

Or alternatively, if you are building with a Makefile, add the following to your `LDFLAGS`:

```
-lUltralight -lUltralightCore -lWebCore -lAppCore
```

> *__Note__: AppCore is optional, only link if you use the AppCore API headers..*

## API Headers

Simply include `<Ultralight/Ultralight.h>` at the top of your code to import the API.

```cpp
#include <Ultralight/Ultralight.h>
```

If you want to use the optional AppCore API (cross-platform windowing/drawing layer), you should also include `<AppCore/AppCore.h>` at the top of your code.

```cpp
#include <AppCore/AppCore.h>
```

Ultralight also exposes the full __JavaScriptCore__ API so that users can make native calls to/from the JavaScript VM. To include these headers simply add:

```cpp
#include <JavaScriptCore/JavaScriptCore.h>
```

## Platform Handlers

Most OS-specific tasks in Ultralight can be overridden by users via the `Platform` interface.

Default platform implementations have been provided for everything except the `FileSystem` interface.

Of special note, the default GPUDriver is an offscreen OpenGL implementation that renders each View to a bitmap (see `View::bitmap`). This isn't the most performant option so you should instead use a native driver for each platform (eg, Metal on macOS). Platform-native drivers are automatically used when creating a Window through the AppCore API.

```cpp
auto& platform = Platform::instance();

platform.set_config(config_);
platform.set_gpu_driver(gpu_driver_);
platform.set_file_system(file_system_);
platform.set_font_loader(font_loader_);
```

### Config

The __Config__ class allows you to configure renderer behavior and runtime WebKit options.

The most common things to customize are `face_winding` for front-facing triangles and `device_scale_hint` for application DPI scaling (used for oversampling raster output).

You can also set the default font (instead of Times New Roman).

```cpp
Config config_;
config.face_winding = kFaceWinding_Clockwise; // CW in D3D, CCW in OGL
config.device_scale_hint = 1.0;               // Set DPI to monitor DPI scale
config.font_family_standard = "Segoe UI";     // Default font family

Platform::instance().set_config(config_);
```

### GPUDriver

The virtual __GPUDriver__ interface is used to perform all rendering in Ultralight.

Reference implementations for Direct3D11, OpenGL 3.2, Metal 2, and others are provided in the AppCore code (see `deps/AppCore/src/` in the SDK).

```cpp
class UExport GPUDriver
{
public:
  virtual ~GPUDriver();

  /******************************
   * Synchronization            *
   ******************************/

  virtual void BeginSynchronize() = 0;
  virtual void EndSynchronize() = 0;

  /******************************
   * Textures                   *
   ******************************/

  virtual uint32_t NextTextureId() = 0;
  virtual void CreateTexture(uint32_t texture_id,
                             Ref<Bitmap> bitmap) = 0;
  virtual void UpdateTexture(uint32_t texture_id,
                             Ref<Bitmap> bitmap) = 0;
  virtual void BindTexture(uint8_t texture_unit,
                           uint32_t texture_id) = 0;
  virtual void DestroyTexture(uint32_t texture_id) = 0;

  /******************************
   * Render Buffers             *
   ******************************/

  virtual uint32_t NextRenderBufferId() = 0;
  virtual void CreateRenderBuffer(uint32_t render_buffer_id,
                                 const RenderBuffer& buffer) = 0;
  virtual void BindRenderBuffer(uint32_t render_buffer_id) = 0;
  virtual void SetRenderBufferViewport(uint32_t render_buffer_id,
                                       uint32_t width,
                                       uint32_t height) = 0;
  virtual void ClearRenderBuffer(uint32_t render_buffer_id) = 0;
  virtual void DestroyRenderBuffer(uint32_t render_buffer_id) = 0;

  /******************************
   * Geometry                   *
   ******************************/

  virtual uint32_t NextGeometryId() = 0;
  virtual void CreateGeometry(uint32_t geometry_id,
                              const VertexBuffer& vertices,
                              const IndexBuffer& indices) = 0;
  virtual void UpdateGeometry(uint32_t geometry_id,
                              const VertexBuffer& vertices,
                              const IndexBuffer& indices) = 0;
  virtual void DrawGeometry(uint32_t geometry_id,
                            uint32_t indices_count,
                            uint32_t indices_offset,
                            const GPUState& state) = 0;
  virtual void DestroyGeometry(uint32_t geometry_id) = 0;

  /******************************
   * Command List               *
   ******************************/

  virtual void UpdateCommandList(const CommandList& list) = 0;
  virtual bool HasCommandsPending() = 0;
  virtual void DrawCommandList() = 0;
};
```

### FontLoader

The virtual __FontLoader__ interface is used to load font files (TrueType and OpenType files, usually stored somewhere in the Operating System) by font family.

```cpp
class UExport FontLoader
{
public:
  virtual ~FontLoader();

  virtual String16 fallback_font() const = 0;
  
  virtual Ref<Buffer> Load(const String16& family,
                           int weight,
                           bool italic,
                           float size) = 0;
};
```

### FileSystem

The virtual __FileSystem__ interface is used for loading File URLs (eg, `file:///page.html`) and the JavaScript FileSystem API.

This API should be used to load any HTML/JS/CSS assets you've bundled with your application.

Only a small subset needs to be implemented to support File URL loading, specifically the following: 

```cpp
class UExport FileSystem
{
public:
  virtual ~FileSystem();

  virtual bool FileExists(const String16& path) = 0;

  virtual bool GetFileSize(FileHandle handle,
                           int64_t& result) = 0;

  virtual bool GetFileMimeType(const String16& path,
                               String16& result) = 0;

  virtual FileHandle OpenFile(const String16& path,
                              bool open_for_writing) = 0;

  virtual void CloseFile(FileHandle& handle) = 0;

  virtual int64_t ReadFromFile(FileHandle handle,
                               char* data,
                               int64_t length) = 0;
};
```

## Rendering

### Creating the Renderer

The __Renderer__ class is used to create __Views__ and update them.

You should create only one instance per application lifetime:

```cpp
Ref<Renderer> renderer = Renderer::Create();
```

### Updating the Renderer

Once per frame, you should call `Renderer::Update()` and `Renderer::Render()`, here is brief outline of how your Update function should look:

```cpp
void MyApplication::Update()
{
  // Update internal logic (timers, event callbacks, etc.)
  renderer->Update();

  driver->BeginSynchronize();

  // Render all active views to command lists and dispatch calls to GPUDriver
  renderer->Render();

  driver->EndSynchronize();

  // Draw any pending commands to screen
  if (driver->HasCommandsPending())
  {
    driver->DrawCommandList();
    
    // Perform any additional drawing (Overlays) here...
    DrawOverlays();

    // Flip buffers here.
  }
}
```

### Drawing View Overlays

When using your own GPUDriver, Views are rendered to an offscreen texture and so it is the user's responsibility to draw this texture to the screen. To get the __Texture ID__ for a __View__, please see `View::render_target()`.

## Managing Views

__Views__ are used to display and interact with web content in Ultralight.

### Creating Views

To create a View, simply call `Renderer::CreateView()` with your desired width and height:

```
Ref<View> view = renderer_->CreateView(800, 600, false);
```

### Loading Content

You can load content into a View via either `View::LoadHTML()` or `View::LoadURL()`:

```cpp
view->LoadHTML("<h1>Hello World</h1>"); // HTML string

view->LoadURL("http://www.google.com"); // Remote URL

view->LoadURL("file:///asset.html");    // File URL
```
> *__Note__: To load local File URLs, make sure your FileSystem resolves file paths relative to your application's asset directory.*

### Passing Mouse / Keyboard Input

You can pass input events to a __View__ via the following methods:

```cpp
view->FireMouseEvent(mouse_event);

view->FireKeyEvent(key_event);

view->FireScrollEvent(scroll_event);
```

Look at `MouseEvent.h`, `KeyEvent.h`, and `ScrollEvent.h` for more information.

### Handling View Events

You can set callbacks for various __View__-related events by implementing the __ViewListener__ interface and/or the __LoadListener__ interface.

#### ViewListener Interface

To listen for View-specific events, you should inherit from the virtual __ViewListener__ interface and bind your instance to a __View__ via `View::set_view_listener()`.

```cpp
class MyViewListener : public ViewListener
{
public:
  MyViewListener() {}
  virtual ~MyViewListener() {}

  virtual void OnChangeTitle(View* caller,
                             const String& title) {}
  virtual void OnChangeURL(View* caller,
                           const String& url) {}
  virtual void OnChangeTooltip(View* caller,
                               const String& tooltip) {}
  virtual void OnChangeCursor(View* caller,
                              Cursor cursor) {}
  virtual void OnAddConsoleMessage(View* caller,
                                   MessageSource source,
                                   MessageLevel level,
                                   const String& message,
                                   uint32_t line_number,
                                   uint32_t column_number,
                                   const String& source_id) {}
};

// ... <snip>

// Later, bind an instance of MyViewListener to your View
view->set_view_listener(new MyViewListener());
```

#### LoadListener Interface

To listen for page load events, you should inherit from the virtual __LoadListener__ interface and bind your instance to a __View__ via `View::set_load_listener()`.

```cpp
class MyLoadListener : public LoadListener
{
public:
  MyLoadListener() {}
  virtual ~MyLoadListener() {}

  virtual void OnBeginLoading(View* caller) {}
  virtual void OnFinishLoading(View* caller) {}
  virtual void OnUpdateHistory(View* caller) {}
  virtual void OnDOMReady(View* caller) {}
};

// ... <snip>

// Later, bind an instance of MyLoadListener to your View
view->set_load_listener(new MyLoadListener());
```

## JavaScript Integration

### JavaScriptCore API

Ultralight exposes the entire __JavaScriptCore C API__ for maximum performance and flexibility. This allows you to make direct calls to and from the native JavaScriptCore VM.

To include this API in your code, simply include `<JavaScriptCore/JavaScript.h>`

```cpp
#include <JavaScriptCore/JavaScript.h>
```

To simplify things, a C++ wrapper for JavaScriptCore is provided in the AppCore code __(we'll be using this in subsequent code examples)__. Simply include `<AppCore/JSHelpers.h>` to use it:

```cpp
#include <AppCore/JSHelpers.h>
```

### Set the JSContext

Before you can make any calls to JavaScript code (including creating any JSValues, JSObjects, etc.), you should __pass your View's JSContext to `SetJSContext()`__:

```cpp
#include <AppCore/JSHelpers.h>
using namespace framework;

//...

// Get JSContext from a View
JSContextRef myContext = view->js_context();

// Set the JSContext for all subsequent JSHelper calls
SetJSContext(myContext);

// Now we can create JSValues/JSObjects and call JavaScript code
JSValue val = 42;

JSObject obj;
obj["myProperty"] = val;

JSValue result = JSEval("1 + 1");
```

> __Note: you can create JSStrings and JSFunctions without a context.__
> 
> ```cpp
> // Don't need a context to create JSStrings:
> JSString str = "Hello!";
> 
> // Don't need a context to default construct JSFunctions:
> JSFunction emptyFunc;
> emptyFunc.IsValid(); // Evaluates to FALSE
> ```

### DOMReady Event

You should perform all API calls that reference DOM elements or scripts on a page in the __DOMReady Event__. See the __[LoadListener Interface](#loadlistener-interface)__ above for details on how to register your own listener for this event.

```cpp
struct MyListener : public LoadListener
{
  virtual void OnDOMReady(View* caller) override
  {
    SetJSContext(caller->js_context());

    // Perform page-specific JavaScript here.
  }
};
```

### Evaluating Scripts

You can evaluate JavaScript in the current context by calling `JSEval()`.

```cpp
JSValue result = JSEval("1 + 1");

result.IsNumber(); // TRUE

result.ToInteger(); // 2
```

### Calling JS Functions from C++

Let's say you had the following JavaScript function defined on your page:

```js
function addNumbers(a, b) { return a + b; }
```

To get the function in C++, you would simply call:

```cpp
// Get the global object
JSObject global = JSGlobalObject();

// Get the "addNumbers" property as a JSFunction. (debug assert if invalid)
JSFunction addNumbers = global["addNumbers"];
```

Call the JSFunction object (takes an initializer list of zero or more JSValues):

```cpp
// Check if function is valid first.
if (addNumbers.IsValid())
{
  // Call the JSFunction
  JSValue result = addNumbers({ 1, 1 }); // will equal 2
  
  /**
   * NOTE: You can also pass a JSObject as the first parameter of a
   * JSFunction invocation to specify the 'this' object in JavaScript.
   *
   * If you don't specify one like above, the Global Object is used.
   */
   addNumbers(myObject, { 1, 1 });
}
```

### Calling C++ Functions from JS

First define your C++ callback function with the following signature:

```cpp
void MyClass::MyCallback(const JSObject& thisObject, const JSArgs& args)
{
  // Handle callback here.
}

/**
 * NOTE: You can also bind callbacks with return values: simply use JSValue
 * in your function's return value. The rest of the code remains the same.
 */
JSValue MyClass::MyCallback(const JSObject& thisObject, const JSArgs& args)
{
  // Handle callback here.
  
  return JSValue();
}
```

Then simply bind it to a named property using the `BindJSCallback` macro:

```cpp
// Get the global object
JSObject global = JSGlobalObject();

// Bind it to the "MyCallback" property, will be exposed to JS as a Function
global["MyCallback"] = BindJSCallback(&MyClass::MyCallback);
```

Now you can call it from JavaScript on the page:

```js
MyCallback(1, 2, 3, "hello");
```

# Using the AppCore Code

The AppCore code provides a cross-platform base for you to start writing applications with Ultralight-- the only thing you need to provide are HTML assets and application logic.

Take a look at the Browser sample code and Tutorials for an example of use.
