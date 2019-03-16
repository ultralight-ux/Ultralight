///
/// @file CAPI.h
///
/// @brief The C-language API for Ultralight
///
/// @author
///
/// This file is a part of Ultralight, a fast, lightweight, HTML UI engine
///
/// Website: <http://ultralig.ht>
///
/// Copyright (C) 2019 Ultralight, Inc. All rights reserved.
///
#ifndef ULTRALIGHT_CAPI_H
#define ULTRALIGHT_CAPI_H

#ifndef __cplusplus
//#include <stdbool.h>
#endif

#include <stddef.h>
#include <JavaScriptCore/JavaScript.h>
#ifdef __OBJC__
#import <AppKit/NSEvent.h>
#endif

#if defined(__WIN32__) || defined(_WIN32)
#  if defined(ULTRALIGHT_IMPLEMENTATION)
#    define ULExport __declspec(dllexport)
#  else
#    define ULExport __declspec(dllimport)
#  endif
#define _thread_local __declspec(thread)
#ifndef _NATIVE_WCHAR_T_DEFINED
#define DISABLE_NATIVE_WCHAR_T
typedef unsigned short ULChar16;
#else
typedef wchar_t ULChar16;
#endif
#else
#  define ULExport __attribute__((visibility("default")))
#define _thread_local __thread
typedef unsigned short ULChar16;
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct C_Config* ULConfig;
typedef struct C_Renderer* ULRenderer;
typedef struct C_View* ULView;
typedef struct C_Bitmap* ULBitmap;
typedef struct C_String* ULString;
typedef struct C_Buffer* ULBuffer;
typedef struct C_RenderTarget* ULRenderTarget;
typedef struct C_KeyEvent* ULKeyEvent;
typedef struct C_MouseEvent* ULMouseEvent;
typedef struct C_ScrollEvent* ULScrollEvent;

typedef enum {
  kMessageSource_XML = 0,
  kMessageSource_JS,
  kMessageSource_Network,
  kMessageSource_ConsoleAPI,
  kMessageSource_Storage,
  kMessageSource_AppCache,
  kMessageSource_Rendering,
  kMessageSource_CSS,
  kMessageSource_Security,
  kMessageSource_ContentBlocker,
  kMessageSource_Other,
} ULMessageSource;

typedef enum {
  kMessageLevel_Log = 1,
  kMessageLevel_Warning = 2,
  kMessageLevel_Error = 3,
  kMessageLevel_Debug = 4,
  kMessageLevel_Info = 5,
} ULMessageLevel;

typedef enum {
  kCursor_Pointer = 0,
  kCursor_Cross,
  kCursor_Hand,
  kCursor_IBeam,
  kCursor_Wait,
  kCursor_Help,
  kCursor_EastResize,
  kCursor_NorthResize,
  kCursor_NorthEastResize,
  kCursor_NorthWestResize,
  kCursor_SouthResize,
  kCursor_SouthEastResize,
  kCursor_SouthWestResize,
  kCursor_WestResize,
  kCursor_NorthSouthResize,
  kCursor_EastWestResize,
  kCursor_NorthEastSouthWestResize,
  kCursor_NorthWestSouthEastResize,
  kCursor_ColumnResize,
  kCursor_RowResize,
  kCursor_MiddlePanning,
  kCursor_EastPanning,
  kCursor_NorthPanning,
  kCursor_NorthEastPanning,
  kCursor_NorthWestPanning,
  kCursor_SouthPanning,
  kCursor_SouthEastPanning,
  kCursor_SouthWestPanning,
  kCursor_WestPanning,
  kCursor_Move,
  kCursor_VerticalText,
  kCursor_Cell,
  kCursor_ContextMenu,
  kCursor_Alias,
  kCursor_Progress,
  kCursor_NoDrop,
  kCursor_Copy,
  kCursor_None,
  kCursor_NotAllowed,
  kCursor_ZoomIn,
  kCursor_ZoomOut,
  kCursor_Grab,
  kCursor_Grabbing,
  kCursor_Custom
} ULCursor;

