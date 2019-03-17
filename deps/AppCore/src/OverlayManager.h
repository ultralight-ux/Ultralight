#pragma once
#include <AppCore/Defines.h>
#include <Ultralight/KeyEvent.h>
#include <Ultralight/MouseEvent.h>
#include <Ultralight/ScrollEvent.h>
#include <vector>

namespace ultralight {

class Overlay;

class OverlayManager {
public:
  OverlayManager();
  ~OverlayManager();

  virtual void Add(Overlay* overlay);

  virtual void Remove(Overlay* overlay);

  virtual void Draw();

  virtual void FireKeyEvent(const ultralight::KeyEvent& evt);

  virtual void FireMouseEvent(const ultralight::MouseEvent& evt);

  virtual void FireScrollEvent(const ultralight::ScrollEvent& evt);

  virtual void FocusOverlay(Overlay* overlay);

  virtual void UnfocusOverlay(Overlay* overlay);

  virtual bool IsOverlayFocused(Overlay* overlay) const;

protected:
  Overlay* HitTest(int x, int y);

  std::vector<Overlay*> overlays_;
  Overlay* focused_overlay_ = nullptr;
  Overlay* hovered_overlay_ = nullptr;
  bool is_dragging_ = false;
};

}  // namespace ultralight
