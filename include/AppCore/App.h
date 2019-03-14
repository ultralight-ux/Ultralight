///
/// @file App.h
///
/// @brief The header for the App class.
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
#include <Ultralight/Renderer.h>

namespace ultralight {

class Monitor;
class Window;

///
/// Interface for all App-related events. @see App::set_listener
///
class AExport AppListener {
public:
  virtual ~AppListener() {}

  ///
  /// Called whenever the App updates. You should update all app logic here.
  ///
  /// @note  This event is fired right before the run loop calls
  ///        Renderer::Update and Renderer::Render.
  ///
  virtual void OnUpdate() {}
};

///
/// Main application class.
///
class AExport App : public RefCounted {
public:
  ///
  /// Create the App singleton.
  ///
  /// @note  You should only create one of these per application lifetime.
  ///        
  ///        App maintains its own Renderer instance, make sure to set your
  ///        Config before creating App. (@see Platform::set_config)
  ///
  static Ref<App> Create();

  ///
  /// Get the App singleton.
  ///
  static App* instance();

  ///
  /// Set the main window. You must set this before calling Run.
  ///
  /// @param  window  The window to use for all rendering.
  ///
  /// @note  We currently only support one Window per App, this will change
  ///        later once we add support for multiple driver instances.
  ///
  virtual void set_window(Ref<Window> window) = 0;

  ///
  /// Get the main window.
  ///
  virtual RefPtr<Window> window() = 0;

  ///
  /// Set an AppListener to receive callbacks for app-related events.
  ///
  /// @note  Ownership remains with the caller.
  ///
  virtual void set_listener(AppListener* listener) = 0;

  ///
  /// Get the AppListener, if any.
  ///
  virtual AppListener* listener() = 0;

  ///
  /// Whether or not the App is running.
  ///
  virtual bool is_running() const = 0;

  ///
  /// Get the main monitor (this is never NULL).
  ///
  /// @note  We'll add monitor enumeration later.
  ///
  virtual Monitor* main_monitor() = 0;

  ///
  /// Get the underlying Renderer instance.
  ///
  virtual Ref<Renderer> renderer() = 0;

  ///
  /// Run the main loop.
  ///
  /// @note  Make sure to call set_window before calling this.
  ///
  virtual void Run() = 0;

  ///
  /// Quit the application.
  ///
  virtual void Quit() = 0;

protected:
  virtual ~App();
};

}  // namespace ultralight