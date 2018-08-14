#include "Tab.h"
#include "UI.h"
#include <iostream>
#include <string>

Tab::Tab(UI* ui, size_t id) : ui_(ui), id_(id), Overlay(ui->renderer_, ui->driver_, ui->screen_width_,
  ui->tab_height_, 0, ui->ui_height_, ui->scale_) {
  view()->set_view_listener(this);
  view()->set_load_listener(this);
}

Tab::~Tab() {
  view()->set_view_listener(nullptr);
  view()->set_load_listener(nullptr);
}

void Tab::OnChangeTitle(ultralight::View* caller,
  const ultralight::String& title) {
  ui_->UpdateTabTitle(id_, title);
}

void Tab::OnChangeURL(ultralight::View* caller,
  const ultralight::String& url) {
  ui_->UpdateTabURL(id_, url);
}

void Tab::OnChangeTooltip(ultralight::View* caller,
  const ultralight::String& tooltip) {}

void Tab::OnChangeCursor(ultralight::View* caller,
  ultralight::Cursor cursor) {
  if (id_ == ui_->active_tab_id_)
    ui_->SetCursor(cursor);
}

void Tab::OnAddConsoleMessage(ultralight::View* caller,
  ultralight::MessageSource source,
  ultralight::MessageLevel level,
  const ultralight::String& message,
  uint32_t line_number,
  uint32_t column_number,
  const ultralight::String& source_id) {
}

void Tab::OnBeginLoading(ultralight::View* caller) {
  ui_->UpdateTabNavigation(id_, caller->is_loading(), caller->CanGoBack(), caller->CanGoForward());
}

void Tab::OnFinishLoading(ultralight::View* caller) {
  ui_->UpdateTabNavigation(id_, caller->is_loading(), caller->CanGoBack(), caller->CanGoForward());
}

void Tab::OnUpdateHistory(ultralight::View* caller) {
  ui_->UpdateTabNavigation(id_, caller->is_loading(), caller->CanGoBack(), caller->CanGoForward());
}

void Tab::OnDOMReady(ultralight::View* caller) {}
