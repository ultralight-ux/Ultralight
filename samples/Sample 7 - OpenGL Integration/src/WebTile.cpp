#include "WebTile.h"

#if TRANSPARENT
#define TEX_FORMAT	GL_RGBA
#else
#define TEX_FORMAT	GL_RGB
#endif

WebTile::WebTile(RefPtr<Renderer> renderer, int width, int height) {
  view_ = renderer->CreateView(width, height, true, nullptr);
}

WebTile::WebTile(RefPtr<View> existing_view) : view_(existing_view) {
}

void WebTile::ToggleTransparency() {
  view_->EvaluateScript("document.body.style.background = 'transparent'");
}
