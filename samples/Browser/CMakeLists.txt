link_directories("${ULTRALIGHT_LIBRARY_DIRS}")

if (NOT APPLE)
    link_libraries(glfw UltralightCore Ultralight WebCore)
else ()
    # On macOS the last two libraries are Frameworks so we must pass special flags to the linker
    link_libraries(glfw UltralightCore "-framework Ultralight -framework WebCore -Wl,-F${ULTRALIGHT_LIBRARY_DIRS}")
endif()

include_directories("${ULTRALIGHT_INCLUDE_DIRS}")
include_directories(${glfw_INCLUDE_DIRS})
include_directories("${GLFW_SOURCE_DIR}/deps")
include_directories("${FRAMEWORK_INCLUDE_DIRS}")

add_definitions(-DFRAMEWORK_PLATFORM_GLFW)

if (MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /GR-")
elseif (UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")
endif()

set(BROWSER_SRC "src/Browser.h"
                "src/Browser.cc"
                "src/Tab.h"
                "src/Tab.cc"
                "src/UI.h"
                "src/UI.cc")
				
source_group(GLAD FILES ${GLAD_SRC})
source_group(Framework FILES ${FRAMEWORK_SRC})
source_group(Shaders FILES ${FRAMEWORK_SHADER_SRC})
		 
add_executable(Browser WIN32 MACOSX_BUNDLE ${BROWSER_SRC} ${FRAMEWORK_SRC} ${FRAMEWORK_SHADER_SRC} ${GLAD_SRC})

# Copy all binaries to target directory
add_custom_command(TARGET Browser POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${ULTRALIGHT_BINARY_DIRS}" $<TARGET_FILE_DIR:Browser>)  

if (NOT APPLE)
    # Copy assets to target directory
    add_custom_command(TARGET Browser POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/assets/" "$<TARGET_FILE_DIR:Browser>/assets")
    # Copy GLSL shaders to target directory
    add_custom_command(TARGET Browser POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${FRAMEWORK_INCLUDE_DIRS}/Framework/shaders/glsl/" "$<TARGET_FILE_DIR:Browser>/assets/glsl/")          
    
    if (MSVC)
        # Tell MSVC to use main instead of WinMain for Windows subsystem executables
        set_target_properties(Browser PROPERTIES LINK_FLAGS "/ENTRY:mainCRTStartup")
    endif()
else()
    # Copy assets to target directory
    add_custom_command(TARGET Browser POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/assets/" "$<TARGET_FILE_DIR:Browser>/../Resources/assets")
    # Copy GLSL shaders to target directory
    add_custom_command(TARGET Browser POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${FRAMEWORK_INCLUDE_DIRS}/Framework/shaders/glsl/" "$<TARGET_FILE_DIR:Browser>/../Resources/assets/glsl/")          
endif ()

