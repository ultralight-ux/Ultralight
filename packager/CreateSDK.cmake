INSTALL(DIRECTORY "${ULTRALIGHTCORE_DIR}/bin/" DESTINATION "bin" OPTIONAL)
INSTALL(DIRECTORY "${ULTRALIGHTCORE_DIR}/lib/" DESTINATION "lib" OPTIONAL)
INSTALL(DIRECTORY "${ULTRALIGHTCORE_DIR}/include/" DESTINATION "include" OPTIONAL PATTERN "Ultralight/private" EXCLUDE)
INSTALL(DIRECTORY "${WEBCORE_DIR}/bin/" DESTINATION "bin" OPTIONAL)
INSTALL(DIRECTORY "${WEBCORE_DIR}/lib/" DESTINATION "lib" OPTIONAL)
INSTALL(DIRECTORY "${WEBCORE_DIR}/include/" DESTINATION "include" OPTIONAL)
INSTALL(DIRECTORY "${WEBCORE_DIR}/inspector/" DESTINATION "inspector" OPTIONAL)
INSTALL(DIRECTORY "${ULTRALIGHT_DIR}/bin/" DESTINATION "bin" OPTIONAL)
INSTALL(DIRECTORY "${ULTRALIGHT_DIR}/lib/" DESTINATION "lib" OPTIONAL)
INSTALL(DIRECTORY "${ULTRALIGHT_DIR}/include/" DESTINATION "include" OPTIONAL)
INSTALL(DIRECTORY "${CMAKE_SOURCE_DIR}/../license/" DESTINATION "license" OPTIONAL)
INSTALL(FILES "${CMAKE_SOURCE_DIR}/../README.md" DESTINATION ".")

if (NOT PLATFORM MATCHES "win-uwp")
    INSTALL(DIRECTORY "${APPCORE_DIR}/bin/" DESTINATION "bin" OPTIONAL)
    INSTALL(DIRECTORY "${APPCORE_DIR}/lib/" DESTINATION "lib" OPTIONAL)
    INSTALL(DIRECTORY "${APPCORE_DIR}/include/" DESTINATION "include" OPTIONAL)
    INSTALL(DIRECTORY "${CMAKE_SOURCE_DIR}/../samples/" DESTINATION "samples" OPTIONAL)
    INSTALL(FILES "${CMAKE_SOURCE_DIR}/../CMakeLists.txt" DESTINATION ".")
endif ()

set(INSTALL_DIR ${PROJECT_BINARY_DIR}/out)
    
add_custom_target(create_sdk ALL "${CMAKE_COMMAND}" 
    -D CMAKE_INSTALL_PREFIX:string=${INSTALL_DIR}
    -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_install.cmake" 
    DEPENDS GetDeps) 
    
# Get name of current branch
execute_process(
  COMMAND git rev-parse --abbrev-ref HEAD
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_BRANCH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Get status of current branch
execute_process(
  COMMAND git status --untracked-files=no --porcelain
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_STATUS
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Get abbreviated commit hash of current branch
execute_process(
  COMMAND git rev-parse --short HEAD
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_COMMIT_HASH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Get list of local, unpushed commits
execute_process(
  COMMAND git cherry -v
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_CHERRY
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Get formatted log
execute_process(
  COMMAND git log --pretty=oneline --abbrev-commit
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_LOG
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

file(WRITE "${INSTALL_DIR}/LOG.txt" ${GIT_LOG})

if (PORT MATCHES "UltralightMac")
    if(${GET_DBG_DEPS})
    else ()
        # Strip shared libraries in release build
        add_custom_command(TARGET create_sdk POST_BUILD
            COMMAND strip -S -x ${INSTALL_DIR}/bin/*.dylib
        )
    endif ()
endif ()

set(PKG_FILENAME "ultralight-sdk-${GIT_COMMIT_HASH}-${PLATFORM}-${ARCHITECTURE}.7z")
set(LATEST_PKG_FILENAME "ultralight-sdk-latest-${PLATFORM}-${ARCHITECTURE}.7z")

#if (NOT GIT_STATUS STREQUAL "")
    add_custom_command(TARGET create_sdk POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "NOTE: No release archive created, working directory not clean."
    )
#elseif (NOT GIT_CHERRY STREQUAL "")
    add_custom_command(TARGET create_sdk POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "NOTE: No release archive created, branch needs to be pushed to remote repository."
    )
#else ()
    add_custom_command(TARGET create_sdk POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E tar "cf" ${PROJECT_BINARY_DIR}/${PKG_FILENAME} --format=7zip -- .
        WORKING_DIRECTORY ${INSTALL_DIR}
    )
    add_custom_command(TARGET create_sdk POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "Created release archive: ${PROJECT_BINARY_DIR}/${PKG_FILENAME}"
    )
    add_custom_command(TARGET create_sdk POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_BINARY_DIR}/${PKG_FILENAME} ${PROJECT_BINARY_DIR}/latest/${LATEST_PKG_FILENAME}
    )
#endif ()