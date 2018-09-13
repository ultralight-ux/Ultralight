#pragma once
#include <Framework/Overlay.h>
#include <Framework/Window.h>
#include <Framework/JSHelpers.h>
#include "Tab.h"
#include <map>
#include <memory>

using framework::JSObject;
using framework::JSArgs;
using framework::JSFunction;

/**
* Browser UI implementation. Renders the toolbar/addressbar/tabs in top pane.
*/
class UI : public framework::Overlay,
           public ultralight::LoadListener {
public:
  UI(ultralight::Ref<ultralight::Renderer> renderer, ultralight::GPUDriver* driver, framework::Window& window);
  ~UI();

  // Inherited from ultralight::Listener::Load
  virtual void OnDOMReady(ultralight::View* caller) override;

  // Called by UI JavaScript
  void OnBack(const JSObject& obj, const JSArgs& args);
  void OnForward(const JSObject& obj, const JSArgs& args);
  void OnRefresh(const JSObject& obj, const JSArgs& args);
  void OnStop(const JSObject& obj, const JSArgs& args);
  void OnRequestNewTab(const JSObject& obj, const JSArgs& args);
  void OnRequestTabClose(const JSObject& obj, const JSArgs& args);
  void OnActiveTabChange(const JSObject& obj, const JSArgs& args);
  void OnRequestChangeURL(const JSObject& obj, const JSArgs& args);

  // Inhrerited from Overlay, we may dispatch input to a focused tab instead
  virtual void Draw() override;
  virtual void FireKeyEvent(const ultralight::KeyEvent& evt) override;
  virtual void FireMouseEvent(const ultralight::MouseEvent& evt) override;
  virtual void FireScrollEvent(const ultralight::ScrollEvent& evt) override;
  virtual void Resize(int width, int height) override;

protected:
  void CreateNewTab();
  void UpdateTabTitle(size_t id, const ultralight::String& title);
  void UpdateTabURL(size_t id, const ultralight::String& url);
  void UpdateTabNavigation(size_t id, bool is_loading, bool can_go_back, bool can_go_forward);

  void SetLoading(bool is_loading);
  void SetCanGoBack(bool can_go_back);
  void SetCanGoForward(bool can_go_forward);
  void SetURL(const ultralight::String& url);
  void SetCursor(ultralight::Cursor cursor);

  Tab* active_tab() { return tabs_.empty() ? nullptr : tabs_[active_tab_id_].get(); }

  ultralight::Ref<ultralight::Renderer> renderer_;
  ultralight::GPUDriver* driver_;
  framework::Window& window_;
  int screen_width_;
  int ui_height_;
  int tab_height_;
  float scale_;

  std::map<size_t, std::unique_ptr<Tab>> tabs_;
  size_t active_tab_id_ = 0;
  size_t tab_id_counter_ = 0;
  bool tab_has_focus_ = false;
  ultralight::Cursor cur_cursor_;

  JSFunction updateBack;
  JSFunction updateForward;
  JSFunction updateLoading;
  JSFunction updateURL;
  JSFunction addTab;
  JSFunction updateTab;
  JSFunction closeTab;

  friend class Tab;
};
