project(Samples C CXX)

cmake_minimum_required(VERSION 2.8.12)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (EXISTS "${CMAKE_CURRENT_LIST_DIR}/../Deps.cmake")
    set(IN_SOURCE_BUILD 1)
    set(SDK_ROOT "${CMAKE_BINARY_DIR}/SDK/")
    if (UNIX)
        if (APPLE)
            set(PORT UltralightMac)
        else ()
            set(PORT UltralightLinux)
        endif ()
    elseif (CMAKE_SYSTEM_NAME MATCHES "Windows")
        set(PORT UltralightWin)
    else ()
        message(FATAL_ERROR "Unknown OS '${CMAKE_SYSTEM_NAME}'")
    endif ()
    include(${CMAKE_CURRENT_LIST_DIR}/../Deps.cmake)
else ()
    set(IN_SOURCE_BUILD 0)
    set(SDK_ROOT "${CMAKE_CURRENT_LIST_DIR}/../")
endif ()

set(ULTRALIGHT_INCLUDE_DIR "${SDK_ROOT}/include")
set(ULTRALIGHT_BINARY_DIR "${SDK_ROOT}/bin")
set(ULTRALIGHT_INSPECTOR_DIR "${SDK_ROOT}/inspector")

if (CMAKE_SYSTEM_NAME MATCHES "Windows")
    set(ULTRALIGHT_LIBRARY_DIR "${SDK_ROOT}/lib")
else ()
    set(ULTRALIGHT_LIBRARY_DIR "${SDK_ROOT}/bin")
endif ()

add_subdirectory("Browser")
add_subdirectory("Tutorial 1 - Render to PNG")
add_subdirectory("Tutorial 2 - Basic App")
add_subdirectory("Tutorial 3 - Resizable App")
add_subdirectory("Tutorial 4 - JavaScript")
add_subdirectory("Tutorial 5 - File Loading")
add_subdirectory("Tutorial 6 - Intro to C API")

if (${IN_SOURCE_BUILD})
    add_custom_target(
        CopySDK ALL
        COMMAND ${CMAKE_COMMAND} -E make_directory "${SDK_ROOT}"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${SDK_ROOT}/bin"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${SDK_ROOT}/include"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${SDK_ROOT}/inspector"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${ULTRALIGHTCORE_DIR}/bin/" "${SDK_ROOT}/bin"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${ULTRALIGHTCORE_DIR}/include/" "${SDK_ROOT}/include"
        COMMAND ${CMAKE_COMMAND} -E remove_directory "${SDK_ROOT}/include/Ultralight/private"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${WEBCORE_DIR}/bin/" "${SDK_ROOT}/bin"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${WEBCORE_DIR}/include/" "${SDK_ROOT}/include"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${WEBCORE_DIR}/inspector/" "${SDK_ROOT}/inspector"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${ULTRALIGHT_DIR}/bin/" "${SDK_ROOT}/bin"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${ULTRALIGHT_DIR}/include/" "${SDK_ROOT}/include"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${APPCORE_DIR}/bin/" "${SDK_ROOT}/bin"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${APPCORE_DIR}/include/" "${SDK_ROOT}/include"
        DEPENDS UltralightCoreBin WebCoreBin UltralightBin AppCoreBin
    )

    if (PORT MATCHES "UltralightWin")
        add_custom_command(TARGET CopySDK POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory "${SDK_ROOT}/lib"
            COMMAND ${CMAKE_COMMAND} -E copy_directory "${ULTRALIGHTCORE_DIR}/lib/" "${SDK_ROOT}/lib"
            COMMAND ${CMAKE_COMMAND} -E copy_directory "${WEBCORE_DIR}/lib/" "${SDK_ROOT}/lib"
            COMMAND ${CMAKE_COMMAND} -E copy_directory "${ULTRALIGHT_DIR}/lib/" "${SDK_ROOT}/lib"
            COMMAND ${CMAKE_COMMAND} -E copy_directory "${APPCORE_DIR}/lib/" "${SDK_ROOT}/lib"
        )
    endif ()

    add_dependencies(Browser CopySDK)
    add_dependencies(Tutorial1 CopySDK)
    add_dependencies(Tutorial2 CopySDK)
    add_dependencies(Tutorial3 CopySDK)
    add_dependencies(Tutorial4 CopySDK)
    add_dependencies(Tutorial5 CopySDK)
    add_dependencies(Tutorial6 CopySDK)
endif ()