#include "Tab.h"
#include "UI.h"
#include <iostream>
#include <string>
#if _WIN32
#include <Windows.h>
#endif

Tab::Tab(UI* ui, uint64_t id, int width, int height, int x, int y) : ui_(ui), id_(id) {
  overlay_ = Overlay::Create(ui->window_, width, height, x, y);
  view()->set_view_listener(this);
  view()->set_load_listener(this);
}

Tab::~Tab() {
  view()->set_view_listener(nullptr);
  view()->set_load_listener(nullptr);
}

void Tab::Resize(int width, int height) {
  overlay_->Resize(width, height);
}

void Tab::OnChangeTitle(View* caller, const String& title) {
  ui_->UpdateTabTitle(id_, title);
}

void Tab::OnChangeURL(View* caller, const String& url) {
  ui_->UpdateTabURL(id_, url);
}

void Tab::OnChangeTooltip(View* caller, const String& tooltip) {}

void Tab::OnChangeCursor(View* caller, Cursor cursor) {
  if (id_ == ui_->active_tab_id_)
    ui_->SetCursor(cursor);
}

void Tab::OnAddConsoleMessage(View* caller,
  MessageSource source,
  MessageLevel level,
  const String& message,
  uint32_t line_number,
  uint32_t column_number,
  const String& source_id) {
#if _WIN32
  OutputDebugStringW(message.utf16().data());
#endif
}

void Tab::OnBeginLoading(View* caller) {
  ui_->UpdateTabNavigation(id_, caller->is_loading(), caller->CanGoBack(), caller->CanGoForward());
}

void Tab::OnFinishLoading(View* caller) {
  ui_->UpdateTabNavigation(id_, caller->is_loading(), caller->CanGoBack(), caller->CanGoForward());
}

void Tab::OnUpdateHistory(View* caller) {
  ui_->UpdateTabNavigation(id_, caller->is_loading(), caller->CanGoBack(), caller->CanGoForward());
}

void Tab::OnDOMReady(View* caller) {}
