#include "WindowWin.h"
#include <AppCore/Monitor.h>
#include <Windows.h>
#include <ShellScalingAPI.h>
#include "Windowsx.h"
#include <tchar.h>
#include "AppWin.h"

namespace ultralight {

#define WINDOWDATA() ((WindowData*)GetWindowLongPtr(hWnd, GWLP_USERDATA))
#define WINDOW() (WINDOWDATA()->window)

static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
  PAINTSTRUCT ps;
  HDC hdc;
  switch (message) {
  case WM_PAINT:
    hdc = BeginPaint(hWnd, &ps);
    EndPaint(hWnd, &ps);
    break;
  case WM_DESTROY:
    PostQuitMessage(0);
    break;
  case WM_ENTERSIZEMOVE:
    WINDOWDATA()->is_resizing_modal = true;
    break;
  case WM_SIZE:
    if (WINDOWDATA())
      if (!WINDOWDATA()->is_resizing_modal)
        WINDOW()->OnResize(WINDOW()->width(), WINDOW()->height());
    break;
  case WM_EXITSIZEMOVE:
    WINDOWDATA()->is_resizing_modal = false;
    WINDOW()->OnResize(WINDOW()->width(), WINDOW()->height());
    break;
  case WM_KEYDOWN:
    WINDOW()->FireKeyEvent(KeyEvent(KeyEvent::kType_RawKeyDown, (uintptr_t)wParam, (intptr_t)lParam, false));
    break;
  case WM_KEYUP:
    WINDOW()->FireKeyEvent(KeyEvent(KeyEvent::kType_KeyUp, (uintptr_t)wParam, (intptr_t)lParam, false));
    break;
  case WM_CHAR:
    WINDOW()->FireKeyEvent(KeyEvent(KeyEvent::kType_Char, (uintptr_t)wParam, (intptr_t)lParam, false));
    break;
  case WM_MOUSEMOVE:
    WINDOW()->FireMouseEvent(
      { MouseEvent::kType_MouseMoved,
        WINDOW()->PixelsToDevice(GET_X_LPARAM(lParam)),
        WINDOW()->PixelsToDevice(GET_Y_LPARAM(lParam)),
        WINDOWDATA()->cur_btn });
    break;
  case WM_LBUTTONDOWN:
    WINDOWDATA()->cur_btn = MouseEvent::kButton_Left;
    WINDOW()->FireMouseEvent(
      { MouseEvent::kType_MouseDown,
        WINDOW()->PixelsToDevice(GET_X_LPARAM(lParam)),
        WINDOW()->PixelsToDevice(GET_Y_LPARAM(lParam)),
        WINDOWDATA()->cur_btn });
    break;
  case WM_MBUTTONDOWN:
    WINDOWDATA()->cur_btn = MouseEvent::kButton_Middle;
    WINDOW()->FireMouseEvent(
    { MouseEvent::kType_MouseDown,
      WINDOW()->PixelsToDevice(GET_X_LPARAM(lParam)),
      WINDOW()->PixelsToDevice(GET_Y_LPARAM(lParam)),
      WINDOWDATA()->cur_btn });
    break;
  case WM_RBUTTONDOWN:
    WINDOWDATA()->cur_btn = MouseEvent::kButton_Right;
    WINDOW()->FireMouseEvent(
    { MouseEvent::kType_MouseDown,
      WINDOW()->PixelsToDevice(GET_X_LPARAM(lParam)),
      WINDOW()->PixelsToDevice(GET_Y_LPARAM(lParam)),
      WINDOWDATA()->cur_btn });
    break;
  case WM_LBUTTONUP:
  case WM_MBUTTONUP:
  case WM_RBUTTONUP:
    WINDOW()->FireMouseEvent(
    { MouseEvent::kType_MouseUp,
      WINDOW()->PixelsToDevice(GET_X_LPARAM(lParam)),
      WINDOW()->PixelsToDevice(GET_Y_LPARAM(lParam)),
      WINDOWDATA()->cur_btn });
	WINDOWDATA()->cur_btn = MouseEvent::kButton_None;
    break;
  case WM_MOUSEWHEEL:
    WINDOW()->FireScrollEvent(
      { ScrollEvent::kType_ScrollByPixel, 0, WINDOW()->PixelsToDevice(GET_WHEEL_DELTA_WPARAM(wParam)) });
    break;
  default:
    return DefWindowProc(hWnd, message, wParam, lParam);
  }
  return 0;
}

