#pragma once
#include "Sample.h"
#include "GLTextureSurface.h"

using namespace ultralight;

// A "WebTile" is a View backed by an OpenGL texture.
class WebTile {
public:
  WebTile(RefPtr<Renderer> renderer, int width, int height);

  WebTile(RefPtr<View> existing_view);

  RefPtr<View> view() { return view_; }

  GLTextureSurface* surface() { return static_cast<GLTextureSurface*>(view_->surface()); }

  void ToggleTransparency();

protected:
  RefPtr<View> view_;
  bool is_transparent_ = false;
};
