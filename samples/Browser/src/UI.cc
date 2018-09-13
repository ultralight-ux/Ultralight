#include "UI.h"

static UI* g_ui = 0;

using namespace framework;

#define UI_HEIGHT 79

UI::UI(ultralight::Ref<ultralight::Renderer> renderer, ultralight::GPUDriver* driver, framework::Window& window) :
  renderer_(renderer),
  driver_(driver),
  window_(window),
  screen_width_(window.width()),
  ui_height_(UI_HEIGHT),
  tab_height_(window.height() - UI_HEIGHT),
  scale_((float)window.scale()),
  Overlay(renderer, driver, window.width(), UI_HEIGHT, 0, 0, (float)window.scale()) {
  g_ui = this;

  view()->set_load_listener(this);
  view()->LoadURL("file:///ui.html");
}

UI::~UI() {
  view()->set_load_listener(nullptr);
  g_ui = nullptr;
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
    size_t id = args[0];

    auto& tab = tabs_[id];
    if (!tab)
      return;

    if (tabs_.size() == 1)
      window_.Close();

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
    size_t id = args[0];

    if (id == active_tab_id_)
      return;

    auto& tab = tabs_[id];
    if (!tab)
      return;

    if (tabs_[active_tab_id_]->ready_to_close()) {
      tabs_[active_tab_id_].reset();
      tabs_.erase(active_tab_id_);
    }

    active_tab_id_ = id;
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

void UI::Draw() {
  Overlay::Draw();

  if (!tabs_.empty())
    tabs_[active_tab_id_]->Draw();
}

void UI::FireKeyEvent(const ultralight::KeyEvent& evt) {
  if (tab_has_focus_ && !tabs_.empty())
    tabs_[active_tab_id_]->FireKeyEvent(evt);
  else
    Overlay::FireKeyEvent(evt);
}

void UI::FireMouseEvent(const ultralight::MouseEvent& evt) {
  if (evt.type == ultralight::MouseEvent::kType_MouseMoved) {
    if (!tabs_.empty())
      tabs_[active_tab_id_]->FireMouseEvent(evt);

    Overlay::FireMouseEvent(evt);
    return;
  } else if (evt.type == ultralight::MouseEvent::kType_MouseDown &&
             evt.button == ultralight::MouseEvent::kButton_Left) {
    // Do Hit-Testing and Focus UI or Tab
    tab_has_focus_ = evt.y > ui_height_;
  }

  if (tab_has_focus_ && !tabs_.empty())
    tabs_[active_tab_id_]->FireMouseEvent(evt);
  else
    Overlay::FireMouseEvent(evt);
}

void UI::FireScrollEvent(const ultralight::ScrollEvent& evt) {
  if (!tabs_.empty())
    tabs_[active_tab_id_]->FireScrollEvent(evt);
}

void UI::Resize(int width, int height) {
  screen_width_ = window_.width();
  tab_height_ = window_.height() - UI_HEIGHT;
  Overlay::Resize(window_.width(), UI_HEIGHT);
  for (auto& tab : tabs_) {
    if (tab.second)
      tab.second->Resize(screen_width_, tab_height_);
  }
}

void UI::CreateNewTab() {
  size_t id = tab_id_counter_++;
  tabs_[id].reset(new Tab(this, id));
  tabs_[id]->view()->LoadURL("file:///new_tab_page.html");

  addTab({ id, "New Tab", "" });
}

void UI::UpdateTabTitle(size_t id, const ultralight::String& title) {
  updateTab({ id, title, "" });
}

void UI::UpdateTabURL(size_t id, const ultralight::String& url) {
  if (id == active_tab_id_ && !tabs_.empty())
    SetURL(url);
}

void UI::UpdateTabNavigation(size_t id, bool is_loading, bool can_go_back, bool can_go_forward) {
  if (id == active_tab_id_ && !tabs_.empty()) {
    SetLoading(is_loading);
    SetCanGoBack(can_go_back);
    SetCanGoForward(can_go_forward);
  }
}

void UI::SetLoading(bool is_loading) {
  JSContextRef ctx = view()->js_context();
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
  window_.SetCursor(cursor);
}