void CenterHwndOnMainMonitor(HWND hwnd)
{
  RECT rect;
  GetWindowRect(hwnd, &rect);
  LPRECT prc = &rect;

  // Get main monitor
  HMONITOR hMonitor = MonitorFromPoint({ 1,1 }, MONITOR_DEFAULTTONEAREST);

  MONITORINFO mi;
  RECT        rc;
  int         w = prc->right - prc->left;
  int         h = prc->bottom - prc->top;

  mi.cbSize = sizeof(mi);
  GetMonitorInfo(hMonitor, &mi);

  rc = mi.rcMonitor;

  prc->left = rc.left + (rc.right - rc.left - w) / 2;
  prc->top = rc.top + (rc.bottom - rc.top - h) / 2;
  prc->right = prc->left + w;
  prc->bottom = prc->top + h;

  SetWindowPos(hwnd, NULL, rect.left, rect.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
}

WindowWin::WindowWin(Monitor* monitor, uint32_t width, uint32_t height,
  bool fullscreen, unsigned int window_flags) : monitor_(monitor), is_fullscreen_(fullscreen) {

  TCHAR* class_name = _T("UltralightWindow");

  HINSTANCE hInstance = GetModuleHandle(NULL);

  WNDCLASSEX wcex;
  wcex.cbSize = sizeof(WNDCLASSEX);
  wcex.style = CS_HREDRAW | CS_VREDRAW;
  wcex.lpfnWndProc = WndProc;
  wcex.cbClsExtra = 0;
  wcex.cbWndExtra = 0;
  wcex.hInstance = hInstance;
  wcex.hIcon = LoadIcon(hInstance, (LPCTSTR)IDI_APPLICATION);
  wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
  wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
  wcex.lpszMenuName = NULL;
  wcex.lpszClassName = class_name;
  wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_APPLICATION);
  if (!RegisterClassEx(&wcex)) {
    MessageBoxW(NULL, (LPCWSTR)L"RegisterClassEx failed", (LPCWSTR)L"Notification", MB_OK);
    exit(-1);
  }

  DWORD style = WS_SYSMENU;
  if (window_flags & kWindowFlags_Borderless)
    style |= WS_POPUP;
  else
    style |= WS_BORDER;
  if (window_flags & kWindowFlags_Titled)
    style |= WS_CAPTION;
  if (window_flags & kWindowFlags_Resizable)
    style |= WS_SIZEBOX;
  if (window_flags & kWindowFlags_Maximizable)
    style |= WS_MINIMIZEBOX | WS_MAXIMIZEBOX;

  // Create window
  RECT rc = { 0, 0, (LONG)DeviceToPixels(width), (LONG)DeviceToPixels(height) };
  AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
  hwnd_ = ::CreateWindowEx(NULL
    , class_name
    , _T("")
    , fullscreen ? (WS_EX_TOPMOST | WS_POPUP) : style
    , fullscreen ? 0 : CW_USEDEFAULT
    , fullscreen ? 0 : CW_USEDEFAULT
    , fullscreen ? DeviceToPixels(width) : (rc.right - rc.left)
    , fullscreen ? DeviceToPixels(height) : (rc.bottom - rc.top)
    , NULL
    , NULL
    , hInstance
    , NULL);

  if (!hwnd_) {
    MessageBoxW(NULL, (LPCWSTR)L"CreateWindowEx failed", (LPCWSTR)L"Notification", MB_OK);
    exit(-1);
  }

  window_data_.window = this;
  window_data_.cur_btn = ultralight::MouseEvent::kButton_None;
  window_data_.is_resizing_modal = false;

  SetWindowLongPtr(hwnd_, GWLP_USERDATA, (LONG_PTR)&window_data_);

  CenterHwndOnMainMonitor(hwnd_);

  ShowWindow(hwnd_, SW_SHOW);

  // Set the thread affinity mask for better clock
  ::SetThreadAffinityMask(::GetCurrentThread(), 1);

  cursor_hand_ = ::LoadCursor(NULL, IDC_HAND);
  cursor_arrow_ = ::LoadCursor(NULL, IDC_ARROW);
  cursor_ibeam_ = ::LoadCursor(NULL, IDC_IBEAM);
  cur_cursor_ = ultralight::kCursor_Hand;
}

WindowWin::~WindowWin() {
  DestroyCursor(cursor_hand_);
  DestroyCursor(cursor_arrow_);
  DestroyCursor(cursor_ibeam_);
}

uint32_t WindowWin::width() const {
  RECT rc;
  ::GetClientRect(hwnd_, &rc);
  return (uint32_t)PixelsToDevice(rc.right - rc.left);
}

uint32_t WindowWin::height() const {
  RECT rc;
  ::GetClientRect(hwnd_, &rc);
  return (uint32_t)PixelsToDevice(rc.bottom - rc.top);
}

double WindowWin::scale() const {
  return monitor_->scale();
}

void WindowWin::SetTitle(const char* title) {
  SetWindowTextA(hwnd_, title);
}

void WindowWin::SetCursor(ultralight::Cursor cursor) {
  switch (cursor) {
  case ultralight::kCursor_Hand: {
    if (cur_cursor_ != ultralight::kCursor_Hand)
      ::SetCursor(cursor_hand_);
    break;
  }
  case ultralight::kCursor_Pointer: {
    if (cur_cursor_ != ultralight::kCursor_Pointer)
      ::SetCursor(cursor_arrow_);
    break;
  }
  case ultralight::kCursor_IBeam: {
    if (cur_cursor_ != ultralight::kCursor_IBeam)
      ::SetCursor(cursor_ibeam_);
    break;
  }
  };

  cur_cursor_ = cursor;
}

void WindowWin::Close() {
  DestroyWindow(hwnd_);
}

void WindowWin::OnClose() {
  if (listener_)
    listener_->OnClose();
  if (app_listener_)
    app_listener_->OnClose();
}

void WindowWin::OnResize(uint32_t width, uint32_t height) {
  if (listener_)
    listener_->OnResize(width, height);
  if (app_listener_)
    app_listener_->OnResize(width, height);
}

Ref<Window> Window::Create(Monitor* monitor, uint32_t width, uint32_t height,
  bool fullscreen, unsigned int window_flags) {
  return AdoptRef(*new WindowWin(monitor, width, height, fullscreen, window_flags));
}

Window::~Window() {}

}  // namespace ultralight
