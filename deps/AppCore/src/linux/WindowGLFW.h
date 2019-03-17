#pragma once
#include <AppCore/Window.h>
#include <Ultralight/Listener.h>
#include "../RefCountedImpl.h"
#include "../OverlayManager.h"
#include <cmath>

typedef struct GLFWwindow GLFWwindow;
typedef struct GLFWcursor GLFWcursor;

namespace ultralight {

class WindowGLFW : public Window,
                  public RefCountedImpl<WindowGLFW>,
                  public OverlayManager {
public:
  virtual void set_listener(WindowListener* listener) override { listener_ = listener; }

  virtual WindowListener* listener() override { return listener_; }

  virtual int width() const override;

  virtual int height() const override;

  virtual bool is_fullscreen() const override { return is_fullscreen_; }

  virtual double scale() const override;

  virtual void SetTitle(const char* title) override;

  virtual void SetCursor(ultralight::Cursor cursor) override;

  virtual void Close() override;

  virtual int DeviceToPixels(int val) const override {
    return (int)round(val * scale());
  }

  virtual int PixelsToDevice(int val) const override {
    return (int)round(val / scale());
  }

  virtual OverlayManager* overlay_manager() const { return const_cast<WindowGLFW*>(this); }

  REF_COUNTED_IMPL(WindowGLFW);

  void set_app_listener(WindowListener* listener) { app_listener_ = listener; }

  void OnClose();
  void OnResize(int width, int height);

protected:
  WindowGLFW(Monitor* monitor, uint32_t width, uint32_t height,
    bool fullscreen, unsigned int window_flags);

  virtual ~WindowGLFW();

  GLFWwindow* handle() { return window_; }

  friend class Window;
  friend class AppGLFW;

  DISALLOW_COPY_AND_ASSIGN(WindowGLFW);

  GLFWwindow* window_ = nullptr;
  WindowListener* listener_ = nullptr;
  WindowListener* app_listener_ = nullptr;
  bool is_fullscreen_;
  Monitor* monitor_;
  GLFWcursor* cursor_ibeam_;
  GLFWcursor* cursor_crosshair_;
  GLFWcursor* cursor_hand_;
  GLFWcursor* cursor_hresize_;
  GLFWcursor* cursor_vresize_;
};

}  // namespace ultralight
