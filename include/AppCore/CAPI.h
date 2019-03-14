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

AExport ULApp ULCreateApp();

AExport void ULDestroyApp(ULApp app);

AExport void ULAppSetWindow(ULApp app, ULWindow window);

AExport ULWindow ULAppGetWindow(ULApp app);

typedef void
(*ULUpdateCallback) (void* user_data);

AExport void ULAppSetUpdateCallback(ULApp app, ULUpdateCallback callback, void* user_data);

AExport bool ULAppIsRunning(ULApp app);

AExport ULMonitor ULAppGetMainMonitor(ULApp app);

AExport ULRenderer ULAppGetRenderer(ULApp app);

AExport void ULAppRun(ULApp app);

AExport void ULAppQuit(ULApp app);

AExport double ULMonitorGetScale(ULMonitor monitor);

AExport int ULMonitorGetWidth(ULMonitor monitor);

AExport int ULMonitorGetHeight(ULMonitor monitor);

AExport ULWindow ULCreateWindow(ULMonitor monitor, unsigned int width,
	unsigned int height, bool fullscreen, unsigned int window_flags);

AExport void ULDestroyWindow(ULWindow window);

typedef void
(*ULCloseCallback) (void* user_data);

AExport void ULWindowSetCloseCallback(ULWindow window, ULCloseCallback callback, void* user_data);

typedef void
(*ULResizeCallback) (void* user_data, int width, int height);

AExport void ULWindowSetResizeCallback(ULWindow window, ULResizeCallback callback, void* user_data);

AExport int ULWindowGetWidth(ULWindow window);

AExport int ULWindowGetHeight(ULWindow window);

AExport bool ULWindowIsFullscreen(ULWindow window);

AExport double ULWindowGetScale(ULWindow window);

AExport void ULWindowSetTitle(ULWindow window, const char* title);

AExport void ULWindowSetCursor(ULWindow window, ULCursor cursor);

AExport void ULWindowClose(ULWindow window);

AExport int ULWindowDeviceToPixel(ULWindow window, int val);

AExport int ULWindowPixelsToDevice(ULWindow window, int val);

AExport ULOverlay ULCreateOverlay(int width, int height, int x, int y);

AExport void ULDestroyOverlay(ULOverlay overlay);

AExport ULView ULOverlayGetView(ULOverlay overlay);

AExport int ULOverlayGetWidth(ULOverlay overlay);

AExport int ULOverlayGetHeight(ULOverlay overlay);

AExport int ULOverlayGetX(ULOverlay overlay);

AExport int ULOverlayGetY(ULOverlay overlay);

AExport void ULOverlayMoveTo(ULOverlay overlay, int x, int y);

AExport void ULOverlayResize(ULOverlay overlay, int width, int height);

#ifdef __cplusplus
}
#endif

#endif // APPCORE_CAPI_H
