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

  if (focused_overlay_ == overlay)
    focused_overlay_ = nullptr;

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
  hovered_overlay_ = HitTest(evt.x, evt.y);

  if (evt.type == ultralight::MouseEvent::kType_MouseDown)
    focused_overlay_ = HitTest(evt.x, evt.y);

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

Overlay* OverlayManager::HitTest(int x, int y) {
  for (auto& i : overlays_) {
    if (x >= i->x() && y >= i->y() && x < i->x() + i->width() && y < i->y() + i->height())
      return i;
  }

  return nullptr;
}


}  // namespace ultralight
