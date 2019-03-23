#include "OverlayManager.h"
#include <algorithm>
#include <AppCore/Overlay.h>

namespace ultralight {

OverlayManager::OverlayManager() {
}

OverlayManager::~OverlayManager() {
}

void OverlayManager::Add(Overlay* overlay) {
  overlays_.push_back(overlay);
}

void OverlayManager::Remove(Overlay* overlay) {
  overlays_.erase(std::remove(overlays_.begin(), overlays_.end(), overlay), overlays_.end());

  if (focused_overlay_ == overlay) {
    focused_overlay_ = nullptr;
    is_dragging_ = false;
  }

  if (hovered_overlay_ == overlay)
    hovered_overlay_ = nullptr;
}

void OverlayManager::Draw() {
  for (auto& i : overlays_)
    i->Draw();
}

void OverlayManager::FireKeyEvent(const ultralight::KeyEvent& evt) {
  if (focused_overlay_)
    focused_overlay_->view()->FireKeyEvent(evt);
}

void OverlayManager::FireMouseEvent(const ultralight::MouseEvent& evt) {
  if (is_dragging_) {
    MouseEvent rel_evt = evt;
    rel_evt.x -= hovered_overlay_->x();
    rel_evt.y -= hovered_overlay_->y();

    focused_overlay_->view()->FireMouseEvent(rel_evt);

    if (evt.type == ultralight::MouseEvent::kType_MouseUp && evt.button == MouseEvent::kButton_Left)
      is_dragging_ = false;

    return;
  }

  hovered_overlay_ = HitTest(evt.x, evt.y);

  if (evt.type == ultralight::MouseEvent::kType_MouseDown && evt.button == MouseEvent::kButton_Left) {
    focused_overlay_ = HitTest(evt.x, evt.y);
    is_dragging_ = true;
  }

  if (hovered_overlay_) {
    MouseEvent rel_evt = evt;
    rel_evt.x -= hovered_overlay_->x();
    rel_evt.y -= hovered_overlay_->y();

    hovered_overlay_->view()->FireMouseEvent(rel_evt);
  }
}

void OverlayManager::FireScrollEvent(const ultralight::ScrollEvent& evt) {
  if (hovered_overlay_)
    hovered_overlay_->view()->FireScrollEvent(evt);
}

void OverlayManager::FocusOverlay(Overlay* overlay) {
  if (!is_dragging_)
    focused_overlay_ = overlay;
}

void OverlayManager::UnfocusOverlay(Overlay* overlay) {
  if (focused_overlay_ == overlay)
    focused_overlay_ = nullptr;
}

bool OverlayManager::IsOverlayFocused(Overlay* overlay) const {
  return focused_overlay_ == overlay;
}

Overlay* OverlayManager::HitTest(int x, int y) {
  for (auto& i : overlays_) {
    if (!i->is_hidden() && x >= i->x() && y >= i->y() && x < i->x() + (int)i->width() && y < i->y() + (int)i->height())
      return i;
  }

  return nullptr;
}


}  // namespace ultralight
