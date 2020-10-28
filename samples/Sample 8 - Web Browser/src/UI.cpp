#include "UI.h"

static UI* g_ui = 0;

#define UI_HEIGHT 80

UI::UI(Ref<Window> window) : window_(window) {
  uint32_t window_width = App::instance()->window()->width();
  ui_height_ = (uint32_t)std::round(UI_HEIGHT * window_->scale());
  overlay_ = Overlay::Create(window_, window_width, ui_height_, 0, 0);
  g_ui = this;

  view()->set_load_listener(this);
  view()->set_view_listener(this);
  view()->LoadURL("file:///ui.html");
}

UI::~UI() {
  view()->set_load_listener(nullptr);
  view()->set_view_listener(nullptr);
  g_ui = nullptr;
}

void UI::OnClose() {
}

void UI::OnResize(uint32_t width, uint32_t height) {
  RefPtr<Window> window = App::instance()->window();

  int tab_height = window->height() - ui_height_;

  if (tab_height < 1)
    tab_height = 1;

  overlay_->Resize(window->width(), ui_height_);

  for (auto& tab : tabs_) {
    if (tab.second)
      tab.second->Resize(window->width(), (uint32_t)tab_height);
  }
}

void UI::OnDOMReady(View* caller, uint64_t frame_id, bool is_main_frame, const String& url) {
  // Set the context for all subsequent JS* calls
  Ref<JSContext> locked_context = view()->LockJSContext();
  SetJSContext(locked_context.get());

  JSObject global = JSGlobalObject();
  updateBack = global["updateBack"];
  updateForward = global["updateForward"];
  updateLoading = global["updateLoading"];
  updateURL = global["updateURL"];
  addTab = global["addTab"];
  updateTab = global["updateTab"];
  closeTab = global["closeTab"];

  global["OnBack"] = BindJSCallback(&UI::OnBack);
  global["OnForward"] = BindJSCallback(&UI::OnForward);
  global["OnRefresh"] = BindJSCallback(&UI::OnRefresh);
  global["OnStop"] = BindJSCallback(&UI::OnStop);
  global["OnToggleTools"] = BindJSCallback(&UI::OnToggleTools);
  global["OnRequestNewTab"] = BindJSCallback(&UI::OnRequestNewTab);
  global["OnRequestTabClose"] = BindJSCallback(&UI::OnRequestTabClose);
  global["OnActiveTabChange"] = BindJSCallback(&UI::OnActiveTabChange);
  global["OnRequestChangeURL"] = BindJSCallback(&UI::OnRequestChangeURL);

  CreateNewTab();
}

void UI::OnBack(const JSObject& obj, const JSArgs& args) {
  if (active_tab())
    active_tab()->view()->GoBack();
}

void UI::OnForward(const JSObject& obj, const JSArgs& args) {
  if (active_tab())
    active_tab()->view()->GoForward();
}

void UI::OnRefresh(const JSObject& obj, const JSArgs& args) {
  if (active_tab())
    active_tab()->view()->Reload();
}

void UI::OnStop(const JSObject& obj, const JSArgs& args) {
  if (active_tab())
    active_tab()->view()->Stop();
}

void UI::OnToggleTools(const JSObject& obj, const JSArgs& args) {
  if (active_tab())
    active_tab()->ToggleInspector();
}

void UI::OnRequestNewTab(const JSObject& obj, const JSArgs& args) {
  CreateNewTab();
}

void UI::OnRequestTabClose(const JSObject& obj, const JSArgs& args) {
  if (args.size() == 1) {
    uint64_t id = args[0];

    auto& tab = tabs_[id];
    if (!tab)
      return;

    if (tabs_.size() == 1 && App::instance())
      App::instance()->Quit();

    if (id != active_tab_id_) {
      tabs_[id].reset();
      tabs_.erase(id);
    }
    else {
      tab->set_ready_to_close(true);
    }

    Ref<JSContext> lock(view()->LockJSContext());
    closeTab({ id });
  }
}

