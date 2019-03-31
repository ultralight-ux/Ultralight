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

/// @abstract Window creation flags. @see Window::Create
typedef enum {
  kWindowFlags_Borderless = 1 << 0,
  kWindowFlags_Titled = 1 << 1,
  kWindowFlags_Resizable = 1 << 2,
  kWindowFlags_Maximizable = 1 << 3,
} ULWindowFlags;

/// @function ulCreateApp
/// @abstract Create the App singleton.
/// @param config Configuration settings to use.
/// @note You should only create one of these per application lifetime.
///       App maintains its own Renderer instance, make sure to set your
///       Config before creating App. (@see Platform::set_config)
AExport ULApp ulCreateApp(ULConfig config);

/// @function ulDestroyApp
/// @abstract Destroy the App instance.
/// @param app The app that should be destroyed.
AExport void ulDestroyApp(ULApp app);

/// @function ulAppSetWindow
/// @abstract Set the main window. You must set this before calling ulAppRun.
/// @param app    The app singleton to be used.
/// @param window The window to use for all rendering.
/// @note We currently only support one Window per App, this will change
///       later once we add support for multiple driver instances.
AExport void ulAppSetWindow(ULApp app, ULWindow window);

/// @function ulAppGetWindow
/// @abstract Get the main window.
/// @param app The app singleton to be used.
AExport ULWindow ulAppGetWindow(ULApp app);

typedef void
(*ULUpdateCallback) (void* user_data);

/// @function ulAppSetUpdateCallback
/// @abstract Set a callback for whenever the App updates. You should update all app
///           logic here.
/// @param app       The app singleton to be used.
/// @param callback  A static callback function.
/// @param user_data A user-specified pointer to data that will be passed to the
///        callback function when called.
/// @note  This event is fired right before the run loop calls
///        Renderer::Update and Renderer::Render.
AExport void ulAppSetUpdateCallback(ULApp app, ULUpdateCallback callback,
                                    void* user_data);

/// @function ulAppIsRunning
/// @abstract Whether or not the App is running.
/// @param app The app singleton to be used.
AExport bool ulAppIsRunning(ULApp app);

/// @function ulAppGetMainMonitor
/// @abstract Get the main monitor (this is never NULL).
/// @param app The app singleton to be used.
/// @note  We'll add monitor enumeration later.
AExport ULMonitor ulAppGetMainMonitor(ULApp app);

/// @function ulAppGetRenderer
/// @abstract Get the underlying Renderer instance.
/// @param app The app singleton to be used.
AExport ULRenderer ulAppGetRenderer(ULApp app);

/// @function ulAppRun
/// @abstract Run the main loop.
/// @param app The app singleton to be used.
/// @note  Make sure to call ulAppSetWindow before calling this.
AExport void ulAppRun(ULApp app);

/// @function ulAppQuit
/// @abstract Quit the application.
/// @param app The app singleton to be used.
AExport void ulAppQuit(ULApp app);

/// @function ulMonitorGetScale
/// @abstract Get the monitor's DPI scale (1.0 = 100%).
/// @param monitor The monitor instance to be used.
///        (@see ulAppGetMainMonitor)
/// @note Since we currently only provide a monitor
///       instance for the main monitor, you will
///       not be able to obtain the DPI scale for a
///       secondary or currently inactive monitor.
AExport double ulMonitorGetScale(ULMonitor monitor);

/// @function ulMonitorGetWidth
/// @abstract Get the width of the monitor (in device coordinates)
/// @param monitor The monitor instance to be used.
///        (@see ulAppGetMainMonitor)
///
AExport unsigned int ulMonitorGetWidth(ULMonitor monitor);

/// @function ulMonitorGetHeight
/// @abstract Get the height of the monitor (in device coordinates)
/// @param monitor The monitor instance to be used.
AExport unsigned int ulMonitorGetHeight(ULMonitor monitor);

/// @function ulCreateWindow
/// @abstract Create a new Window.
/// @param monitor      The monitor to create the Window on.
/// @param width        The width (in device coordinates).
/// @param height       The height (in device coordinates).
/// @param fullscreen   Whether or not the window is fullscreen.
/// @param window_flags Various window flags.
AExport ULWindow ulCreateWindow(ULMonitor monitor,
                                unsigned int width,
                                unsigned int height,
                                bool fullscreen,
                                unsigned int window_flags);

/// @function ulDestroyWindow
/// @abstract Destroy a Window.
AExport void ulDestroyWindow(ULWindow window);

typedef void
(*ULCloseCallback) (void* user_data);

/// @function ulWindowSetCloseCallback
/// @abstract Set a callback to be notified when a window closes.
/// @param callback  A static callback function.
/// @param user_data A user-specified pointer to data that will be passed
///        to the callback function when called.
AExport void ulWindowSetCloseCallback(ULWindow window,
                                      ULCloseCallback callback,
                                      void* user_data);

typedef void
(*ULResizeCallback) (void* user_data, unsigned int width, unsigned int height);

/// @function ulWindowSetResizeCallback
/// @abstract Set a callback to be notified when a window resizes
///           (parameters are passed back in device coordinates).
/// @param window    The window instance to listen on.
/// @param callback  A static callback function.
/// @param user_data A user-specified pointer to data that will be passed
///        to the callback function when called.
AExport void ulWindowSetResizeCallback(ULWindow window,
                                       ULResizeCallback callback,
                                       void* user_data);

/// @function ulWindowGetWidth
/// @abstract Get window width (in device coordinates).
/// @param window The window instance to be used.
AExport unsigned int ulWindowGetWidth(ULWindow window);

/// @function ulWindowGetHeight
/// @abstract Get window height (in device coordinates).
/// @param window The window instance to be used.
AExport unsigned int ulWindowGetHeight(ULWindow window);

