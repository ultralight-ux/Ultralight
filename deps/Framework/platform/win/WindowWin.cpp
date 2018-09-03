#include <Framework/Window.h>
#include <Windows.h>
#include <ShellScalingAPI.h>
#include "Windowsx.h"
#include <tchar.h>

static ultralight::MouseEvent::Button g_cur_btn = ultralight::MouseEvent::kButton_None;
bool g_is_resizing_modal = false;

#define WINDOW() ((framework::Window*)GetWindowLongPtr(hWnd, GWLP_USERDATA))
#define LISTENER() (WINDOW() ? WINDOW()->listener() : nullptr)

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
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
    g_is_resizing_modal = true;
    break;
  case WM_SIZE:
    if (!g_is_resizing_modal && LISTENER())
      LISTENER()->OnResize(WINDOW()->width(), WINDOW()->height());
    break;
  case WM_EXITSIZEMOVE:
    g_is_resizing_modal = false;
    if (LISTENER())
      LISTENER()->OnResize(WINDOW()->width(), WINDOW()->height());
    break;
  case WM_KEYDOWN:
    if (LISTENER())
      LISTENER()->OnKeyEvent(ultralight::KeyEvent(
      ultralight::KeyEvent::kType_RawKeyDown, wParam, lParam, false));
    break;
  case WM_KEYUP:
    if (LISTENER())
      LISTENER()->OnKeyEvent(ultralight::KeyEvent(
      ultralight::KeyEvent::kType_KeyUp, wParam, lParam, false));
    break;
  case WM_CHAR:
    if (LISTENER())
      LISTENER()->OnKeyEvent(ultralight::KeyEvent(
      ultralight::KeyEvent::kType_Char, wParam, lParam, false));
    break;
  case WM_MOUSEMOVE:
    if (LISTENER())
      LISTENER()->OnMouseEvent(
        { ultralight::MouseEvent::kType_MouseMoved,
          WINDOW()->PixelsToDevice(GET_X_LPARAM(lParam)),
          WINDOW()->PixelsToDevice(GET_Y_LPARAM(lParam)),
          g_cur_btn });
    break;
  case WM_LBUTTONDOWN:
    g_cur_btn = ultralight::MouseEvent::kButton_Left;
    if (LISTENER())
      LISTENER()->OnMouseEvent(
        { ultralight::MouseEvent::kType_MouseDown,
          WINDOW()->PixelsToDevice(GET_X_LPARAM(lParam)), WINDOW()->PixelsToDevice(GET_Y_LPARAM(lParam)), g_cur_btn });
    break;
  case WM_MBUTTONDOWN:
    g_cur_btn = ultralight::MouseEvent::kButton_Middle;
    if (LISTENER())
      LISTENER()->OnMouseEvent(
        { ultralight::MouseEvent::kType_MouseDown,
          WINDOW()->PixelsToDevice(GET_X_LPARAM(lParam)), WINDOW()->PixelsToDevice(GET_Y_LPARAM(lParam)), g_cur_btn });
    break;
  case WM_RBUTTONDOWN:
    g_cur_btn = ultralight::MouseEvent::kButton_Right;
    if (LISTENER())
      LISTENER()->OnMouseEvent(
        { ultralight::MouseEvent::kType_MouseDown,
          WINDOW()->PixelsToDevice(GET_X_LPARAM(lParam)), WINDOW()->PixelsToDevice(GET_Y_LPARAM(lParam)), g_cur_btn });
    break;
  case WM_LBUTTONUP:
    g_cur_btn = ultralight::MouseEvent::kButton_None;
    if (LISTENER())
      LISTENER()->OnMouseEvent(
        { ultralight::MouseEvent::kType_MouseUp,
          WINDOW()->PixelsToDevice(GET_X_LPARAM(lParam)), WINDOW()->PixelsToDevice(GET_Y_LPARAM(lParam)), ultralight::MouseEvent::kButton_Left });
    break;
  case WM_MBUTTONUP:
    g_cur_btn = ultralight::MouseEvent::kButton_None;
    if (LISTENER())
      LISTENER()->OnMouseEvent(
        { ultralight::MouseEvent::kType_MouseUp,
          WINDOW()->PixelsToDevice(GET_X_LPARAM(lParam)), WINDOW()->PixelsToDevice(GET_Y_LPARAM(lParam)), ultralight::MouseEvent::kButton_Middle });
    break;
  case WM_RBUTTONUP:
    g_cur_btn = ultralight::MouseEvent::kButton_None;
    if (LISTENER())
      LISTENER()->OnMouseEvent(
        { ultralight::MouseEvent::kType_MouseUp,
          WINDOW()->PixelsToDevice(GET_X_LPARAM(lParam)), WINDOW()->PixelsToDevice(GET_Y_LPARAM(lParam)), ultralight::MouseEvent::kButton_Right });
    break;
  case WM_MOUSEWHEEL:
    if (LISTENER())
      LISTENER()->OnScrollEvent(
        { ultralight::ScrollEvent::kType_ScrollByPixel, 0, WINDOW()->PixelsToDevice(GET_WHEEL_DELTA_WPARAM(wParam)) });
    break;
  default:
    return DefWindowProc(hWnd, message, wParam, lParam);
  }
  return 0;
}

