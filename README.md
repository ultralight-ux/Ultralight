# Ultralight

__Welcome to the Ultralight Universal SDK!__

This package contains everything you need to start building cross-platform HTML apps.

## Useful Links

| Link              | URL                                 |
| ----------------- | ----------------------------------- |
| __Slack Channel__ | <https://chat.ultralig.ht>          |
| __Support Forum__ | <https://discuss.ultralig.ht>       |
| __Twitter__       | <https://twitter.com/ultralight-ux> |


# Table of Contents

 - [Getting Started](#getting-started)
 	- [Build Requirements](#build-requirements)
 		- [Windows Build Requirements](#windows-build-requirements)
 		- [macOS Build Requirements](#macos-build-requirements)
 		- [Linux Build Requirements](#linux-build-requirements)
 - [Building Sample Projects](#building-sample-projects)
 	- [Building Samples with CMake (All Platforms)](#building-samples-with-cmake-all-platforms)
 	- [Building Samples (Windows)](#building-samples-windows)
 	- [Building Samples (macOS)](#building-samples-macos)
 - [Using the C++ API](#using-the-c-api)
 	- [Compiler / Linker Flags](#compiler-linker-flags)
 		- [Disabling RTTI](#disabling-rtti)
 		- [Setting Your Include Directories](#setting-your-include-directories)
 		- [Linking to the Library (Windows/MSVC)](#linking-to-the-library-windows-msvc)
 		- [Linking to the Library (Linux)](#linking-to-the-library-linux)
 		- [Linking to the Library (macOS)](#linking-to-the-library-macos)
 	- [API Headers](#api-headers)
 	- [Platform Handlers](#platform-handlers)
 		- [Config](#config)
 		- [GPUDriver](#gpudriver)
 		- [FontLoader](#fontloader)
 		- [FileSystem](#filesystem)
 	- [Update Loop]()
 	- [Managing Views]()
 		- [Loading Content]()
 		- [Input Events]()
 		- [Handling Events]()
 	- [JavaScript Interop]()
		- [JavaScriptCore API]()
		- [Obtaining a JSContext]()
		- [DOMReady Event]()
		- [Evaluating Scripts]()
		- [Calling JS Functions from C++]()
		- [Calling C++ Functions from JS]()
 - [Using the Framework Code]()
 	- [Select Framework Platform]()
	- [Writing Your Application]()
	- [JSHelpers Utility Code]()

# Getting Started

Before you get started, you will need the following on each platform:

## Build Requirements

All platforms include an __OpenGL-based sample (powered by GLFW)__. To build these cross-platform CMake/GLFW samples you will need:

 - CMake 2.8.12 or later
 - OpenGL 3.2 or later
 - Compiler with C++11 or later

### Windows Build Requirements

 - In addition to the above...
 	- Visual Studio 2015 or later
 	- DirectX 11+ or later __[optional], only for D3D11-based Samples__

### macOS Build Requirements
 - In addition to the above...
 	- XCode 8.0+ or later
 	- Metal 2 (macOS High Sierra or later) __[optional], only for Metal-based Samples__

### Linux Build Requirements

 - In addition to the above...
 	- 64-bit Debian-based OS (__Debian 9.5.0+__ OR __Ubuntu 12.04.5+__)


# Building Sample Projects

## Building Samples with CMake (All Platforms)

To generate projects for your platform, run the following from this directory:

```
mkdir build
cd build
cmake ..
```

On __macOS__ and __Linux__ you can then simply build the generated Makefile:

```
make
```

On __Windows__, a Visual Studio solution will be generated in the `build` directory which you can then open and build.

```
Ultralight.sln
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

## Building Samples (Windows Only)

A D3D11-based Browser sample is included on Windows, open the following solution in Visual Studio to build and run it:

```
/samples/Browser/projects/win/Browser.sln
```

## Building Samples (macOS Only)

A Metal-based Browser sample is included on macOS, open the following project in XCode to build and run it:

```
/samples/Browser/projects/mac/Browser.xcodeproj
```

# Using the C++ API

## Compiler / Linker Flags

### Disabling RTTI

Ultralight is built __without Run-Time Type Information (RTTI)__, you will need to disable RTTI in your C++ flags to link successfully to the library.

Check your compiler's documentation on how to disable RTTI, on __MSVC__ the command-line flag is `/GR-`, and on __GCC/Clang__ it is `-fno-rtti`. Here's a CMake script that sets these automatically per-platform:

```
if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /GR-")
elseif (UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")
endif()
```

### Setting your Include Directories

To use Ultralight in your C++ code, simply add the following directory to your project's include directories (replace `$(ULTRLIGHT_SDK_ROOT)` with the actual path you've placed the SDK):

```
$(ULTRALIGHT_SDK_ROOT)/include/
```

If you will be using any of the framework code (additional code that helps set up platform-specific windows, GPU contexts/drivers, file systems, etc.), you should also include the following: 

```
$(ULTRALIGHT_SDK_ROOT)/deps/
```

### Linking to the Library (Windows/MSVC)

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
```

### Linking to the Library (Linux)

First, copy the shared libraries in `$(ULTRALIGHT_SDK_ROOT)/bin/linux` to your OS's standard library directory.

Then, add the following to your Makefile's `LDFLAGS`:

```
-lUltralight -lUltralightCore -lWebCore
```

### Linking to the Library (macOS)

Within XCode, select your target and go to __General &rarr; Linked Frameworks and Libraries__ and add the following:

```
Ultralight.framework
WebCore.framework
UltralightCore.dylib
```

## API Headers

Simply include `<Ultralight/Ultralight.h>` at the top of your code to import the API.

```cpp
#include <Ultralight/Ultralight.h>
```

Ultralight also exposes the full __JavaScriptCore__ API so that users can make native calls to/from the JavaScript VM. To include these headers simply add:

```cpp
#include <JavaScriptCore/JavaScriptCore.h>
```


## Platform Handlers

Most OS-specific tasks in Ultralight have been left up to the user to provide via a virtual platform interface. This keeps the codebase small and grants users greater control over the behavior of the library.

At a minimum, users are expected to provide a `Config`, `GPUDriver`, and `FontLoader` before creating the `Renderer`.

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

Reference implementations for Direct3D11, OpenGL 3.2, Metal 2, and others are provided in the Framework code (see `deps/Framework/platform/` in the SDK).

### FontLoader

### FileSystem

## Update Loop

## Managing Views

### Loading Content

### Input Events

### Handling Events

## JavaScript Interop

### JavaScriptCore API

### Obtaining a JSContext

### DOMReady Event

### Evaluating Scripts

### Calling JS Functions from C++

### Calling C++ Functions from JS

# Using the Framework Code

## Select Framework Platform

## Writing Your Application

## JSHelpers Utility Code
