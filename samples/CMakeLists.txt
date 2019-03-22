project(Samples C CXX)

cmake_minimum_required(VERSION 2.8.12)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

get_filename_component(SDK_ROOT "../"
                       REALPATH)

set(ULTRALIGHT_INCLUDE_DIRS "${SDK_ROOT}/include")
				  
if (WIN32)                        
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(ULTRALIGHT_LIBRARY_DIRS "${SDK_ROOT}/lib/win/x64")
        set(ULTRALIGHT_BINARY_DIRS "${SDK_ROOT}/bin/win/x64")
    elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
        set(ULTRALIGHT_LIBRARY_DIRS "${SDK_ROOT}/lib/win/x86")
        set(ULTRALIGHT_BINARY_DIRS "${SDK_ROOT}/bin/win/x86")
    endif()
elseif (APPLE)
    set(ULTRALIGHT_LIBRARY_DIRS "${SDK_ROOT}/bin/mac")
    set(ULTRALIGHT_BINARY_DIRS "${SDK_ROOT}/bin/mac")  
elseif (UNIX)      
    set(ULTRALIGHT_LIBRARY_DIRS "${SDK_ROOT}/bin/linux")
    set(ULTRALIGHT_BINARY_DIRS "${SDK_ROOT}/bin/linux")
endif ()

add_subdirectory("Browser")
add_subdirectory("Tutorial 1 - Render to PNG")
add_subdirectory("Tutorial 2 - Basic App")
add_subdirectory("Tutorial 3 - Resizable App")
add_subdirectory("Tutorial 4 - JavaScript")