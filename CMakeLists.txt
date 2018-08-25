project(Ultralight C CXX)

cmake_minimum_required(VERSION 2.8.12)

set(ULTRALIGHT_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/include")
set(FRAMEWORK_SOURCE_DIR "${PROJECT_SOURCE_DIR}/deps/Framework")
set(FRAMEWORK_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/deps")

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
				  "${FRAMEWORK_SOURCE_DIR}/platform/glfw/MainLoopGLFW.h"
				  "${FRAMEWORK_SOURCE_DIR}/platform/glfw/MainLoopGLFW.cpp"
				  "${FRAMEWORK_SOURCE_DIR}/platform/glfw/PlatformGLFW.h"
				  "${FRAMEWORK_SOURCE_DIR}/platform/glfw/WindowGLFW.cpp")
				  
if (WIN32)
    list(APPEND FRAMEWORK_SRC "${FRAMEWORK_SOURCE_DIR}/platform/win/FileSystemWin.h"
                              "${FRAMEWORK_SOURCE_DIR}/platform/win/FileSystemWin.cpp"
                              "${FRAMEWORK_SOURCE_DIR}/platform/win/FontLoaderWin.h"
                              "${FRAMEWORK_SOURCE_DIR}/platform/win/FontLoaderWin.cpp")
elseif (APPLE)
    list(APPEND FRAMEWORK_SRC "${FRAMEWORK_SOURCE_DIR}/platform/mac/FileSystemMac.h"
	                          "${FRAMEWORK_SOURCE_DIR}/platform/mac/FileSystemMac.mm"
							  "${FRAMEWORK_SOURCE_DIR}/platform/win/FontLoaderMac.h"
	                          "${FRAMEWORK_SOURCE_DIR}/platform/win/FontLoaderMac.mm")
elseif (UNIX)
    list(APPEND FRAMEWORK_SRC "${FRAMEWORK_SOURCE_DIR}/platform/linux/FileSystemLinux.h"
                              "${FRAMEWORK_SOURCE_DIR}/platform/linux/FileSystemLinux.cpp"
                              "${FRAMEWORK_SOURCE_DIR}/platform/linux/FontLoaderLinux.h"
	                          "${FRAMEWORK_SOURCE_DIR}/platform/linux/FontLoaderLinux.cpp")
endif ()

add_subdirectory(deps)
add_subdirectory(samples)