set(APP_NAME Browser)
include_directories("${ULTRALIGHT_INCLUDE_DIR}")
link_directories("${ULTRALIGHT_LIBRARY_DIR}")
link_libraries(UltralightCore AppCore Ultralight WebCore)

set(SOURCES "src/Browser.h"
            "src/Browser.cpp"
            "src/Tab.h"
            "src/Tab.cpp"
            "src/UI.h"
            "src/UI.cpp"
            "src/main.cpp")

add_executable(${APP_NAME} WIN32 MACOSX_BUNDLE ${SOURCES})

if (APPLE)
  # Enable High-DPI on macOS through our custom Info.plist template
  set_target_properties(${APP_NAME} PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.in) 
endif()

if (MSVC)
  # Tell MSVC to use main instead of WinMain for Windows subsystem executables
  set_target_properties(${APP_NAME} PROPERTIES LINK_FLAGS "/ENTRY:mainCRTStartup")
endif()

# Copy all binaries to target directory
add_custom_command(TARGET ${APP_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory "${ULTRALIGHT_BINARY_DIR}" $<TARGET_FILE_DIR:${APP_NAME}>) 

if (APPLE)
  set(ASSETS_PATH "$<TARGET_FILE_DIR:${APP_NAME}>/../Resources/assets") 
else ()
  set(ASSETS_PATH "$<TARGET_FILE_DIR:${APP_NAME}>/assets") 
endif ()

# Copy assets to assets directory
add_custom_command(TARGET ${APP_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/assets/" "${ASSETS_PATH}")
    
# Copy inspector to assets directory
add_custom_command(TARGET ${APP_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory "${ULTRALIGHT_INSPECTOR_DIR}" "${ASSETS_PATH}/inspector")

if (APPLE)
  # Copy resources to resources directory
  add_custom_command(TARGET ${APP_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${WEBCORE_DIR}/bin/resources/" "$<TARGET_FILE_DIR:${APP_NAME}>/../Resources/resources")
endif ()


