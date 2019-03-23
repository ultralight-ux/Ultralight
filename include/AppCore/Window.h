///
/// @file Window.h
///
/// @brief The header for the Window class.
///
/// @author
///
/// This file is a part of Ultralight, a fast, lightweight, HTML UI engine
///
/// Website: <http://ultralig.ht>
///
/// Copyright (C) 2019 Ultralight, Inc. All rights reserved.
///
#pragma once
#include "Defines.h"
#include <Ultralight/RefPtr.h>
#include <Ultralight/Listener.h>

namespace ultralight {

class Monitor;
class OverlayManager;

///
/// Interface for all Window-related events. @see Window::set_listener
///
class WindowListener {
public:
  virtual ~WindowListener() {}

  ///
  /// Called when the Window is closed.
  ///
  virtual void OnClose() = 0;

  ///
  /// Called when the Window is resized.
  ///
  /// @param  width   The new width (in device coordinates).
  ///
  /// @param  height  The new height (in device coordinates).
  ///
  virtual void OnResize(uint32_t width, uint32_t height) = 0;
};

///
/// Window creation flags. @see Window::Create
///
enum WindowFlags : uint8_t {
  kWindowFlags_Borderless  = 1 << 0,
  kWindowFlags_Titled      = 1 << 1,
  kWindowFlags_Resizable   = 1 << 2,
  kWindowFlags_Maximizable = 1 << 3,
};

///
/// Window class, represents a platform window.
///
class AExport Window : public RefCounted {
public:
  ///
  /// Create a new Window.
  ///
  /// @param  monitor       The monitor to create the Window on.
  ///
  /// @param  width         The width (in device coordinates).
  ///
  /// @param  height        The height (in device coordinates).
  ///
  /// @param  fullscreen    Whether or not the window is fullscreen.
  ///
  /// @param  window_flags  Various window flags.
  ///
  static Ref<Window> Create(Monitor* monitor, uint32_t width, uint32_t height,
    bool fullscreen, unsigned int window_flags);

  ///
  /// Set a WindowListener to receive callbacks for window-related events.
  ///
  /// @note  Ownership remains with the caller.
  ///
  virtual void set_listener(WindowListener* listener) = 0;

  ///
  /// Get the WindowListener, if any.
  ///
  virtual WindowListener* listener() = 0;

  ///
  /// Get the window width (in device coordinates).
  ///
  virtual uint32_t width() const = 0;

  ///
  /// Get the window height (in device coordinates).
  ///
  virtual uint32_t height() const = 0;

  ///
  /// Whether or not the window is fullscreen.
  ///
  virtual bool is_fullscreen() const = 0;

  ///
  /// The DPI scale of the window.
  ///
  virtual double scale() const = 0;

  ///
  /// Set the window title.
  ///
  virtual void SetTitle(const char* title) = 0;

  ///
  /// Set the cursor.
  ///
  virtual void SetCursor(ultralight::Cursor cursor) = 0;

  ///
  /// Close the window.
  ///
  virtual void Close() = 0;

  ///
  /// Convert device coordinates to pixels using the current DPI scale.
  ///
  virtual int DeviceToPixels(int val) const = 0;

  ///
  /// Convert pixels to device coordinates using the current DPI scale.
  ///
  virtual int PixelsToDevice(int val) const = 0;

protected:
  virtual ~Window();
  virtual OverlayManager* overlay_manager() const = 0;

  friend class OverlayImpl;
};

}  // namespace ultralight
