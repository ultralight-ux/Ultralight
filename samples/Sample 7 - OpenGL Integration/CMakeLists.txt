set(APP_NAME Sample7)
include_directories("${ULTRALIGHT_INCLUDE_DIR}")
link_directories("${ULTRALIGHT_LIBRARY_DIR}")
link_libraries(UltralightCore Ultralight WebCore AppCore)

# Disable various GLFW options
option(GLFW_BUILD_EXAMPLES "Build the GLFW example programs" OFF)
option(GLFW_BUILD_TESTS "Build the GLFW test programs" OFF)
option(GLFW_BUILD_DOCS "Build the GLFW documentation" OFF)
option(GLFW_INSTALL "Generate installation target" OFF)

set(GLFW_DIR "glfw")
add_subdirectory(${GLFW_DIR})
include_directories("${GLFW_DIR}/include")
include_directories("${GLFW_DIR}/deps")

link_libraries(glfw)

set(GLAD_SOURCES "${GLFW_DIR}/deps/glad/glad.h"
                 "${GLFW_DIR}/deps/glad.c")

set(SOURCES "src/Sample.h"
            "src/Sample.cpp"
            "src/WebTile.h"
            "src/WebTile.cpp"
            "src/Window.h"
            "src/Window.cpp"
            "src/GLTextureSurface.h"
            "src/GLTextureSurface.cpp"
            "src/main.cpp")

add_executable(${APP_NAME} MACOSX_BUNDLE ${SOURCES} ${GLAD_SOURCES})

if (APPLE)
  # Enable High-DPI on macOS through our custom Info.plist template
  set_target_properties(${APP_NAME} PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.in) 
endif()

# Copy all binaries to target directory
add_custom_command(TARGET ${APP_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${ULTRALIGHT_BINARY_DIR}" $<TARGET_FILE_DIR:${APP_NAME}>)  

if (APPLE)
  set(ASSETS_PATH "$<TARGET_FILE_DIR:${APP_NAME}>/../Resources/assets") 
else ()
  set(ASSETS_PATH "$<TARGET_FILE_DIR:${APP_NAME}>/assets") 
endif ()
  
# Copy assets to assets path
add_custom_command(TARGET ${APP_NAME} POST_BUILD
COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/assets/" "${ASSETS_PATH}")
