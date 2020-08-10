set(APP_NAME Sample1)
include_directories("${ULTRALIGHT_INCLUDE_DIR}")
link_directories("${ULTRALIGHT_LIBRARY_DIR}")
link_libraries(UltralightCore Ultralight WebCore AppCore)

if (PORT MATCHES "UltralightMac")
    SET(CMAKE_INSTALL_RPATH ".")
endif ()

add_executable(${APP_NAME} "main.cpp")

# Copy all binaries to target directory
add_custom_command(TARGET ${APP_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${ULTRALIGHT_BINARY_DIR}" $<TARGET_FILE_DIR:${APP_NAME}>)  

