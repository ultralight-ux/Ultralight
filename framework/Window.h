#pragma once
#include <memory>
#include "Platform.h"
#include <Ultralight/Listener.h>
#include <Ultralight/KeyEvent.h>
#include <Ultralight/MouseEvent.h>
#include <Ultralight/ScrollEvent.h>

namespace framework {

class WindowListener {
public:
  virtual ~WindowListener() {}

  virtual void OnKeyEvent(const ultralight::KeyEvent& evt) = 0;

  virtual void OnMouseEvent(const ultralight::MouseEvent& evt) = 0;

  virtual void OnScrollEvent(const ultralight::ScrollEvent& evt) = 0;

  virtual void OnResize(int width, int height) = 0;
};

class Window {
public:
  virtual ~Window() {}

  static std::unique_ptr<Window> Create(const PlatformWindowConfig& config);

  virtual void set_listener(WindowListener* listener) = 0;

  virtual WindowListener* listener() = 0;

  virtual int width() const = 0;

  virtual int height() const = 0;

  virtual bool is_fullscreen() const = 0;

  virtual PlatformWindowHandle handle() const = 0;

  virtual double scale() const = 0;

  virtual int DeviceToPixels(int val) const = 0;

  virtual int PixelsToDevice(int val) const = 0;

  virtual void SetTitle(const char* title) = 0;

  virtual void SetCursor(ultralight::Cursor cursor) = 0;

  virtual void Close() = 0;
};

}  // namespace framework
