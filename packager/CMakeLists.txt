cmake_minimum_required(VERSION 3.3.2)
project(AppCore C CXX)
set_property(DIRECTORY . PROPERTY FOLDER "UltralightSDK")

include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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

include(../Deps.cmake)

add_custom_target(GetDeps)
add_dependencies(GetDeps UltralightCoreBin WebCoreBin UltralightBin AppCoreBin)

include(CreateSDK.cmake)