// Copyright 2018 Ultralight, Inc. All rights reserved.
#pragma once
#include <JavaScriptCore/JavaScript.h>
#include <Ultralight/Defines.h>
#include <Ultralight/RefPtr.h>
#include <Ultralight/KeyEvent.h>
#include <Ultralight/MouseEvent.h>
#include <Ultralight/ScrollEvent.h>
#include <Ultralight/RenderTarget.h>
#include <Ultralight/Listener.h>

namespace ultralight {

/**
 * View for displaying web content.
 */
class UExport View : public RefCounted {
public:
  /*************************
   * Getters               *
   *************************/

  // Current page URL
  virtual String url() = 0;

  // Current page title
  virtual String title() = 0;

  // Check if main frame is loading
  virtual bool is_loading() = 0;

  // Get RenderTarget for display purposes
  virtual RenderTarget render_target() = 0;

  /*************************
   * Loading               *
   *************************/

  // Load a raw string of HTML
  virtual void LoadHTML(const String& html) = 0;

  // Load a URL into main frame
  virtual void LoadURL(const String& url) = 0;

  /*************************
  * Resizing               *
  *************************/

  // Resize view to a certain width and height
  virtual void Resize(uint32_t width, uint32_t height) = 0;

  /*************************
   * Scripting             *
   *************************/

  // Get the page's JSContext for use with JavaScriptCore API
  virtual JSContextRef js_context() = 0;

  // Evaluate raw string of JavaScript and return result
  virtual JSValueRef EvaluateScript(const String& script) = 0;

  /*************************
   * History               *
   *************************/

  // Can navigate backwards in history
  virtual bool CanGoBack() = 0;

  // Can navigate forwards in history
  virtual bool CanGoForward() = 0;

  // Navigate backwards in history
  virtual void GoBack() = 0;

  // Navigate forwards in history
  virtual void GoForward() = 0;

  // Navigate to arbitrary offset in history
  virtual void GoToHistoryOffset(int offset) = 0;

  // Reload current page
  virtual void Reload() = 0;

  // Stop all page loads
  virtual void Stop() = 0;
  
  /*************************
   * Input Events          *
   *************************/

  // Fire a keyboard event
  virtual void FireKeyEvent(const KeyEvent& evt) = 0;

  // Fire a mouse event
  virtual void FireMouseEvent(const MouseEvent& evt) = 0;

  // Fire a scroll event
  virtual void FireScrollEvent(const ScrollEvent& evt) = 0;

  /*************************
   * Listeners              
   *************************/

  // Set/Get view event listener
  virtual void set_view_listener(ViewListener* listener) = 0;
  virtual ViewListener* view_listener() const = 0;

  // Set/Get load event listener
  virtual void set_load_listener(LoadListener* listener) = 0;
  virtual LoadListener* load_listener() const = 0;

protected:
  virtual ~View();
};

}  // namespace ultralight
