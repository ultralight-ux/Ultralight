
link_directories("${ULTRALIGHT_LIBRARY_DIRS}")
link_libraries(glfw Ultralight UltralightCore WebCore)

include_directories("${ULTRALIGHT_INCLUDE_DIRS}")
include_directories(${glfw_INCLUDE_DIRS})
include_directories("${GLFW_SOURCE_DIR}/deps")
include_directories("${FRAMEWORK_INCLUDE_DIRS}")

if (WIN32)
    link_libraries(Shlwapi)
endif()

if (BUILD_SHARED_LIBS)
    link_libraries("${MATH_LIBRARY}")
endif()

add_definitions(-DFRAMEWORK_PLATFORM_GLFW)

if (MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS -DNOMINMAX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /GR-")
endif()

set(BROWSER_SRC "src/Browser.h"
                "src/Browser.cc"
				"src/Tab.h"
				"src/Tab.cc"
				"src/UI.h"
				"src/UI.cc")
				
source_group(GLAD FILES ${GLAD_SRC})
source_group(Framework FILES ${FRAMEWORK_SRC})
		 
#add_executable(Browser WIN32 MACOSX_BUNDLE ${BROWSER_SRC} ${FRAMEWORK_SRC} ${GLAD_SRC})
add_executable(Browser ${BROWSER_SRC} ${FRAMEWORK_SRC} ${GLAD_SRC})


add_custom_command(TARGET Browser POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${ULTRALIGHT_BINARY_DIRS}" $<TARGET_FILE_DIR:Browser>)  

if (MSVC)
    add_custom_command(TARGET Browser POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/assets/" "$<TARGET_FILE_DIR:Browser>/assets")  
    
    # Tell MSVC to use main instead of WinMain for Windows subsystem executables
    set_target_properties(Browser PROPERTIES
                          LINK_FLAGS "/ENTRY:mainCRTStartup")
endif()

if (APPLE)
    # TODO Bundle stuff
endif ()

