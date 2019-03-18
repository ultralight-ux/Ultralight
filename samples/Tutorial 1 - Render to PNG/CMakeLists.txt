set(APP_NAME Tutorial1)
include_directories("${ULTRALIGHT_INCLUDE_DIRS}")
link_directories("${ULTRALIGHT_LIBRARY_DIRS}")
link_libraries(UltralightCore Ultralight WebCore)

add_executable(${APP_NAME} "main.cpp")

# Copy all binaries to target directory
add_custom_command(TARGET ${APP_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${ULTRALIGHT_BINARY_DIRS}" $<TARGET_FILE_DIR:${APP_NAME}>)  

