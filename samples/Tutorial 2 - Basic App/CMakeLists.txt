link_directories("${ULTRALIGHT_LIBRARY_DIRS}")

if (NOT APPLE)
    link_libraries(UltralightCore AppCore Ultralight WebCore)
else ()
    # On macOS the last two libraries are Frameworks so we must pass special flags to the linker
    link_libraries(UltralightCore AppCore "-framework Ultralight -framework WebCore -Wl,-F${ULTRALIGHT_LIBRARY_DIRS}")
endif()

include_directories("${ULTRALIGHT_INCLUDE_DIRS}")

if (MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /GR-")
elseif (UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")
endif()

add_executable(Tutorial2 WIN32 "main.cpp")

# Copy all binaries to target directory
add_custom_command(TARGET Tutorial2 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${ULTRALIGHT_BINARY_DIRS}" $<TARGET_FILE_DIR:Tutorial2>)  

