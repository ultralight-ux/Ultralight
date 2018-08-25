
link_libraries(glfw)

include_directories("${ULTRALIGHT_INCLUDE_DIRS}")
include_directories(${glfw_INCLUDE_DIRS})
include_directories("${GLFW_SOURCE_DIR}/deps")
include_directories("${FRAMEWORK_INCLUDE_DIRS}")

if (BUILD_SHARED_LIBS)
    link_libraries("${MATH_LIBRARY}")
endif()

if (MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

set(BROWSER_SRC "src/Browser.h"
                "src/Browser.cc"
				"src/Tab.h"
				"src/Tab.cc"
				"src/UI.h"
				"src/UI.cc")
				
source_group(GLAD FILES ${GLAD_SRC})
source_group(Framework FILES ${FRAMEWORK_SRC})
		 
add_executable(Browser WIN32 MACOSX_BUNDLE ${BROWSER_SRC} ${FRAMEWORK_SRC} ${GLAD_SRC})

if (MSVC)
    # Tell MSVC to use main instead of WinMain for Windows subsystem executables
    set_target_properties(${WINDOWS_BINARIES} PROPERTIES
                          LINK_FLAGS "/ENTRY:mainCRTStartup")
endif()

if (APPLE)
    # TODO Bundle stuff
endif ()