static HCURSOR g_cursor_hand;
static HCURSOR g_cursor_arrow;
static HCURSOR g_cursor_ibeam;

namespace framework {

class WindowWin : public Window {
 public:
  WindowWin() : hwnd_(nullptr) {}

  virtual ~WindowWin() {}

  bool Initialize(const PlatformWindowConfig& config) {
    HRESULT hr = SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);

    if (hr != S_OK) {
      MessageBoxW(NULL, (LPCWSTR)L"SetProcessDpiAwareness failed", (LPCWSTR)L"Notification", MB_OK);
      return FALSE;
    }

    // Get the DPI for the main monitor, and set the scaling factor
    HMONITOR hMonitor;
    POINT pt = { 1, 1 };
    UINT dpix, dpiy;
    hMonitor = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
    hr = GetDpiForMonitor(hMonitor, MDT_EFFECTIVE_DPI, &dpix, &dpiy);

    if (hr != S_OK) {
      MessageBoxW(NULL, (LPCWSTR)L"GetDpiForMonitor failed", (LPCWSTR)L"Notification", MB_OK);
      return FALSE;
    }

    // Set the current scale from monitor DPI (default on Windows is 96).
    scale_ = (double)dpix / 96.0;
    int width = config.width;
    int height = config.height;
    is_fullscreen_ = config.fullscreen;

    TCHAR* class_name = _T("UltralightWindow");

    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = config.hInstance;
    wcex.hIcon = LoadIcon(config.hInstance, (LPCTSTR)IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = class_name;
    wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_APPLICATION);
    if (!RegisterClassEx(&wcex))
      return false;

    // Create window
    RECT rc = { 0, 0, (LONG)DeviceToPixels(width), (LONG)DeviceToPixels(height) };
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
    hwnd_ = ::CreateWindowEx(NULL
      , class_name
      , _T("")
      , is_fullscreen_ ? (WS_EX_TOPMOST | WS_POPUP) : WS_OVERLAPPEDWINDOW
      , is_fullscreen_ ? 0 : CW_USEDEFAULT
      , is_fullscreen_ ? 0 : CW_USEDEFAULT
      , is_fullscreen_ ? DeviceToPixels(width) : (rc.right - rc.left)
      , is_fullscreen_ ? DeviceToPixels(height) : (rc.bottom - rc.top)
      , NULL
      , NULL
      , config.hInstance
      , NULL);

    if (!hwnd_)
      return false;

    ShowWindow(hwnd_, SW_SHOW);

    SetWindowLongPtr(hwnd_, GWLP_USERDATA, (LONG_PTR)this);

    // Set the thread affinity mask for better clock
    ::SetThreadAffinityMask(::GetCurrentThread(), 1);

    g_cursor_hand = ::LoadCursor(NULL, IDC_HAND);
    g_cursor_arrow = ::LoadCursor(NULL, IDC_ARROW);
    g_cursor_ibeam = ::LoadCursor(NULL, IDC_IBEAM);
    cur_cursor_ = ultralight::kCursor_Hand;

    return true;
  }

  virtual void set_listener(WindowListener* listener) {
    listener_ = listener;
  }

  virtual WindowListener* listener() { return listener_; }

  virtual int width() const { 
    RECT rc;
    ::GetClientRect(hwnd_, &rc);
    return PixelsToDevice(rc.right - rc.left);
  }

  virtual int height() const {
    RECT rc;
    ::GetClientRect(hwnd_, &rc);
    return PixelsToDevice(rc.bottom - rc.top);
  }

  virtual bool is_fullscreen() const { return is_fullscreen_; }

  virtual PlatformWindowHandle handle() const { return hwnd_; }

  virtual double scale() const { return scale_; }

  virtual int DeviceToPixels(int val) const {
    return (int)round(val * scale_);
  }

  virtual int PixelsToDevice(int val) const {
    return (int)round(val / scale_);
  }

  virtual void SetTitle(const char* title) {
    SetWindowTextA(hwnd_, title);
  }

  virtual void SetCursor(ultralight::Cursor cursor) {
    switch (cursor) {
    case ultralight::kCursor_Hand: {
      if (cur_cursor_ != ultralight::kCursor_Hand)
        ::SetCursor(g_cursor_hand);
      break;
    }
    case ultralight::kCursor_Pointer: {
      if (cur_cursor_ != ultralight::kCursor_Pointer)
        ::SetCursor(g_cursor_arrow);
      break;
    }
    case ultralight::kCursor_IBeam: {
      if (cur_cursor_ != ultralight::kCursor_IBeam)
        ::SetCursor(g_cursor_ibeam);
      break;
    }
    };

    cur_cursor_ = cursor;
  }

  virtual void Close() {
    DestroyWindow(hwnd_);
  }

protected:
  bool is_fullscreen_;
  HWND hwnd_;
  double scale_;
  ultralight::Cursor cur_cursor_;
  WindowListener* listener_;
};

std::unique_ptr<Window> Window::Create(const PlatformWindowConfig& config) {
  auto window = std::make_unique<WindowWin>();
  if (window->Initialize(config))
    return std::move(window);

  return nullptr;
}

}  // namespace framework
