#pragma once
#include <Ultralight/Ultralight.h>

typedef struct GLFWwindow GLFWwindow;
typedef struct GLFWcursor GLFWcursor;

using namespace ultralight;

class WindowListener {
public:
  virtual ~WindowListener() {}

  virtual void OnClose() = 0;
  virtual void OnResize(uint32_t width, uint32_t height) = 0;
  virtual void OnChangeFocus(bool focused) = 0;
  virtual void OnKeyEvent(const KeyEvent& evt) = 0;
  virtual void OnMouseEvent(const MouseEvent& evt) = 0;
  virtual void OnScrollEvent(const ScrollEvent& evt) = 0;
};

class Window {
public:
  Window(uint32_t width, uint32_t height, bool enable_vsync);
  ~Window();

  void set_listener(WindowListener* listener) { listener_ = listener; }
  WindowListener* listener() { return listener_; }

  void MoveToCenter();

  uint32_t width() const;
  uint32_t height() const;
  double scale() const;

  void SetTitle(const char* title);
  void SetCursor(ultralight::Cursor cursor);
  void Close();

  int DeviceToPixels(int val) const;
  int PixelsToDevice(int val) const;

  void PresentFrame();

  GLFWwindow* handle() { return window_; }

protected:
  WindowListener* listener_ = nullptr;
  GLFWwindow* window_ = nullptr;
  GLFWcursor* cursor_ibeam_;
  GLFWcursor* cursor_crosshair_;
  GLFWcursor* cursor_hand_;
  GLFWcursor* cursor_hresize_;
  GLFWcursor* cursor_vresize_;
};