typedef enum {
  kBitmapFormat_A8,
  kBitmapFormat_RGBA8
} ULBitmapFormat;

typedef enum {
  kKeyEventType_KeyDown,
  kKeyEventType_KeyUp,
  kKeyEventType_RawKeyDown,
  kKeyEventType_Char,
} ULKeyEventType;

typedef enum {
  kMouseEventType_MouseMoved,
  kMouseEventType_MouseDown,
  kMouseEventType_MouseUp,
} ULMouseEventType;

typedef enum {
  kMouseButton_None = 0,
  kMouseButton_Left,
  kMouseButton_Middle,
  kMouseButton_Right,
} ULMouseButton;

typedef enum {
  kScrollEventType_ScrollByPixel,
  kScrollEventType_ScrollByPage,
} ULScrollEventType;

/******************************************************************************
 * API Note:
 *
 * You should only destroy objects that you explicitly create. Do not destroy
 * any objects returned from the API or callbacks unless otherwise noted.
 *****************************************************************************/

/******************************************************************************
 * Config
 *****************************************************************************/

///
/// Create config with default values (see <Ultralight/platform/Config.h>).
///
ULExport ULConfig ulCreateConfig();

///
/// Destroy config.
///
ULExport void ulDestroyConfig(ULConfig config);

///
/// Set whether images should be enabled (Default = True)
///
ULExport void ulConfigSetEnableImages(ULConfig config, bool enabled);

///
/// Set whether JavaScript should be eanbled (Default = True)
///
ULExport void ulConfigSetEnableJavaScript(ULConfig config, bool enabled);

///
/// Set whether we should use BGRA byte order (instead of RGBA) for View
/// bitmaps. (Default = False)
///
ULExport void ulConfigSetUseBGRAForOffscreenRendering(ULConfig config,
                                                      bool enabled);

///
/// Set the amount that the application DPI has been scaled, used for
/// scaling device coordinates to pixels and oversampling raster shapes.
/// (Default = 1.0)
///
ULExport void ulConfigSetDeviceScaleHint(ULConfig config, double value);

///
/// Set default font-family to use (Default = Times New Roman)
///
ULExport void ulConfigSetFontFamilyStandard(ULConfig config,
                                            ULString font_name);

///
/// Set default font-family to use for fixed fonts, eg <pre> and <code>.
/// (Default = Courier New)
///
ULExport void ulConfigSetFontFamilyFixed(ULConfig config, ULString font_name);

///
/// Set default font-family to use for serif fonts. (Default = Times New Roman)
///
ULExport void ulConfigSetFontFamilySerif(ULConfig config, ULString font_name);

///
/// Set default font-family to use for sans-serif fonts. (Default = Arial)
///
ULExport void ulConfigSetFontFamilySansSerif(ULConfig config,
                                             ULString font_name);

///
/// Set user agent string. (See <Ultralight/platform/Config.h> for the default)
///
ULExport void ulConfigSetUserAgent(ULConfig config, ULString agent_string);

///
/// Set user stylesheet (CSS). (Default = Empty)
///
ULExport void ulConfigSetUserStylesheet(ULConfig config, ULString css_string);

/******************************************************************************
 * Renderer
 *****************************************************************************/

///
/// Create renderer (create this only once per application lifetime).
///
ULExport ULRenderer ulCreateRenderer(ULConfig config);

///
/// Destroy renderer.
///
ULExport void ulDestroyRenderer(ULRenderer renderer);

///
/// Update timers and dispatch internal callbacks (JavaScript and network)
///
ULExport void ulUpdate(ULRenderer renderer);

///
/// Render all active Views to their respective bitmaps.
///
ULExport void ulRender(ULRenderer renderer);

/******************************************************************************
 * View
 *****************************************************************************/

