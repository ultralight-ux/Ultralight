include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)

if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(ARCHITECTURE "x64")
else ()
    set(ARCHITECTURE "x86")
endif ()

if (PORT MATCHES "UltralightLinux")
    set(PLATFORM "linux")
elseif (PORT MATCHES "UltralightMac")
    set(PLATFORM "mac")
elseif (PORT MATCHES "UltralightWin")
    set(PLATFORM "win")
endif ()

set(ULTRALIGHTCORE_REV "06007e0")
set(WEBCORE_REV "fc579b5")
set(ULTRALIGHT_REV "30c8d06")
set(APPCORE_REV "8f3635a")

set(ULTRALIGHTCORE_DIR "${CMAKE_CURRENT_LIST_DIR}/deps/UltralightCore/")
set(WEBCORE_DIR "${CMAKE_CURRENT_LIST_DIR}/deps/WebCore/")
set(ULTRALIGHT_DIR "${CMAKE_CURRENT_LIST_DIR}/deps/Ultralight/")
set(APPCORE_DIR "${CMAKE_CURRENT_LIST_DIR}/deps/AppCore/")

ExternalProject_Add(UltralightCoreBin
  URL https://ultralightcore-bin.sfo2.cdn.digitaloceanspaces.com/ultralightcore-bin-${ULTRALIGHTCORE_REV}-${PLATFORM}-${ARCHITECTURE}.7z
  SOURCE_DIR "${ULTRALIGHTCORE_DIR}"
  BUILD_IN_SOURCE 1
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  INSTALL_COMMAND ""
)

ExternalProject_Add(WebCoreBin
  URL https://webcore-bin.sfo2.cdn.digitaloceanspaces.com/webcore-bin-${WEBCORE_REV}-${PLATFORM}-${ARCHITECTURE}.7z
  SOURCE_DIR "${WEBCORE_DIR}"
  BUILD_IN_SOURCE 1
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  INSTALL_COMMAND ""
)

ExternalProject_Add(UltralightBin
  URL https://ultralight-bin.sfo2.cdn.digitaloceanspaces.com/ultralight-bin-${ULTRALIGHT_REV}-${PLATFORM}-${ARCHITECTURE}.7z
  SOURCE_DIR "${ULTRALIGHT_DIR}"
  BUILD_IN_SOURCE 1
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  INSTALL_COMMAND ""
)

ExternalProject_Add(AppCoreBin
  URL https://appcore-bin.sfo2.cdn.digitaloceanspaces.com/appcore-bin-${APPCORE_REV}-${PLATFORM}-${ARCHITECTURE}.7z
  SOURCE_DIR "${APPCORE_DIR}"
  BUILD_IN_SOURCE 1
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  INSTALL_COMMAND ""
)
