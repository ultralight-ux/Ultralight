cmake_minimum_required(VERSION 2.8.12)
project(AppCore)
set_property(DIRECTORY . PROPERTY FOLDER "AppCore")
set (CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

get_filename_component(SDK_ROOT "../../"
                       REALPATH)

include_directories("${SDK_ROOT}/include")

set(AppCore_SOURCES
    "${SDK_ROOT}/include/AppCore/App.h"
    "${SDK_ROOT}/include/AppCore/Defines.h"
    "${SDK_ROOT}/include/AppCore/JSHelpers.h"
    "${SDK_ROOT}/include/AppCore/Monitor.h"
    "${SDK_ROOT}/include/AppCore/Overlay.h"
    "${SDK_ROOT}/include/AppCore/Window.h"
    "${SDK_ROOT}/include/AppCore/CAPI.h"
    "src/CAPI.cpp"
    "src/JSHelpers.cpp"
    "src/Overlay.cpp"
    "src/OverlayManager.h"
    "src/OverlayManager.cpp"
    "src/RefCountedImpl.h"
    )
    
if (WIN32)
    list(APPEND AppCore_SOURCES
        "src/win/AppWin.h"
        "src/win/AppWin.cpp"
        "src/win/FileSystemWin.h"
        "src/win/FileSystemWin.cpp"
        "src/win/FontLoaderWin.h"
        "src/win/FontLoaderWin.cpp"
        "src/win/MonitorWin.h"
        "src/win/MonitorWin.cpp"
        "src/win/WindowWin.h"
        "src/win/WindowWin.cpp"
        "src/win/d3d11/GPUDriverD3D11.h"
        "src/win/d3d11/GPUDriverD3D11.cpp"
        "src/win/d3d11/GPUContextD3D11.h"
        "src/win/d3d11/GPUContextD3D11.cpp"
    )
    
    add_definitions(-DAPPCORE_IMPLEMENTATION -D_UNICODE -DNOMINMAX)
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(LIB_DIR "${SDK_ROOT}/lib/win/x64")
    elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
        set(LIB_DIR "${SDK_ROOT}/lib/win/x86")
    endif()
    set(AppCore_LIBRARIES "${LIB_DIR}/Ultralight.lib" "${LIB_DIR}/UltralightCore.lib" "${LIB_DIR}/WebCore.lib" 
      "Shcore.lib" "Shlwapi.lib" "Usp10.lib" "d3d11.lib" "d3dcompiler.lib")
elseif (APPLE)
    list(APPEND AppCore_SOURCES
        "src/mac/AppMac.h"
        "src/mac/AppMac.mm"
        "src/mac/MonitorMac.h"
        "src/mac/MonitorMac.mm"
        "src/mac/WindowMac.h"
        "src/mac/WindowMac.mm"
        "src/mac/AppDelegate.h"
        "src/mac/AppDelegate.m"
        "src/mac/ViewController.h"
        "src/mac/ViewController.mm"
        "src/mac/ViewDelegate.h"
        "src/mac/ViewDelegate.mm"
        "src/mac/WindowDelegate.h"
        "src/mac/WindowDelegate.mm"
        "src/mac/FileSystemMac.h"
        "src/mac/FileSystemMac.mm"
        "src/mac/metal/GPUContextMetal.h"
        "src/mac/metal/GPUContextMetal.mm"
        "src/mac/metal/GPUDriverMetal.h"
        "src/mac/metal/GPUDriverMetal.mm"
#        "shaders/metal/Shaders.metal"
    )
    set(LIB_DIR "${SDK_ROOT}/bin/mac")
    set(AppCore_LIBRARIES "${LIB_DIR}/libUltralight.dylib" "${LIB_DIR}/libUltralightCore.dylib" "${LIB_DIR}/libWebCore.dylib")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
    set_source_files_properties("shaders/metal/Shaders.metal" PROPERTIES LANGUAGE METAL)
elseif (UNIX)
    set(GLFW_SOURCE_DIR "${SDK_ROOT}/deps/AppCore/deps/glfw")

    # Disable various GLFW options
    option(GLFW_BUILD_EXAMPLES "Build the GLFW example programs" OFF)
    option(GLFW_BUILD_TESTS "Build the GLFW test programs" OFF)
    option(GLFW_BUILD_DOCS "Build the GLFW documentation" OFF)
    option(GLFW_INSTALL "Generate installation target" OFF)
    
    list(APPEND AppCore_SOURCES
        "src/linux/AppGLFW.h"
        "src/linux/AppGLFW.cpp"
        "src/linux/MonitorGLFW.h"
        "src/linux/MonitorGLFW.cpp"
        "src/linux/WindowGLFW.h"
        "src/linux/WindowGLFW.cpp"
        "src/linux/FileSystemBasic.h"
        "src/linux/FileSystemBasic.cpp"
        "src/linux/gl/GPUContextGL.h"
        "src/linux/gl/GPUContextGL.cpp"
        "src/linux/gl/GPUDriverGL.h"
        "src/linux/gl/GPUDriverGL.cpp"
        "${GLFW_SOURCE_DIR}/deps/glad/glad.h"
        "${GLFW_SOURCE_DIR}/deps/glad.c"
    )
                 
    add_subdirectory("deps/glfw")
    include_directories("${GLFW_SOURCE_DIR}/include")
    include_directories("${GLFW_SOURCE_DIR}/deps")
    
    set(LIB_DIR "${SDK_ROOT}/bin/linux")
    
    link_libraries(glfw)
    set(AppCore_LIBRARIES "${LIB_DIR}/libUltralight.so" "${LIB_DIR}/libUltralightCore.so" "${LIB_DIR}/libWebCore.so")
endif()


message("SDK_ROOT = ${SDK_ROOT}")
message("LIB_DIR = ${LIB_DIR}")

add_library(AppCore SHARED ${AppCore_SOURCES})
target_link_libraries (AppCore ${AppCore_LIBRARIES})

if (APPLE)
    set_target_properties(AppCore PROPERTIES LINK_FLAGS "-framework Cocoa -framework Metal -framework Foundation -framework CoreFoundation -framework MetalKit")
    set_property (TARGET AppCore APPEND_STRING PROPERTY COMPILE_FLAGS "-fobjc-arc")
endif ()