///
/// Create a View with certain size (in device coordinates).
///
ULExport ULView ulCreateView(ULRenderer renderer, unsigned int width,
                             unsigned int height, bool transparent);

///
/// Destroy a View.
///
ULExport void ulDestroyView(ULView view);

///
/// Get current URL.
///
/// @note Don't destroy the returned string, it is owned by the View.
///
ULExport ULString ulViewGetURL(ULView view);

///
/// Get current title.
///
/// @note Don't destroy the returned string, it is owned by the View.
///
ULExport ULString ulViewGetTitle(ULView view);

///
/// Check if main frame is loading.
///
ULExport bool ulViewIsLoading(ULView view);

///
/// Check if bitmap is dirty (has changed since last call to ulViewGetBitmap)
///
ULExport bool ulViewIsBitmapDirty(ULView view);

///
/// Get bitmap (will reset the dirty flag).
///
/// @note Don't destroy the returned bitmap, it is owned by the View.
///
ULExport ULBitmap ulViewGetBitmap(ULView view);

///
/// Load a raw string of html
///
ULExport void ulViewLoadHTML(ULView view, ULString html_string);

///
/// Load a URL into main frame
///
ULExport void ulViewLoadURL(ULView view, ULString url_string);

///
/// Resize view to a certain width and height (in device coordinates)
///
ULExport void ulViewResize(ULView view, unsigned int width,
                           unsigned int height);

///
/// Get the page's JSContext for use with JavaScriptCore API
///
ULExport JSContextRef ulViewGetJSContext(ULView view);

///
/// Evaluate a raw string of JavaScript and return result
///
ULExport JSValueRef ulViewEvaluateScript(ULView view, ULString js_string);

///
/// Check if can navigate backwards in history
///
ULExport bool ulViewCanGoBack(ULView view);

///
/// Check if can navigate forwards in history
///
ULExport bool ulViewCanGoForward(ULView view);

///
/// Navigate backwards in history
///
ULExport void ulViewGoBack(ULView view);

///
/// Navigate forwards in history
///
ULExport void ulViewGoForward(ULView view);

///
/// Navigate to arbitrary offset in history
///
ULExport void ulViewGoToHistoryOffset(ULView view, int offset);

///
/// Reload current page
///
ULExport void ulViewReload(ULView view);

///
/// Stop all page loads
///
ULExport void ulViewStop(ULView view);

///
/// Fire a keyboard event
///
ULExport void ulViewFireKeyEvent(ULView view, ULKeyEvent key_event);

///
/// Fire a mouse event
///
ULExport void ulViewFireMouseEvent(ULView view, ULMouseEvent mouse_event);

///
/// Fire a scroll event
///
ULExport void ulViewFireScrollEvent(ULView view, ULScrollEvent scroll_event);

typedef void
(*ULChangeTitleCallback) (void* user_data, ULView caller, ULString title);

///
/// Set callback for when the page title changes
///
ULExport void ulViewSetChangeTitleCallback(ULView view,
                                           ULChangeTitleCallback callback,
                                           void* user_data);

typedef void
(*ULChangeURLCallback) (void* user_data, ULView caller, ULString url);

///
/// Set callback for when the page URL changes
///
ULExport void ulViewSetChangeURLCallback(ULView view,
                                         ULChangeURLCallback callback,
                                         void* user_data);

typedef void
(*ULChangeTooltipCallback) (void* user_data, ULView caller, ULString tooltip);

///
/// Set callback for when the tooltip changes (usually result of a mouse hover)
///
ULExport void ulViewSetChangeTooltipCallback(ULView view,
                                             ULChangeTooltipCallback callback,
                                             void* user_data);

typedef void
(*ULChangeCursorCallback) (void* user_data, ULView caller, ULCursor cursor);

///
/// Set callback for when the mouse cursor changes
///
ULExport void ulViewSetChangeCursorCallback(ULView view,
                                            ULChangeCursorCallback callback,
                                            void* user_data);

