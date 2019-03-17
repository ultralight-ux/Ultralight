#include "UI.h"

static UI* g_ui = 0;

#define UI_HEIGHT 79

UI::UI(Ref<Window> window) : window_(window) {
  int window_width = App::instance()->window()->width();
  overlay_ = Overlay::Create(window_, window_width, UI_HEIGHT, 0, 0);
  g_ui = this;

  view()->set_load_listener(this);
  view()->LoadURL("file:///ui.html");
}

UI::~UI() {
  view()->set_load_listener(nullptr);
  g_ui = nullptr;
}

void UI::OnClose() {
}

void UI::OnResize(int width, int height) {
  RefPtr<Window> window = App::instance()->window();
  int tab_height = window->height() - UI_HEIGHT;
  overlay_->Resize(window->width(), UI_HEIGHT);
  for (auto& tab : tabs_) {
    if (tab.second)
      tab.second->Resize(window->width(), tab_height);
  }
}

void UI::OnDOMReady(ultralight::View* caller) {
  // Set the context for all subsequent JS* calls
  SetJSContext(view()->js_context());

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
      
    tabs_[active_tab_id_]->overlay()->Hide();
    tabs_[active_tab_id_]->overlay()->Unfocus();

    if (tabs_[active_tab_id_]->ready_to_close()) {
      tabs_[active_tab_id_].reset();
      tabs_.erase(active_tab_id_);
    }

    active_tab_id_ = id;
    tabs_[active_tab_id_]->overlay()->Show();
    tabs_[active_tab_id_]->overlay()->Focus();
      
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
  tabs_[id].reset(new Tab(this, id, window->width(), window->height() - UI_HEIGHT, 0, UI_HEIGHT));
  tabs_[id]->view()->LoadURL("file:///new_tab_page.html");

  addTab({ id, "New Tab", "" });
}

void UI::UpdateTabTitle(uint64_t id, const ultralight::String& title) {
  updateTab({ id, title, "" });
}

void UI::UpdateTabURL(uint64_t id, const ultralight::String& url) {
  if (id == active_tab_id_ && !tabs_.empty())
    SetURL(url);
}

void UI::UpdateTabNavigation(uint64_t id, bool is_loading, bool can_go_back, bool can_go_forward) {
  if (id == active_tab_id_ && !tabs_.empty()) {
    SetLoading(is_loading);
    SetCanGoBack(can_go_back);
    SetCanGoForward(can_go_forward);
  }
}

void UI::SetLoading(bool is_loading) {
  updateLoading({ is_loading });
}

void UI::SetCanGoBack(bool can_go_back) {
  updateBack({ can_go_back });
}

void UI::SetCanGoForward(bool can_go_forward) {
  updateForward({ can_go_forward });
}

void UI::SetURL(const ultralight::String& url) {
  updateURL({ url });
}

void UI::SetCursor(ultralight::Cursor cursor) {
  if (App::instance())
    App::instance()->window()->SetCursor(cursor);
}
