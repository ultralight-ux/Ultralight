///
/// @file CAPI.h
///
/// @brief The C-language API for AppCore
///
/// @author
///
/// This file is a part of Ultralight, a fast, lightweight, HTML UI engine
///
/// Website: <http://ultralig.ht>
///
/// Copyright (C) 2019 Ultralight, Inc. All rights reserved.
///
#ifndef APPCORE_CAPI_H
#define APPCORE_CAPI_H

#include <Ultralight/CAPI.h>

#if defined(__WIN32__) || defined(_WIN32)
#  if defined(APPCORE_IMPLEMENTATION)
#    define AExport __declspec(dllexport)
#  else
#    define AExport __declspec(dllimport)
#  endif
#else
#  define AExport __attribute__((visibility("default")))
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct C_App* ULApp;
typedef struct C_Window* ULWindow;
typedef struct C_Monitor* ULMonitor;
typedef struct C_Overlay* ULOverlay;

AExport ULApp ulCreateApp();

AExport void ulDestroyApp(ULApp app);

AExport void ulAppSetWindow(ULApp app, ULWindow window);

AExport ULWindow ulAppGetWindow(ULApp app);

typedef void
(*ULUpdateCallback) (void* user_data);

AExport void ulAppSetUpdateCallback(ULApp app, ULUpdateCallback callback,
                                    void* user_data);

AExport bool ulAppIsRunning(ULApp app);

AExport ULMonitor ulAppGetMainMonitor(ULApp app);

AExport ULRenderer ulAppGetRenderer(ULApp app);

AExport void ulAppRun(ULApp app);

AExport void ulAppQuit(ULApp app);

AExport double ulMonitorGetScale(ULMonitor monitor);

AExport int ulMonitorGetWidth(ULMonitor monitor);

AExport int ulMonitorGetHeight(ULMonitor monitor);

AExport ULWindow ulCreateWindow(ULMonitor monitor, unsigned int width,
	                              unsigned int height, bool fullscreen,
                                unsigned int window_flags);

AExport void ulDestroyWindow(ULWindow window);

typedef void
(*ULCloseCallback) (void* user_data);

AExport void ulWindowSetCloseCallback(ULWindow window,
                                      ULCloseCallback callback,
                                      void* user_data);

typedef void
(*ULResizeCallback) (void* user_data, int width, int height);

AExport void ulWindowSetResizeCallback(ULWindow window,
                                       ULResizeCallback callback,
                                       void* user_data);

AExport int ulWindowGetWidth(ULWindow window);

AExport int ulWindowGetHeight(ULWindow window);

AExport bool ulWindowIsFullscreen(ULWindow window);

AExport double ulWindowGetScale(ULWindow window);

AExport void ulWindowSetTitle(ULWindow window, const char* title);

AExport void ulWindowSetCursor(ULWindow window, ULCursor cursor);

AExport void ulWindowClose(ULWindow window);

AExport int ulWindowDeviceToPixel(ULWindow window, int val);

AExport int ulWindowPixelsToDevice(ULWindow window, int val);

AExport ULOverlay ulCreateOverlay(int width, int height, int x, int y);

AExport void ulDestroyOverlay(ULOverlay overlay);

AExport ULView ulOverlayGetView(ULOverlay overlay);

AExport int ulOverlayGetWidth(ULOverlay overlay);

AExport int ulOverlayGetHeight(ULOverlay overlay);

AExport int ulOverlayGetX(ULOverlay overlay);

AExport int ulOverlayGetY(ULOverlay overlay);

AExport void ulOverlayMoveTo(ULOverlay overlay, int x, int y);

AExport void ulOverlayResize(ULOverlay overlay, int width, int height);

#ifdef __cplusplus
}
#endif

#endif // APPCORE_CAPI_H
