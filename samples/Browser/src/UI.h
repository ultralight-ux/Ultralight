#pragma once
#include <AppCore/AppCore.h>
#include "Tab.h"
#include <map>
#include <memory>

using ultralight::JSObject;
using ultralight::JSArgs;
using ultralight::JSFunction;
using namespace ultralight;

class Console;

/**
* Browser UI implementation. Renders the toolbar/addressbar/tabs in top pane.
*/
class UI : public WindowListener,
           public LoadListener,
           public ViewListener {
 public:
  UI(Ref<Window> window);
  ~UI();
               
  // Inherited from WindowListener
  virtual void OnClose() override;
  virtual void OnResize(uint32_t width, uint32_t height) override;

  // Inherited from LoadListener
  virtual void OnDOMReady(View* caller, uint64_t frame_id,
    bool is_main_frame, const String& url) override;

  // Inherited from ViewListener
  virtual void OnChangeCursor(ultralight::View* caller, Cursor cursor) override { SetCursor(cursor); }

  // Called by UI JavaScript
  void OnBack(const JSObject& obj, const JSArgs& args);
  void OnForward(const JSObject& obj, const JSArgs& args);
  void OnRefresh(const JSObject& obj, const JSArgs& args);
  void OnStop(const JSObject& obj, const JSArgs& args);
  void OnToggleTools(const JSObject& obj, const JSArgs& args);
  void OnRequestNewTab(const JSObject& obj, const JSArgs& args);
  void OnRequestTabClose(const JSObject& obj, const JSArgs& args);
  void OnActiveTabChange(const JSObject& obj, const JSArgs& args);
  void OnRequestChangeURL(const JSObject& obj, const JSArgs& args);

protected:
  void CreateNewTab();
  RefPtr<View> CreateNewTabForChildView(const String& url);
  void UpdateTabTitle(uint64_t id, const String& title);
  void UpdateTabURL(uint64_t id, const String& url);
  void UpdateTabNavigation(uint64_t id, bool is_loading, bool can_go_back, bool can_go_forward);

  void SetLoading(bool is_loading);
  void SetCanGoBack(bool can_go_back);
  void SetCanGoForward(bool can_go_forward);
  void SetURL(const String& url);
  void SetCursor(Cursor cursor);

  Tab* active_tab() { return tabs_.empty() ? nullptr : tabs_[active_tab_id_].get(); }
               
  Ref<View> view() { return overlay_->view(); }

  Ref<Window> window_;
  RefPtr<Overlay> overlay_;
  int ui_height_;
  int tab_height_;
  float scale_;

  std::map<uint64_t, std::unique_ptr<Tab>> tabs_;
  uint64_t active_tab_id_ = 0;
  uint64_t tab_id_counter_ = 0;
  Cursor cur_cursor_;

  JSFunction updateBack;
  JSFunction updateForward;
  JSFunction updateLoading;
  JSFunction updateURL;
  JSFunction addTab;
  JSFunction updateTab;
  JSFunction closeTab;

  friend class Tab;
};
