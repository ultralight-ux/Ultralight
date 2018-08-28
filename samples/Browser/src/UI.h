#pragma once
#include <Framework/Overlay.h>
#include <Framework/Window.h>
#include <Framework/JSHelpers.h>
#include "Tab.h"
#include <map>
#include <memory>

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
  JSValueRef OnBack(JSContextRef ctx, size_t numArgs, const JSValueRef args[]);
  JSValueRef OnForward(JSContextRef ctx, size_t numArgs, const JSValueRef args[]);
  JSValueRef OnRefresh(JSContextRef ctx, size_t numArgs, const JSValueRef args[]);
  JSValueRef OnStop(JSContextRef ctx, size_t numArgs, const JSValueRef args[]);
  JSValueRef OnRequestNewTab(JSContextRef ctx, size_t numArgs, const JSValueRef args[]);
  JSValueRef OnRequestTabClose(JSContextRef ctx, size_t numArgs, const JSValueRef args[]);
  JSValueRef OnActiveTabChange(JSContextRef ctx, size_t numArgs, const JSValueRef args[]);
  JSValueRef OnRequestChangeURL(JSContextRef ctx, size_t numArgs, const JSValueRef args[]);

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

  std::unique_ptr<JSCallbackHelper> callback_helper_;

  JSObjectRef func_update_back_ = nullptr;
  JSObjectRef func_update_forward_ = nullptr;
  JSObjectRef func_update_loading_ = nullptr;
  JSObjectRef func_update_url_ = nullptr;
  JSObjectRef func_add_tab_ = nullptr;
  JSObjectRef func_update_tab_ = nullptr;
  JSObjectRef func_close_tab_ = nullptr;

  friend class Tab;
};
