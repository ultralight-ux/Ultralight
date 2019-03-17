#pragma once
#include <AppCore/Window.h>
#include <Ultralight/Listener.h>
#include "../RefCountedImpl.h"
#include "../OverlayManager.h"
#import "Cocoa/Cocoa.h"
#import "ViewController.h"
#import "WindowDelegate.h"
#import <MetalKit/MetalKit.h>

namespace ultralight {

class WindowMac : public Window,
                  public RefCountedImpl<WindowMac>,
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

  virtual OverlayManager* overlay_manager() const override { return const_cast<WindowMac*>(this); }

  REF_COUNTED_IMPL(WindowMac);

  void set_app_listener(WindowListener* listener) { app_listener_ = listener; }
                      
  void OnClose();
  
  void OnResize(int width, int height);
                      
  MTKView* view();

protected:
  WindowMac(Monitor* monitor, uint32_t width, uint32_t height,
    bool fullscreen, unsigned int window_flags);

  virtual ~WindowMac();

  friend class Window;

  DISALLOW_COPY_AND_ASSIGN(WindowMac);

  NSWindow* window_;
  ViewController* controller_;
  WindowDelegate* delegate_;
  WindowListener* listener_ = nullptr;
  WindowListener* app_listener_ = nullptr;
  Monitor* monitor_;
  bool is_fullscreen_;
};

}  // namespace ultralight

