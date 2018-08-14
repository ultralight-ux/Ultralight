#include "UI.h"

static UI* g_ui = 0;

using namespace std::placeholders;

#define UI_HEIGHT 79

UI::UI(ultralight::Ref<ultralight::Renderer> renderer, ultralight::GPUDriver* driver, framework::Window& window) :
  renderer_(renderer),
  driver_(driver),
  window_(window),
  screen_width_(window.width()),
  ui_height_(UI_HEIGHT),
  tab_height_(window.height() - UI_HEIGHT),
  scale_(window.scale()),
  Overlay(renderer, driver, window.width(), UI_HEIGHT, 0, 0, window.scale()) {
  g_ui = this;

  view()->set_load_listener(this);
  view()->LoadURL("file:///ui.html");
}

UI::~UI() {
  view()->set_load_listener(nullptr);
  g_ui = nullptr;
}

void UI::OnDOMReady(ultralight::View* caller) {
  JSContextRef ctx = view()->js_context();
  func_update_back_ = JSGetFunction(ctx, "updateBack");
  func_update_forward_ = JSGetFunction(ctx, "updateForward");
  func_update_loading_ = JSGetFunction(ctx, "updateLoading");
  func_update_url_ = JSGetFunction(ctx, "updateURL");
  func_add_tab_ = JSGetFunction(ctx, "addTab");
  func_update_tab_ = JSGetFunction(ctx, "updateTab");
  func_close_tab_ = JSGetFunction(ctx, "closeTab");

  callback_helper_ = std::make_unique<JSCallbackHelper>(ctx);
  callback_helper_->Bind("OnBack", std::bind(&UI::OnBack, this, _1, _2, _3));
  callback_helper_->Bind("OnForward", std::bind(&UI::OnForward, this, _1, _2, _3));
  callback_helper_->Bind("OnRefresh", std::bind(&UI::OnRefresh, this, _1, _2, _3));
  callback_helper_->Bind("OnStop", std::bind(&UI::OnStop, this, _1, _2, _3));
  callback_helper_->Bind("OnRequestNewTab", std::bind(&UI::OnRequestNewTab, this, _1, _2, _3));
  callback_helper_->Bind("OnRequestTabClose", std::bind(&UI::OnRequestTabClose, this, _1, _2, _3));
  callback_helper_->Bind("OnActiveTabChange", std::bind(&UI::OnActiveTabChange, this, _1, _2, _3));
  callback_helper_->Bind("OnRequestChangeURL", std::bind(&UI::OnRequestChangeURL, this, _1, _2, _3));

  CreateNewTab();
}

JSValueRef UI::OnBack(JSContextRef ctx, size_t numArgs, const JSValueRef args[]) {
  if (active_tab())
    active_tab()->view()->GoBack();

  return JSValueMakeNull(ctx);
}

JSValueRef UI::OnForward(JSContextRef ctx, size_t numArgs, const JSValueRef args[]) {
  if (active_tab())
    active_tab()->view()->GoForward();

  return JSValueMakeNull(ctx);
}

JSValueRef UI::OnRefresh(JSContextRef ctx, size_t numArgs, const JSValueRef args[]) {
  if (active_tab())
    active_tab()->view()->Reload();

  return JSValueMakeNull(ctx);
}

JSValueRef UI::OnStop(JSContextRef ctx, size_t numArgs, const JSValueRef args[]) {
  if (active_tab())
    active_tab()->view()->Stop();

  return JSValueMakeNull(ctx);
}

JSValueRef UI::OnRequestNewTab(JSContextRef ctx, size_t numArgs, const JSValueRef args[]) {
  CreateNewTab();

  return JSValueMakeNull(ctx);
}

JSValueRef UI::OnRequestTabClose(JSContextRef ctx, size_t numArgs, const JSValueRef args[]) {
  if (numArgs == 1) {
    size_t id = (size_t)JSValueToNumber(ctx, args[0], 0);

    auto& tab = tabs_[id];
    if (!tab)
      return JSValueMakeNull(ctx);

    if (tabs_.size() == 1)
      window_.Close();

    if (id != active_tab_id_) {
      tabs_[id].reset();
      tabs_.erase(id);
    }
    else {
      tab->set_ready_to_close(true);
    }

    CallJS(view()->js_context(), func_close_tab_, (double)id);
  }

  return JSValueMakeNull(ctx);
}

JSValueRef UI::OnActiveTabChange(JSContextRef ctx, size_t numArgs, const JSValueRef args[]) {
  if (numArgs == 1) {
    size_t id = (size_t)JSValueToNumber(ctx, args[0], 0);
    if (id == active_tab_id_)
      return JSValueMakeNull(ctx);

    auto& tab = tabs_[id];
    if (!tab)
      return JSValueMakeNull(ctx);

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

  return JSValueMakeNull(ctx);
}

JSValueRef UI::OnRequestChangeURL(JSContextRef ctx, size_t numArgs, const JSValueRef args[]) {
  if (numArgs == 1) {
    ultralight::String url = FromJSStr(JSValueToStringCopy(ctx, args[0], 0));

    if (!tabs_.empty()) {
      auto& tab = tabs_[active_tab_id_];
      tab->view()->LoadURL(url);
    }
  }

  return JSValueMakeNull(ctx);
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
  tabs_[id] = std::make_unique<Tab>(this, id);
  tabs_[id]->view()->LoadURL("file:///new_tab_page.html");

  JSContextRef ctx = view()->js_context();
  JSValueRef args[] = { JSValueMakeNumber(ctx, (double)id),
    JSValueMakeString(ctx, JSStr("New Tab")),
    JSValueMakeString(ctx, JSStr("")) };
  CallJS(ctx, func_add_tab_, 3, args);
}

void UI::UpdateTabTitle(size_t id, const ultralight::String& title) {
  JSContextRef ctx = view()->js_context();
  JSValueRef args[] = { JSValueMakeNumber(ctx, (double)id),
    JSValueMakeString(ctx, JSStr(title)),
    JSValueMakeString(ctx, JSStr("")) };
  CallJS(ctx, func_update_tab_, 3, args);
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
  CallJS(view()->js_context(), func_update_loading_, is_loading);
}

void UI::SetCanGoBack(bool can_go_back) {
  CallJS(view()->js_context(), func_update_back_, can_go_back);
}

void UI::SetCanGoForward(bool can_go_forward) {
  CallJS(view()->js_context(), func_update_forward_, can_go_forward);
}

void UI::SetURL(const ultralight::String& url) {
  CallJS(view()->js_context(), func_update_url_, url);
}

void UI::SetCursor(ultralight::Cursor cursor) {
  window_.SetCursor(cursor);
}