typedef void
(*ULAddConsoleMessageCallback) (void* user_data, ULView caller,
                                ULMessageSource source, ULMessageLevel level,
                                ULString message, unsigned int line_number,
                                unsigned int column_number,
                                ULString source_id);

///
/// Set callback for when a message is added to the console (useful for
/// JavaScript / network errors and debugging)
///
ULExport void ulViewSetAddConsoleMessageCallback(ULView view,
                                          ULAddConsoleMessageCallback callback,
                                          void* user_data);

typedef void
(*ULBeginLoadingCallback) (void* user_data, ULView caller);

///
/// Set callback for when the page begins loading new URL into main frame
///
ULExport void ulViewSetBeginLoadingCallback(ULView view,
                                            ULBeginLoadingCallback callback,
                                            void* user_data);

typedef void
(*ULFinishLoadingCallback) (void* user_data, ULView caller);

///
/// Set callback for when the page finishes loading URL into main frame
///
ULExport void ulViewSetFinishLoadingCallback(ULView view,
                                             ULFinishLoadingCallback callback,
                                             void* user_data);

typedef void
(*ULUpdateHistoryCallback) (void* user_data, ULView caller);

///
/// Set callback for when the history (back/forward state) is modified
///
ULExport void ulViewSetUpdateHistoryCallback(ULView view,
                                             ULUpdateHistoryCallback callback,
                                             void* user_data);

typedef void
(*ULDOMReadyCallback) (void* user_data, ULView caller);

///
/// Set callback for when all JavaScript has been parsed and the document is
/// ready. This is the best time to make initial JavaScript calls to your page.
///
ULExport void ulViewSetDOMReadyCallback(ULView view,
                                        ULDOMReadyCallback callback,
                                        void* user_data);

///
/// Set whether or not a view should be repainted during the next call to
/// ulRender.
///
/// @note  This flag is automatically set whenever the page content changes
///        but you can set it directly in case you need to force a repaint.
///
ULExport void ulViewSetNeedsPaint(ULView view, bool needs_paint);

///
/// Whether or not a view should be painted during the next call to ulRender.
///
ULExport bool ulViewGetNeedsPaint(ULView view);

/******************************************************************************
 * String
 *****************************************************************************/

///
/// Create string from null-terminated ASCII C-string
///
ULExport ULString ulCreateString(const char* str);

///
/// Create string from UTF-8 buffer
///
ULExport ULString ulCreateStringUTF8(const char* str, size_t len);

///
/// Create string from UTF-16 buffer
///
ULExport ULString ulCreateStringUTF16(ULChar16* str, size_t len);

///
/// Destroy string (you should destroy any strings you explicitly Create).
///
ULExport void ulDestroyString(ULString str);

///
/// Get internal UTF-16 buffer data.
///
ULExport ULChar16* ulStringGetData(ULString str);

///
/// Get length in UTF-16 characters
///
ULExport size_t ulStringGetLength(ULString str);

///
/// Whether this string is empty or not.
///
ULExport bool ulStringIsEmpty(ULString str);

/******************************************************************************
 * Bitmap
 *****************************************************************************/

///
/// Create empty bitmap.
///
ULExport ULBitmap ulCreateEmptyBitmap();

///
/// Create bitmap with certain dimensions and pixel format.
///
ULExport ULBitmap ulCreateBitmap(unsigned int width, unsigned int height,
                                 ULBitmapFormat format);

///
/// Create bitmap from existing pixel buffer. @see Bitmap for help using
/// this function.
///
ULExport ULBitmap ulCreateBitmapFromPixels(unsigned int width,
                                           unsigned int height,
                                           ULBitmapFormat format, 
                                           unsigned int row_bytes,
                                           const void* pixels, size_t size,
                                           bool should_copy);

///
/// Create bitmap from copy.
///
ULExport ULBitmap ulCreateBitmapFromCopy(ULBitmap existing_bitmap);

