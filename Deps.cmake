include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)

set(ARCHITECTURE "x64")

if (PORT MATCHES "UltralightLinux")
    set(PLATFORM "linux")
elseif (PORT MATCHES "UltralightMac")
    set(PLATFORM "mac")
elseif (PORT MATCHES "UltralightWin")
    if (CMAKE_SYSTEM_NAME MATCHES "WindowsStore")
        set(PLATFORM "win-uwp")
    else ()
        set(PLATFORM "win")
    endif ()
endif ()

set(ULTRALIGHTCORE_REV "96fdb30c")
set(WEBCORE_REV "88630b80")
set(ULTRALIGHT_REV "193592e1")
set(APPCORE_REV "b4326cd")

set(ULTRALIGHTCORE_DIR "${CMAKE_CURRENT_BINARY_DIR}/deps/UltralightCore/")
set(WEBCORE_DIR "${CMAKE_CURRENT_BINARY_DIR}/deps/WebCore/")
set(ULTRALIGHT_DIR "${CMAKE_CURRENT_BINARY_DIR}/deps/Ultralight/")
set(APPCORE_DIR "${CMAKE_CURRENT_BINARY_DIR}/deps/AppCore/")

set(ULTRALIGHTCORE_BUCKET "ultralightcore-bin")
set(WEBCORE_BUCKET "webcore-bin")
set(ULTRALIGHT_BUCKET "ultralight-bin")
set(APPCORE_BUCKET "appcore-bin")

if(${GET_DBG_DEPS})
    set(WEBCORE_BUCKET "webcore-bin-dbg")
    set(APPCORE_BUCKET "appcore-bin-dbg")
endif ()

set(S3_DOMAIN ".sfo2.cdn.digitaloceanspaces.com")

if(${USE_LOCAL_DEPS})
    add_custom_target(UltralightCoreBin)
    add_custom_target(WebCoreBin)
    add_custom_target(UltralightBin)
    add_custom_target(AppCoreBin)
else()
    ExternalProject_Add(UltralightCoreBin
      URL https://${ULTRALIGHTCORE_BUCKET}${S3_DOMAIN}/ultralightcore-bin-${ULTRALIGHTCORE_REV}-${PLATFORM}-${ARCHITECTURE}.7z
      SOURCE_DIR "${ULTRALIGHTCORE_DIR}"
      BUILD_IN_SOURCE 1
      CONFIGURE_COMMAND ""
      BUILD_COMMAND ""
      INSTALL_COMMAND ""
    )

    ExternalProject_Add(WebCoreBin
      URL https://${WEBCORE_BUCKET}${S3_DOMAIN}/webcore-bin-${WEBCORE_REV}-${PLATFORM}-${ARCHITECTURE}.7z
      SOURCE_DIR "${WEBCORE_DIR}"
      BUILD_IN_SOURCE 1
      CONFIGURE_COMMAND ""
      BUILD_COMMAND ""
      INSTALL_COMMAND ""
    )

    ExternalProject_Add(UltralightBin
      URL https://${ULTRALIGHT_BUCKET}${S3_DOMAIN}/ultralight-bin-${ULTRALIGHT_REV}-${PLATFORM}-${ARCHITECTURE}.7z
      SOURCE_DIR "${ULTRALIGHT_DIR}"
      BUILD_IN_SOURCE 1
      CONFIGURE_COMMAND ""
      BUILD_COMMAND ""
      INSTALL_COMMAND ""
    )

    if (PLATFORM MATCHES "win-uwp")
      add_custom_target(AppCoreBin)
    else ()
      ExternalProject_Add(AppCoreBin
        URL https://${APPCORE_BUCKET}${S3_DOMAIN}/appcore-bin-${APPCORE_REV}-${PLATFORM}-${ARCHITECTURE}.7z
        SOURCE_DIR "${APPCORE_DIR}"
        BUILD_IN_SOURCE 1
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
        INSTALL_COMMAND ""
      )
    endif ()
endif()