/// @function ulWindowIsFullscreen
/// @abstract Get whether or not a window is fullscreen.
/// @param window The window instance to be used.
AExport bool ulWindowIsFullscreen(ULWindow window);

/// @function ulWindowGetScale
/// @abstract Get the DPI scale of a window.
/// @param window The window instance to be used.
AExport double ulWindowGetScale(ULWindow window);

/// @function ulWindowSetTitle
/// @abstract Set the window title.
/// @param window The window instance to be used.
AExport void ulWindowSetTitle(ULWindow window, const char* title);

/// @function ulWindowSetCursor
/// @abstract Set the cursor for a window.
/// @param window The window instance to be used.
AExport void ulWindowSetCursor(ULWindow window, ULCursor cursor);

/// @function ulWindowClose
/// @abstract Close a window.
/// @param window The window instance to be closed.
AExport void ulWindowClose(ULWindow window);

/// @function ulWindowDeviceToPixel
/// @abstract Convert device coordinates to pixels using the
///           current DPI scale.
/// @param window The window instance whose DPI scale shall be used.
/// @param val    The X or Y coordinate to be translated.
AExport int ulWindowDeviceToPixel(ULWindow window, int val);

/// @function ulWindowPixelsToDevice
/// @abstract Convert pixels to device coordinates using the
///           current DPI scale.
/// @param window The window instance whose DPI scale shall be used.
/// @param val    The X or Y coordinate to be translated.
AExport int ulWindowPixelsToDevice(ULWindow window, int val);

/// @function ulCreateOverlay
/// @abstract Create a new Overlay.
/// @param  window  The window to create the Overlay in. (we currently
///                 only support one window per application)
/// @param  width   The width in device coordinates.
/// @param  height  The height in device coordinates.
/// @param  x       The x-position (offset from the left of the Window),
///                 in device coordinates.
/// @param  y       The y-position (offset from the top of the Window),
///                 in device coordinates.
/// @note  Each Overlay is essentially a View and an on-screen quad. You
///        should create the Overlay then load content into the underlying
///        View.
AExport ULOverlay ulCreateOverlay(ULWindow window, unsigned int width,
                                  unsigned int height, int x, int y);

/// @function ulDestroyOverlay
/// @abstract Destroy an overlay.
/// @param overlay The overlay that shall be destroyed.
AExport void ulDestroyOverlay(ULOverlay overlay);

/// @function ulOverlayGetView
/// @abstract Get the underlying View.
/// @param overlay The overlay whose view shall be returned.
AExport ULView ulOverlayGetView(ULOverlay overlay);

/// @function ulOverlayGetWidth
/// @abstract Get the width (in device coordinates).
/// @param overlay The overlay whose width shall be returned.
AExport unsigned int ulOverlayGetWidth(ULOverlay overlay);

/// @function ulOverlayGetHeight
/// @abstract Get the height (in device coordinates).
/// @param overlay The overlay whose height shall be returned.
AExport unsigned int ulOverlayGetHeight(ULOverlay overlay);

/// @function ulOverlayGetX
/// @abstract Get the x-position (offset from the left of the Window),
///           in device coordinates.
/// @param overlay The overlay whose X position shall be determined.
AExport int ulOverlayGetX(ULOverlay overlay);

/// @function ulOverlayGetY
/// @abstract Get the y-position (offset from the top of the Window),
///           in device coordinates.
/// @param overlay The overlay whose Y position shall be determined.
AExport int ulOverlayGetY(ULOverlay overlay);

/// @function ulOverlayMoveTo
/// @abstract Move the overlay to a new position (in device coordinates).
/// @param overlay The overlay that shall be moved.
/// @param x The new X-position (in device coordinates) of the overlay.
/// @param y The new Y-position (in device coordinates) of the overlay.
AExport void ulOverlayMoveTo(ULOverlay overlay, int x, int y);

/// @function ulOverlayResize
/// @abstract Resize the overlay (and underlying View), dimensions should be
///           specified in device coordinates.
/// @param overlay The overlay that shall be resized.
/// @param width   The new width (in device-coordinates) of the overlay.
/// @param height  The new height (in device-coordinates) of the overlay.
AExport void ulOverlayResize(ULOverlay overlay, unsigned int width,
                             unsigned int height);

/// @function ulOverlayIsHidden
/// @abstract Whether or not the overlay is hidden (not drawn).
/// @param overlay The overlay to be used.
AExport bool ulOverlayIsHidden(ULOverlay overlay);

/// @function ulOverlayHide
/// @abstract Hide the overlay (will no longer be drawn).
/// @param overlay The overlay that shall be hidden.
AExport void ulOverlayHide(ULOverlay overlay);

/// @function ulOverlayHide
/// @abstract Show the overlay (will be drawn again).
/// @param overlay The overlay that shall be shown.
AExport void ulOverlayShow(ULOverlay overlay);

/// @function ulOverlayHasFocus
/// @abstract Whether or not an overlay has keyboard focus.
/// @param overlay The overlay to be checked for focus.
AExport bool ulOverlayHasFocus(ULOverlay overlay);

/// @function ulOverlayFocus
/// @abstract Grant this overlay exclusive keyboard focus.
/// @param overlay The overlay that shall be granted exclusive
///                keyboard focus.
AExport void ulOverlayFocus(ULOverlay overlay);

/// @function ulOverlayUnfocus
/// @abstract Remove keyboard focus.
/// @param overlay The overlay whose focus shall be removed.
AExport void ulOverlayUnfocus(ULOverlay overlay);

#ifdef __cplusplus
}
#endif

#endif // APPCORE_CAPI_H
