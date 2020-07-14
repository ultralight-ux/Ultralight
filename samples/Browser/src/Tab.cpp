#include "Tab.h"
#include "UI.h"
#include <iostream>
#include <string>

#define INSPECTOR_HEIGHT 400

Tab::Tab(UI* ui, uint64_t id, uint32_t width, uint32_t height, int x, int y) 
  : ui_(ui), id_(id), container_width_(width), container_height_(height) {
  overlay_ = Overlay::Create(ui->window_, width, height, x, y);
  view()->set_view_listener(this);
  view()->set_load_listener(this);
}

Tab::~Tab() {
  view()->set_view_listener(nullptr);
  view()->set_load_listener(nullptr);
}

void Tab::Show() {
  overlay_->Show();
  overlay_->Focus();

  if (inspector_overlay_)
    inspector_overlay_->Show();
}

void Tab::Hide() {
  overlay_->Hide();
  overlay_->Unfocus();

  if (inspector_overlay_)
    inspector_overlay_->Hide();
}

void Tab::ToggleInspector() {
  if (!inspector_overlay_) {
    inspector_overlay_ = Overlay::Create(ui_->window_, *view()->inspector(), 0, 0);
  } else {
    if (inspector_overlay_->is_hidden())
      inspector_overlay_->Show();
    else
      inspector_overlay_->Hide();
  }

  // Force resize to update layout
  Resize(container_width_, container_height_);
}

void Tab::Resize(uint32_t width, uint32_t height) {
  container_width_ = width;
  container_height_ = height;

  uint32_t content_height = container_height_;
  if (inspector_overlay_ && !inspector_overlay_->is_hidden()) {
    uint32_t inspector_height_px = (uint32_t)std::round(INSPECTOR_HEIGHT * ui_->window_->scale());
    inspector_overlay_->Resize(container_width_, inspector_height_px);
    content_height -= inspector_height_px;
  }
  
  if (content_height < 1)
    content_height = 1;

  overlay_->Resize(container_width_, content_height);

  if (inspector_overlay_ && !inspector_overlay_->is_hidden())
    inspector_overlay_->MoveTo(0, overlay_->y() + overlay_->height());
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
}

RefPtr<View> Tab::OnCreateChildView(ultralight::View* caller,
  const String& opener_url, const String& target_url,
  bool is_popup, const IntRect& popup_rect) {
  return ui_->CreateNewTabForChildView(target_url);
}

void Tab::OnBeginLoading(View* caller, uint64_t frame_id, bool is_main_frame, const String& url) {
  ui_->UpdateTabNavigation(id_, caller->is_loading(), caller->CanGoBack(), caller->CanGoForward());
}

void Tab::OnFinishLoading(View* caller, uint64_t frame_id, bool is_main_frame, const String& url) {
  ui_->UpdateTabNavigation(id_, caller->is_loading(), caller->CanGoBack(), caller->CanGoForward());
}

void Tab::OnFailLoading(View* caller, uint64_t frame_id, bool is_main_frame, const String& url,
  const String& description, const String& error_domain, int error_code) {

}

void Tab::OnUpdateHistory(View* caller) {
  ui_->UpdateTabNavigation(id_, caller->is_loading(), caller->CanGoBack(), caller->CanGoForward());
}
