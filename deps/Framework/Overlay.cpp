#include "Overlay.h"
#include <Ultralight/Renderer.h>
#include <Ultralight/platform/Platform.h>
#include <Ultralight/platform/Config.h>

namespace framework {

static ultralight::IndexType patternCW[] = { 0, 1, 3, 1, 2, 3 };
static ultralight::IndexType patternCCW[] = { 0, 3, 1, 1, 3, 2 };

Overlay::Overlay(ultralight::Ref<ultralight::Renderer> renderer,
  ultralight::GPUDriver* driver,
  int width, int height, int x, int y, float scale) :
  view_(renderer->CreateView(width, height, false)), width_(width), height_(height),
  x_(x), y_(y), needs_update_(true), driver_(driver) {
}

Overlay::~Overlay() {
  if (vertices_.size())
    driver_->DestroyGeometry(geometry_id_);
}

void Overlay::Draw() {
  UpdateGeometry();

  gpu_state_.texture_1_id = view()->render_target().texture_id;

  driver_->DrawGeometry(geometry_id_, 6, 0, gpu_state_);
}

void Overlay::FireKeyEvent(const ultralight::KeyEvent& evt) {
  view()->FireKeyEvent(evt);
}

void Overlay::FireMouseEvent(const ultralight::MouseEvent& evt) {
  if (evt.type == ultralight::MouseEvent::kType_MouseDown &&
      evt.button == ultralight::MouseEvent::kButton_Left)
    has_focus_ = Contains(evt.x, evt.y);
  else if (evt.type == ultralight::MouseEvent::kType_MouseMoved)
    has_hover_ = Contains(evt.x, evt.y);

  ultralight::MouseEvent rel_evt = evt;
  rel_evt.x -= x_;
  rel_evt.y -= y_;

  view()->FireMouseEvent(rel_evt);
}

void Overlay::FireScrollEvent(const ultralight::ScrollEvent& evt) {
  //if (has_hover_)
    view()->FireScrollEvent(evt);
}

void Overlay::Resize(int width, int height) {
  if (width == width_ && height == height_)
    return;

  view_->Resize(width, height);

  width_ = width;
  height_ = height;
  needs_update_ = true;
}

void Overlay::UpdateGeometry() {
  bool initial_creation = false;
  if (vertices_.empty()) {
    vertices_.resize(4);
    indices_.resize(6);

    auto& config = ultralight::Platform::instance().config();
    if (config.face_winding == ultralight::kFaceWinding_Clockwise)
      memcpy(indices_.data(), patternCW, sizeof(ultralight::IndexType) * indices_.size());
    else
      memcpy(indices_.data(), patternCCW, sizeof(ultralight::IndexType) * indices_.size());

    memset(&gpu_state_, 0, sizeof(gpu_state_));
    ultralight::Matrix identity;
    identity.SetIdentity();

    gpu_state_.transform = ultralight::ConvertAffineTo4x4(identity);
    gpu_state_.enable_blend = true;
    gpu_state_.enable_texturing = true;
    gpu_state_.shader_type = ultralight::kShaderType_Fill;
    gpu_state_.render_buffer_id = 0; // default render target view (screen)

    initial_creation = true;
  }

  if (!needs_update_)
    return;

  ultralight::Vertex_2f_4ub_2f_2f_28f v;
  memset(&v, 0, sizeof(v));

  v.data0[0] = 1; // Fill Type: Image

  v.color[0] = 255;
  v.color[1] = 255;
  v.color[2] = 255;
  v.color[3] = 255;

  float left = static_cast<float>(x_);
  float top = static_cast<float>(y_);
  float right = static_cast<float>(x_ + width());
  float bottom = static_cast<float>(y_ + height());

  ultralight::RenderTarget target = view_->render_target();
  float tex_width_ratio = target.width / (float)target.texture_width;
  float tex_height_ratio = target.height / (float)target.texture_height;

#ifdef FRAMEWORK_PLATFORM_GLFW
  const float flip_y = false;
#else
  const float flip_y = false;
#endif

  // TOP LEFT
  v.pos[0] = v.obj[0] = left;
  v.pos[1] = v.obj[1] = top;
  v.tex[0] = 0;
  v.tex[1] = flip_y ? tex_height_ratio : 0;

  vertices_[0] = v;

  // TOP RIGHT
  v.pos[0] = v.obj[0] = right;
  v.pos[1] = v.obj[1] = top;
  v.tex[0] = tex_width_ratio;
  v.tex[1] = flip_y ? tex_height_ratio : 0;

  vertices_[1] = v;

  // BOTTOM RIGHT
  v.pos[0] = v.obj[0] = right;
  v.pos[1] = v.obj[1] = bottom;
  v.tex[0] = tex_width_ratio;
  v.tex[1] = flip_y ? 0 : tex_height_ratio;

  vertices_[2] = v;

  // BOTTOM LEFT
  v.pos[0] = v.obj[0] = left;
  v.pos[1] = v.obj[1] = bottom;
  v.tex[0] = 0;
  v.tex[1] = flip_y ? 0 : tex_height_ratio;

  vertices_[3] = v;

  ultralight::VertexBuffer vbuffer;
  vbuffer.format = ultralight::kVertexBufferFormat_2f_4ub_2f_2f_28f;
  vbuffer.size = static_cast<uint32_t>(sizeof(ultralight::Vertex_2f_4ub_2f_2f_28f) * vertices_.size());
  vbuffer.data = (uint8_t*)vertices_.data();

  ultralight::IndexBuffer ibuffer;
  ibuffer.size = static_cast<uint32_t>(sizeof(ultralight::IndexType) * indices_.size());
  ibuffer.data = (uint8_t*)indices_.data();

  if (initial_creation) {
    geometry_id_ = driver_->NextGeometryId();
    driver_->CreateGeometry(geometry_id_, vbuffer, ibuffer);
  }
  else {
    driver_->UpdateGeometry(geometry_id_, vbuffer, ibuffer);
  }

  needs_update_ = false;
}

}  // namespace framework
