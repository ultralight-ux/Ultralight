#pragma once
#include "Defines.h"
#include <Ultralight/RefPtr.h>
#include <Ultralight/Listener.h>

namespace ultralight {

class Monitor;
class OverlayManager;

class WindowListener {
public:
  virtual ~WindowListener() {}

  virtual void OnClose() = 0;
  virtual void OnResize(int width, int height) = 0;
};

enum WindowFlags : uint8_t {
  kWindowFlags_Borderless  = 1 << 0,
  kWindowFlags_Titled      = 1 << 1,
  kWindowFlags_Resizable   = 1 << 2,
  kWindowFlags_Maximizable = 1 << 3,
};

class AExport Window : public RefCounted {
public:
  static Ref<Window> Create(Monitor* monitor, uint32_t width, uint32_t height,
    bool fullscreen, unsigned int window_flags);

  virtual void set_listener(WindowListener* listener) = 0;

  virtual WindowListener* listener() = 0;

  virtual int width() const = 0;

  virtual int height() const = 0;

  virtual bool is_fullscreen() const = 0;

  virtual double scale() const = 0;

  virtual void SetTitle(const char* title) = 0;

  virtual void SetCursor(ultralight::Cursor cursor) = 0;

  virtual void Close() = 0;

  virtual int DeviceToPixels(int val) const = 0;

  virtual int PixelsToDevice(int val) const = 0;

protected:
  virtual ~Window();
  virtual OverlayManager* overlay_manager() = 0;

  friend class OverlayImpl;
};

}  // namespace ultralight
