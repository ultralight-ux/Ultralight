#pragma once
#include "Window.h"
#include <Ultralight/View.h>

namespace ultralight {

class AExport Overlay : public RefCounted {
public:
  static Ref<Overlay> Create(int width, int height, int x, int y);

  virtual ultralight::Ref<ultralight::View> view() = 0;

  virtual int width() const = 0;

  virtual int height() const = 0;
  
  virtual int x() const = 0;

  virtual int y() const = 0;

  virtual void MoveTo(int x, int y) = 0;

  virtual void Resize(int width, int height) = 0;

protected:
  virtual ~Overlay();
  virtual void Draw() = 0;
  friend class OverlayManager;
};

}  // namespace framework
