project(Ultralight C CXX)

cmake_minimum_required(VERSION 2.8.12)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(ULTRALIGHT_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/include")
set(FRAMEWORK_SOURCE_DIR "${PROJECT_SOURCE_DIR}/deps/Framework")
set(FRAMEWORK_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/deps")
set(GLFW_SOURCE_DIR "${PROJECT_SOURCE_DIR}/deps/glfw")

# Disable various GLFW options
option(GLFW_BUILD_EXAMPLES "Build the GLFW example programs" OFF)
option(GLFW_BUILD_TESTS "Build the GLFW test programs" OFF)
option(GLFW_BUILD_DOCS "Build the GLFW documentation" OFF)
option(GLFW_INSTALL "Generate installation target" OFF)

set(GLAD_SRC "${GLFW_SOURCE_DIR}/deps/glad/glad.h"
             "${GLFW_SOURCE_DIR}/deps/glad.c")
		 
set(FRAMEWORK_SRC "${FRAMEWORK_SOURCE_DIR}/Application.h"
                  "${FRAMEWORK_SOURCE_DIR}/GPUContext.h"
                  "${FRAMEWORK_SOURCE_DIR}/JSHelpers.h"
                  "${FRAMEWORK_SOURCE_DIR}/JSHelpers.cpp"
                  "${FRAMEWORK_SOURCE_DIR}/Overlay.h"
                  "${FRAMEWORK_SOURCE_DIR}/Overlay.cpp"
                  "${FRAMEWORK_SOURCE_DIR}/Platform.h"
                  "${FRAMEWORK_SOURCE_DIR}/Window.h"
                  "${FRAMEWORK_SOURCE_DIR}/platform/glfw/main.cpp"
                  "${FRAMEWORK_SOURCE_DIR}/platform/glfw/PlatformGLFW.h"
                  "${FRAMEWORK_SOURCE_DIR}/platform/glfw/WindowGLFW.cpp"
                  "${FRAMEWORK_SOURCE_DIR}/platform/glfw/gl/GPUContextGL.cpp"
                  "${FRAMEWORK_SOURCE_DIR}/platform/glfw/gl/GPUDriverGL.h"
                  "${FRAMEWORK_SOURCE_DIR}/platform/glfw/gl/GPUDriverGL.cpp")
                  
set(FRAMEWORK_SHADER_SRC "${FRAMEWORK_SOURCE_DIR}/shaders/glsl/ps/fill.frag"
                         "${FRAMEWORK_SOURCE_DIR}/shaders/glsl/vs/v2f_c4f_t2f_t2f_d28f.vert")
				  
if (WIN32)
    list(APPEND FRAMEWORK_SRC "${FRAMEWORK_SOURCE_DIR}/platform/common/FileSystemBasic.h"
                              "${FRAMEWORK_SOURCE_DIR}/platform/common/FileSystemBasic.cpp"
                              "${FRAMEWORK_SOURCE_DIR}/platform/win/FontLoaderWin.h"
                              "${FRAMEWORK_SOURCE_DIR}/platform/win/FontLoaderWin.cpp")
                              
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(ULTRALIGHT_LIBRARY_DIRS "${PROJECT_SOURCE_DIR}/lib/win/x64")
        set(ULTRALIGHT_BINARY_DIRS "${PROJECT_SOURCE_DIR}/bin/win/x64")
    elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
        set(ULTRALIGHT_LIBRARY_DIRS "${PROJECT_SOURCE_DIR}/lib/win/x86")
        set(ULTRALIGHT_BINARY_DIRS "${PROJECT_SOURCE_DIR}/bin/win/x86")
    endif()
    
elseif (APPLE)
    list(APPEND FRAMEWORK_SRC "${FRAMEWORK_SOURCE_DIR}/platform/mac/FileSystemMac.h"
                              "${FRAMEWORK_SOURCE_DIR}/platform/mac/FileSystemMac.mm"
                              "${FRAMEWORK_SOURCE_DIR}/platform/mac/FontLoaderMac.h"
                              "${FRAMEWORK_SOURCE_DIR}/platform/mac/FontLoaderMac.mm")
                              
    set(ULTRALIGHT_LIBRARY_DIRS "${PROJECT_SOURCE_DIR}/bin/mac")
    set(ULTRALIGHT_BINARY_DIRS "${PROJECT_SOURCE_DIR}/bin/mac")
    
elseif (UNIX)
    list(APPEND FRAMEWORK_SRC "${FRAMEWORK_SOURCE_DIR}/platform/common/FileSystemBasic.h"
                              "${FRAMEWORK_SOURCE_DIR}/platform/common/FileSystemBasic.cpp"
                              "${FRAMEWORK_SOURCE_DIR}/platform/common/FontLoaderRoboto.h"
                              "${FRAMEWORK_SOURCE_DIR}/platform/common/FontLoaderRoboto.cpp")
                              
    set(ULTRALIGHT_LIBRARY_DIRS "${PROJECT_SOURCE_DIR}/bin/linux")
    set(ULTRALIGHT_BINARY_DIRS "${PROJECT_SOURCE_DIR}/bin/linux")
    
endif ()

add_subdirectory(deps)
add_subdirectory(samples)
