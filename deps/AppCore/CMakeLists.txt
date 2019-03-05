cmake_minimum_required(VERSION 2.8.12)
project(AppCore)
set_property(DIRECTORY . PROPERTY FOLDER "AppCore")

set(SDK_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../..")

include_directories("${SDK_ROOT}/include")

set(AppCore_SOURCES
    "${SDK_ROOT}/include/AppCore/App.h"
    "${SDK_ROOT}/include/AppCore/Defines.h"
    "${SDK_ROOT}/include/AppCore/JSHelpers.h"
    "${SDK_ROOT}/include/AppCore/Monitor.h"
    "${SDK_ROOT}/include/AppCore/Overlay.h"
    "${SDK_ROOT}/include/AppCore/Window.h"
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
        link_directories("${SDK_ROOT}/lib/win/x64")
    elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
        link_directories("${SDK_ROOT}/lib/win/x86")
    endif()
elseif (APPLE)
    link_directories("${SDK_ROOT}/lib/mac")
elseif (UNIX)
    link_directories("${SDK_ROOT}/lib/linux")
endif()


add_library(AppCore SHARED ${AppCore_SOURCES})
link_directories("${SDK_ROOT}/lib")
target_link_libraries (AppCore Ultralight UltralightCore WebCore)

if (WIN32)
    target_link_libraries (AppCore Shcore.lib Shlwapi.lib Usp10.lib d3d11.lib d3dcompiler.lib)
elseif (APPLE)
    set_target_properties(AppCore PROPERTIES LINK_FLAGS "-framework Ultralight -Wl,-F${SDK_ROOT}/lib")
endif ()