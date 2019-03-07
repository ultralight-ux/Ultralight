set(APP_NAME Tutorial2)
include_directories("${ULTRALIGHT_INCLUDE_DIRS}")
link_directories("${ULTRALIGHT_LIBRARY_DIRS}")
link_libraries(UltralightCore AppCore Ultralight WebCore)

add_executable(${APP_NAME} WIN32 MACOSX_BUNDLE "main.cpp")

if (APPLE)
  # Enable High-DPI on macOS through our custom Info.plist template
  set_target_properties(${APP_NAME} PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/../Info.plist.in) 
endif()

# Copy all binaries to target directory
add_custom_command(TARGET ${APP_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${ULTRALIGHT_BINARY_DIRS}" $<TARGET_FILE_DIR:${APP_NAME}>)  

