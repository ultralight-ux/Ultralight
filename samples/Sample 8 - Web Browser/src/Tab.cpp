#include "Tab.h"
#include "UI.h"
#include <iostream>
#include <string>

#define INSPECTOR_DRAG_HANDLE_HEIGHT 10

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
    view()->CreateLocalInspectorView();
  } else {
    if (inspector_overlay_->is_hidden()) {
      inspector_overlay_->Show();
    } else {
      inspector_overlay_->Hide();
    }
  }

  // Force resize to update layout
}


bool Tab::IsInspectorShowing() const {
  if (!inspector_overlay_)
    return false;

  return !inspector_overlay_->is_hidden();
}

IntRect Tab::GetInspectorResizeDragHandle() const {
  if (!IsInspectorShowing())
    return IntRect::MakeEmpty();

  int drag_handle_height_px = (uint32_t)std::round(INSPECTOR_DRAG_HANDLE_HEIGHT * ui_->window()->scale());

  // This drag handle should span the width of the UI and be centered vertically at the boundary between
  // the page overlay and inspector overlay.

  int drag_handle_x = (int)inspector_overlay_->x();
  int drag_handle_y = (int)inspector_overlay_->y() - drag_handle_height_px / 2;

  return { drag_handle_x, drag_handle_y, drag_handle_x + (int)inspector_overlay_->width(),
           drag_handle_y + drag_handle_height_px };
}

int Tab::GetInspectorHeight() const {
  if (inspector_overlay_)
    return inspector_overlay_->height();

  return 0;
}

void Tab::SetInspectorHeight(int height) {
  if (height > 2) {
    inspector_overlay_->Resize(inspector_overlay_->width(), height);

    // Trigger a resize to perform re-layout / re-size of content overlay
    Resize(container_width_, container_height_);
  }
}

void Tab::Resize(uint32_t width, uint32_t height) {
  container_width_ = width;
  container_height_ = height;

  uint32_t content_height = container_height_;
  if (inspector_overlay_ && !inspector_overlay_->is_hidden()) {
    content_height -= inspector_overlay_->height();
  }
  
  if (content_height < 1)
    content_height = 1;

  overlay_->Resize(container_width_, content_height);

  if (inspector_overlay_ && !inspector_overlay_->is_hidden()) {
    inspector_overlay_->MoveTo(0, overlay_->y() + overlay_->height());
    inspector_overlay_->Resize(container_width_, inspector_overlay_->height());
  }
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

RefPtr<View> Tab::OnCreateInspectorView(ultralight::View* caller, bool is_local,
                                         const String& inspected_url) {
  if (inspector_overlay_)
    return nullptr;

  inspector_overlay_ = Overlay::Create(ui_->window_, container_width_, container_height_ / 2, 0, 0);

  // Force resize to update layout
  Resize(container_width_, container_height_);
  inspector_overlay_->Show();

  return inspector_overlay_->view();
}

void Tab::OnBeginLoading(View* caller, uint64_t frame_id, bool is_main_frame, const String& url) {
  ui_->UpdateTabNavigation(id_, caller->is_loading(), caller->CanGoBack(), caller->CanGoForward());
}

void Tab::OnFinishLoading(View* caller, uint64_t frame_id, bool is_main_frame, const String& url) {
  ui_->UpdateTabNavigation(id_, caller->is_loading(), caller->CanGoBack(), caller->CanGoForward());
}

void Tab::OnFailLoading(View* caller, uint64_t frame_id, bool is_main_frame, const String& url,
  const String& description, const String& error_domain, int error_code) {
  if (is_main_frame) {
    char error_code_str[16]; 
    sprintf(error_code_str,"%d", error_code);

    String html_string = "<html><head><style>";
    html_string += "* { font-family: sans-serif; }";
    html_string += "body { background-color: #CCC; color: #555; padding: 4em; }";
    html_string += "dt { font-weight: bold; padding: 1em; }";
    html_string += "</style></head><body>";
    html_string += "<h2>A Network Error was Encountered</h2>";
    html_string += "<dl>";
    html_string += "<dt>URL</dt><dd>" + url + "</dd>";
    html_string += "<dt>Description</dt><dd>" + description + "</dd>";
    html_string += "<dt>Error Domain</dt><dd>" + error_domain + "</dd>";
    html_string += "<dt>Error Code</dt><dd>" + String(error_code_str) + "</dd>";
    html_string += "</dl></body></html>";

    view()->LoadHTML(html_string);
  }
}

void Tab::OnUpdateHistory(View* caller) {
  ui_->UpdateTabNavigation(id_, caller->is_loading(), caller->CanGoBack(), caller->CanGoForward());
}
