#include "WebTile.h"

#if TRANSPARENT
#define TEX_FORMAT	GL_RGBA
#else
#define TEX_FORMAT	GL_RGB
#endif

WebTile::WebTile(RefPtr<Renderer> renderer, int width, int height, double scale) {
  ViewConfig view_config;
  view_config.initial_device_scale = scale;
  view_config.is_accelerated = false;
  view_ = renderer->CreateView(width, height, view_config, nullptr);
}

WebTile::WebTile(RefPtr<View> existing_view) : view_(existing_view) {
}

void WebTile::ToggleTransparency() {
  view_->EvaluateScript("document.body.style.background = 'transparent'");
}
