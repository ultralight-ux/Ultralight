///
/// @file View.h
///
/// @brief The header for the View class.
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
#include <JavaScriptCore/JavaScript.h>
#include <Ultralight/Defines.h>
#include <Ultralight/RefPtr.h>
#include <Ultralight/KeyEvent.h>
#include <Ultralight/MouseEvent.h>
#include <Ultralight/ScrollEvent.h>
#include <Ultralight/RenderTarget.h>
#include <Ultralight/Bitmap.h>
#include <Ultralight/Listener.h>

namespace ultralight {

///
/// @brief A View is similar to a tab in a browser-- you load web content into
///	       it and display it however you want. @see Renderer::CreateView
///
/// @note  The API is currently not thread-safe, all calls must be made on the
///        main thread.
///
class UExport View : public RefCounted {
public:
  ///
  /// Get the URL of the current page loaded into this View, if any.
  ///
  virtual String url() = 0;

  ///
  /// Get the title of the current page loaded into this View, if any.
  ///
  virtual String title() = 0;

  ///
  /// Check if the main frame of the page is currently loading.
  ///
  virtual bool is_loading() = 0;

  ///
  /// Get the RenderTarget for the View.
  ///
  /// @note  The RenderTarget is only used when you define a custom GPUDriver.
  ///
  ///        During each call to Renderer::Render, each View will be drawn to
  ///        an offscreen texture/render-buffer. You should use RenderTarget
  ///        to obtain the internal texture ID for the View and then display
  ///        it in some quad (eg, call  GPUDriver::BindTexture with the texture
  ///        ID, then draw a textured quad in your 3D engine, making sure to
  ///        use the UV coords specified in the RenderTarget).
  ///
  ///        @see Overlay.cpp within the AppCore source for an example.
  ///
  virtual RenderTarget render_target() = 0;

  ///
  /// Check if bitmap is dirty (has changed since last call to View::bitmap)
  ///
  /// @note  Only valid when using the default, offscreen GPUDriver.
  ///
  virtual bool is_bitmap_dirty() = 0;

  ///
  /// Get the bitmap for the View (calling this resets the dirty state).
  ///
  // @note  Only valid when using the default, offscreen GPUDriver.
  ///
  virtual RefPtr<Bitmap> bitmap() = 0;

  ///
  /// Load a raw string of HTML, the View will navigate to it as a new page.
  ///
  virtual void LoadHTML(const String& html) = 0;

  ///
  /// Load a URL, the View will navigate to it as a new page.
  ///
  /// @note  You can use File URLs (eg, file:///page.html) but you must define
  ///        your own FileSystem implementation. @see Platform::set_file_system
  ///
  virtual void LoadURL(const String& url) = 0;

  ///
  /// Resize View to a certain size.
  ///
  /// @param  width   The initial width, in device coordinates.
  /// 
  /// @param  height  The initial height, in device coordinates.
  ///
  /// @note  The device coordinates are scaled to pixels by multiplying them
  ///        with the current DPI scale (@see Config::device_scale_hint) and
  ///        rounding to the nearest integer value.
  ///
  virtual void Resize(uint32_t width, uint32_t height) = 0;

  ///
  /// Get the page's JSContext for use with the JavaScriptCore API
  ///
  /// @note  We expose the entire JavaScriptCore API to users for maximum
  ///        flexibility. Each page has its own JSContext that gets reset
  ///        after each page navigation. You should make all your initial
  ///        JavaScript calls to the page within the DOMReady event,
  ///        @see ViewListener::OnDOMReady
  ///
  virtual JSContextRef js_context() = 0;

  ///
  /// Evaluate a raw string of JavaScript and return results as a native
  /// JavaScriptCore JSValueRef (@see <JavaScriptCore/JSValueRef.h>)
  ///
  /// @note  This is just a wrapper for JSEvaluateScript() in JavaScriptCore
  ///
  virtual JSValueRef EvaluateScript(const String& script) = 0;

  ///
  /// Whether or not we can navigate backwards in history
  ///
  virtual bool CanGoBack() = 0;

  ///
  /// Whether or not we can navigate forwards in history
  ///
  virtual bool CanGoForward() = 0;

  ///
  /// Navigate backwards in history
  ///
  virtual void GoBack() = 0;

  ///
  /// Navigate forwards in history
  ///
  virtual void GoForward() = 0;

  ///
  /// Navigate to an arbitrary offset in history
  ///
  virtual void GoToHistoryOffset(int offset) = 0;

  ///
  /// Reload current page
  ///
  virtual void Reload() = 0;

  ///
  /// Stop all page loads
  ///
  virtual void Stop() = 0;
  
  ///
  /// Fire a keyboard event
  ///
  /// @note  Only 'Char' events actually generate text in input fields.
  ///
  virtual void FireKeyEvent(const KeyEvent& evt) = 0;

  ///
  /// Fire a mouse event
  ///
  virtual void FireMouseEvent(const MouseEvent& evt) = 0;

  ///
  /// Fire a scroll event
  ///
  virtual void FireScrollEvent(const ScrollEvent& evt) = 0;

  ///
  /// Set a ViewListener to receive callbacks for View-related events.
  ///
  /// @note  Ownership remains with the caller.
  ///
  virtual void set_view_listener(ViewListener* listener) = 0;
  
  ///
  /// Get the active ViewListener, if any
  ///
  virtual ViewListener* view_listener() const = 0;

  ///
  /// Set a LoadListener to receive callbacks for Load-related events.
  ///
  /// @note  Ownership remains with the caller.
  ///
  virtual void set_load_listener(LoadListener* listener) = 0;

  ///
  /// Get the active LoadListener, if any
  ///
  virtual LoadListener* load_listener() const = 0;

  ///
  /// Set whether or not this View should be repainted during the next
  /// call to Renderer::Render
  ///
  /// @note  This flag is automatically set whenever the page content changes
  ///        but you can set it directly in case you need to force a repaint.
  ///
  virtual void set_needs_paint(bool needs_paint) = 0;

  ///
  /// Whether or not this View should be repainted during the next call to
  /// Renderer::Render.
  ///
  virtual bool needs_paint() const = 0;

protected:
  virtual ~View();
};

}  // namespace ultralight