void UI::OnActiveTabChange(const JSObject& obj, const JSArgs& args) {
  if (args.size() == 1) {
    uint64_t id = args[0];

    if (id == active_tab_id_)
      return;

    auto& tab = tabs_[id];
    if (!tab)
      return;
      
    tabs_[active_tab_id_]->Hide();

    if (tabs_[active_tab_id_]->ready_to_close()) {
      tabs_[active_tab_id_].reset();
      tabs_.erase(active_tab_id_);
    }

    active_tab_id_ = id;
    tabs_[active_tab_id_]->Show();
      
    auto tab_view = tabs_[active_tab_id_]->view();
    SetLoading(tab_view->is_loading());
    SetCanGoBack(tab_view->CanGoBack());
    SetCanGoForward(tab_view->CanGoBack());
    SetURL(tab_view->url());
  }
}

void UI::OnRequestChangeURL(const JSObject& obj, const JSArgs& args) {
  if (args.size() == 1) {
    ultralight::String url = args[0];

    if (!tabs_.empty()) {
      auto& tab = tabs_[active_tab_id_];
      tab->view()->LoadURL(url);
    }
  }
}

void UI::CreateNewTab() {
  uint64_t id = tab_id_counter_++;
  RefPtr<Window> window = App::instance()->window();
  int tab_height = window->height() - ui_height_;
  if (tab_height < 1)
    tab_height = 1;
  tabs_[id].reset(new Tab(this, id, window->width(), (uint32_t)tab_height, 0, ui_height_));
  tabs_[id]->view()->LoadURL("file:///new_tab_page.html");

  Ref<JSContext> lock(view()->LockJSContext());
  addTab({ id, "New Tab", "", tabs_[id]->view()->is_loading() });
}

RefPtr<View> UI::CreateNewTabForChildView(const String& url) {
  uint64_t id = tab_id_counter_++;
  RefPtr<Window> window = App::instance()->window();
  int tab_height = window->height() - ui_height_;
  if (tab_height < 1)
    tab_height = 1;
  tabs_[id].reset(new Tab(this, id, window->width(), (uint32_t)tab_height, 0, ui_height_));

  Ref<JSContext> lock(view()->LockJSContext());
  addTab({ id, "", url, tabs_[id]->view()->is_loading() });

  return tabs_[id]->view();
}


void UI::UpdateTabTitle(uint64_t id, const ultralight::String& title) {
  Ref<JSContext> lock(view()->LockJSContext());
  updateTab({ id, title, "", tabs_[id]->view()->is_loading() });
}

void UI::UpdateTabURL(uint64_t id, const ultralight::String& url) {
  if (id == active_tab_id_ && !tabs_.empty())
    SetURL(url);
}

void UI::UpdateTabNavigation(uint64_t id, bool is_loading, bool can_go_back, bool can_go_forward) {
  if (tabs_.empty())
    return;

  Ref<JSContext> lock(view()->LockJSContext());
  updateTab({ id, tabs_[id]->view()->title(), "", tabs_[id]->view()->is_loading() });

  if (id == active_tab_id_) {
    SetLoading(is_loading);
    SetCanGoBack(can_go_back);
    SetCanGoForward(can_go_forward);
  }
}

void UI::SetLoading(bool is_loading) {
  Ref<JSContext> lock(view()->LockJSContext());
  updateLoading({ is_loading });
}

void UI::SetCanGoBack(bool can_go_back) {
  Ref<JSContext> lock(view()->LockJSContext());
  updateBack({ can_go_back });
}

void UI::SetCanGoForward(bool can_go_forward) {
  Ref<JSContext> lock(view()->LockJSContext());
  updateForward({ can_go_forward });
}

void UI::SetURL(const ultralight::String& url) {
  Ref<JSContext> lock(view()->LockJSContext());
  updateURL({ url });
}

void UI::SetCursor(ultralight::Cursor cursor) {
  if (App::instance())
    App::instance()->window()->SetCursor(cursor);
}
