#include "Overlay.h"
#include "Window.h"
#include <Ultralight/Renderer.h>
#include <Ultralight/Buffer.h>
#include <Ultralight/platform/Platform.h>
#include <Ultralight/platform/Config.h>

using namespace ultralight;

namespace framework {

static IndexType patternCW[] = { 0, 1, 3, 1, 2, 3 };
static IndexType patternCCW[] = { 0, 3, 1, 1, 3, 2 };

Overlay::Overlay(Ref<Renderer> renderer,
  GPUDriver* driver, const Window& window,
  int width, int height, int x, int y) :
  view_(renderer->CreateView(width, height, false)), width_(width), height_(height),
  x_(x), y_(y), needs_update_(true), driver_(driver), window_(window) {
}

Overlay::~Overlay() {
  if (vertices_.size())
    driver_->DestroyGeometry(geometry_id_);
}

void Overlay::Draw() {
  UpdateGeometry();

  driver_->DrawGeometry(geometry_id_, 6, 0, gpu_state_);
}

void Overlay::FireKeyEvent(const KeyEvent& evt) {
  view()->FireKeyEvent(evt);
}

void Overlay::FireMouseEvent(const MouseEvent& evt) {
  if (evt.type == MouseEvent::kType_MouseDown &&
      evt.button == MouseEvent::kButton_Left)
    has_focus_ = Contains(evt.x, evt.y);
  else if (evt.type == MouseEvent::kType_MouseMoved)
    has_hover_ = Contains(evt.x, evt.y);

  MouseEvent rel_evt = evt;
  rel_evt.x -= x_;
  rel_evt.y -= y_;

  view()->FireMouseEvent(rel_evt);
}

void Overlay::FireScrollEvent(const ScrollEvent& evt) {
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
  UpdateGeometry();

  // Update these now since they were invalidated
  RenderTarget target = view_->render_target();
  gpu_state_.texture_1_id = target.texture_id;
  gpu_state_.viewport_width = (float)window_.width();
  gpu_state_.viewport_height = (float)window_.height();
}

void Overlay::UpdateGeometry() {
  bool initial_creation = false;
  RenderTarget target = view_->render_target();

  if (vertices_.empty()) {
    vertices_.resize(4);
    indices_.resize(6);

    auto& config = Platform::instance().config();
    if (config.face_winding == kFaceWinding_Clockwise)
      memcpy(indices_.data(), patternCW, sizeof(IndexType) * indices_.size());
    else
      memcpy(indices_.data(), patternCCW, sizeof(IndexType) * indices_.size());

    memset(&gpu_state_, 0, sizeof(gpu_state_));
    Matrix identity;
    identity.SetIdentity();

    gpu_state_.viewport_width = (float)window_.width();
    gpu_state_.viewport_height = (float)window_.height();
    gpu_state_.transform = ConvertAffineTo4x4(identity);
    gpu_state_.enable_blend = true;
    gpu_state_.enable_texturing = true;
    gpu_state_.shader_type = kShaderType_Fill;
    gpu_state_.render_buffer_id = 0; // default render target view (screen)
    gpu_state_.texture_1_id = target.texture_id;

    initial_creation = true;
  }

  if (!needs_update_)
    return;

  Vertex_2f_4ub_2f_2f_28f v;
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

  // TOP LEFT
  v.pos[0] = v.obj[0] = left;
  v.pos[1] = v.obj[1] = top;
  v.tex[0] = target.uv_coords.left;
  v.tex[1] = target.uv_coords.top;

  vertices_[0] = v;

  // TOP RIGHT
  v.pos[0] = v.obj[0] = right;
  v.pos[1] = v.obj[1] = top;
  v.tex[0] = target.uv_coords.right;
  v.tex[1] = target.uv_coords.top;

  vertices_[1] = v;

  // BOTTOM RIGHT
  v.pos[0] = v.obj[0] = right;
  v.pos[1] = v.obj[1] = bottom;
  v.tex[0] = target.uv_coords.right;
  v.tex[1] = target.uv_coords.bottom;

  vertices_[2] = v;

  // BOTTOM LEFT
  v.pos[0] = v.obj[0] = left;
  v.pos[1] = v.obj[1] = bottom;
  v.tex[0] = target.uv_coords.left;
  v.tex[1] = target.uv_coords.bottom;

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
