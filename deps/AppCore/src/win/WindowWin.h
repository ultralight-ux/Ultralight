#pragma once
#include <AppCore/Window.h>
#include <Ultralight/Listener.h>
#include "../RefCountedImpl.h"
#include "../OverlayManager.h"
#include <Windows.h>

namespace ultralight {

struct WindowData {
  class WindowWin* window;
  MouseEvent::Button cur_btn;
  bool is_resizing_modal;
};

class WindowWin : public Window,
                  public RefCountedImpl<WindowWin>,
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

  virtual OverlayManager* overlay_manager() const { return const_cast<WindowWin*>(this); }

  REF_COUNTED_IMPL(WindowWin);

  HWND hwnd() { return hwnd_; }

  void set_app_listener(WindowListener* listener) { app_listener_ = listener; }

  // These are called by WndProc then forwarded to listener(s)
  void OnClose();
  void OnResize(int width, int height);

protected:
  WindowWin(Monitor* monitor, uint32_t width, uint32_t height,
    bool fullscreen, unsigned int window_flags);

  virtual ~WindowWin();

  friend class Window;

  DISALLOW_COPY_AND_ASSIGN(WindowWin);

  WindowListener* listener_ = nullptr;
  WindowListener* app_listener_ = nullptr;
  Monitor* monitor_;
  bool is_fullscreen_;
  HWND hwnd_;
  HCURSOR cursor_hand_;
  HCURSOR cursor_arrow_;
  HCURSOR cursor_ibeam_;
  Cursor cur_cursor_;
  WindowData window_data_;
};

}  // namespace ultralight