///
/// Destroy a bitmap (you should only destroy Bitmaps you have explicitly
/// created via one of the creation functions above.
///
ULExport void ulDestroyBitmap(ULBitmap bitmap);

///
/// Get the width in pixels.
///
ULExport unsigned int ulBitmapGetWidth(ULBitmap bitmap);

///
/// Get the height in pixels.
///
ULExport unsigned int ulBitmapGetHeight(ULBitmap bitmap);

///
/// Get the pixel format.
///
ULExport ULBitmapFormat ulBitmapGetFormat(ULBitmap bitmap);

///
/// Get the bytes per pixel.
///
ULExport unsigned int ulBitmapGetBpp(ULBitmap bitmap);

///
/// Get the number of bytes per row.
///
ULExport unsigned int ulBitmapGetRowBytes(ULBitmap bitmap);

///
/// Get the size in bytes of the underlying pixel buffer.
///
ULExport size_t ulBitmapGetSize(ULBitmap bitmap);

///
/// Whether or not this bitmap owns its own pixel buffer.
///
ULExport bool ulBitmapOwnsPixels(ULBitmap bitmap);

///
/// Lock pixels for reading/writing, returns pointer to pixel buffer.
///
ULExport void* ulBitmapLockPixels(ULBitmap bitmap);

///
/// Unlock pixels after locking.
///
ULExport void ulBitmapUnlockPixels(ULBitmap bitmap);

///
/// Get raw pixel buffer-- you should only call this if Bitmap is already
/// locked.
///
ULExport void* ulBitmapRawPixels(ULBitmap bitmap);

///
/// Whether or not this bitmap is empty.
///
ULExport bool ulBitmapIsEmpty(ULBitmap bitmap);

///
/// Reset bitmap pixels to 0.
///
ULExport void ulBitmapErase(ULBitmap bitmap);

///
/// Write bitmap to a PNG on disk.
///
ULExport bool ulBitmapWritePNG(ULBitmap bitmap, const char* path);

/******************************************************************************
* Key Event
******************************************************************************/

///
/// Create a key event, @see KeyEvent for help with the following parameters.
///
ULExport ULKeyEvent ulCreateKeyEvent(ULKeyEventType type,
                                     unsigned int modifiers,
                                     int virtual_key_code, int native_key_code,
                                     ULString text, ULString unmodified_text,
                                     bool is_keypad, bool is_auto_repeat,
                                     bool is_system_key);

#ifdef _WIN32
///
/// Create a key event from native Windows event.
///
ULExport ULKeyEvent ulCreateKeyEventWindows(ULKeyEventType type,
                                            uintptr_t wparam, intptr_t lparam,
                                            bool is_system_key);
#endif

#ifdef __OBJC__
///
/// Create a key event from native macOS event.
///
ULExport ULKeyEvent ulCreateKeyEventMacOS(NSEvent* evt);
#endif

///
/// Destroy a key event.
///
ULExport void ulDestroyKeyEvent(ULKeyEvent evt);

/******************************************************************************
 * Mouse Event
 *****************************************************************************/

///
/// Create a mouse event, @see MouseEvent for help using this function.
///
ULExport ULMouseEvent ulCreateMouseEvent(ULMouseEventType type, int x, int y,
                                         ULMouseButton button);

///
/// Destroy a mouse event.
///
ULExport void ulDestroyMouseEvent(ULMouseEvent evt);

/******************************************************************************
 * Scroll Event
 *****************************************************************************/

///
/// Create a scroll event, @see ScrollEvent for help using this function.
///
ULExport ULScrollEvent ulCreateScrollEvent(ULScrollEventType type, int delta_x,
                                           int delta_y);

///
/// Destroy a scroll event.
///
ULExport void ulDestroyScrollEvent(ULScrollEvent evt);

#ifdef __cplusplus
}
#endif

#endif // ULTRALIGHT_CAPI_H
