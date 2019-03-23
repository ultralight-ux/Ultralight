#include <AppCore/Overlay.h>
#include <AppCore/Window.h>
#include <AppCore/App.h>
#include <Ultralight/Renderer.h>
#include <Ultralight/Buffer.h>
#include <Ultralight/platform/Platform.h>
#include <Ultralight/platform/Config.h>
#include <Ultralight/platform/GPUDriver.h>
#include "RefCountedImpl.h"
#include "OverlayManager.h"
#include <vector>

namespace ultralight {

static IndexType patternCW[] = { 0, 1, 3, 1, 2, 3 };
static IndexType patternCCW[] = { 0, 3, 1, 1, 3, 2 };

class OverlayImpl : public Overlay,
                    public RefCountedImpl<OverlayImpl> {
public:
  virtual void Draw() override {
    if (is_hidden_)
      return;

    UpdateGeometry();

    driver_->DrawGeometry(geometry_id_, 6, 0, gpu_state_);
  }

  virtual void Resize(uint32_t width, uint32_t height) override {
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
    gpu_state_.viewport_width = (float) window_->width();
    gpu_state_.viewport_height = (float) window_->height();
  }

  void UpdateGeometry() {
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

      gpu_state_.viewport_width = (float) window_->width();
      gpu_state_.viewport_height = (float) window_->height();
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

  virtual Ref<View> view() override { return view_; }

  virtual uint32_t width() const override { return width_; }

  virtual uint32_t height() const override { return height_; }

  virtual int x() const override { return x_; }

  virtual int y() const override { return y_; }

  virtual bool is_hidden() const override { return is_hidden_; }

  virtual void Hide() override {
    is_hidden_ = true;
  }

  virtual void Show() override {
    is_hidden_ = false;
  }

  virtual bool has_focus() const override {
    return window_->overlay_manager()->IsOverlayFocused((Overlay*)this);
  }

  virtual void Focus() override {
    window_->overlay_manager()->FocusOverlay((Overlay*)this);
  }

  virtual void Unfocus() override {
    window_->overlay_manager()->UnfocusOverlay((Overlay*)this);
  }

  virtual void MoveTo(int x, int y) override {
    x_ = x;
    y_ = y;
    needs_update_ = true;
  }

  REF_COUNTED_IMPL(OverlayImpl);

protected:
  OverlayImpl(Ref<Window> window, uint32_t width, uint32_t height, int x, int y) :
    window_(window), view_(App::instance()->renderer()->CreateView(width, height, false)),
    width_(width), height_(height), x_(x), y_(y), needs_update_(true),
    driver_(Platform::instance().gpu_driver()) {
    window_->overlay_manager()->Add(this);
  }

  ~OverlayImpl() {
    if (App::instance()) {
      window_->overlay_manager()->Remove(this);

      if (vertices_.size())
        driver_->DestroyGeometry(geometry_id_);
    }
  }

  Ref<Window> window_;
  uint32_t width_;
  uint32_t height_;
  int x_;
  int y_;
  bool is_hidden_ = false;
  ultralight::Ref<ultralight::View> view_;
  ultralight::GPUDriver* driver_;
  std::vector<ultralight::Vertex_2f_4ub_2f_2f_28f> vertices_;
  std::vector<ultralight::IndexType> indices_;
  bool needs_update_;
  uint32_t geometry_id_;
  ultralight::GPUState gpu_state_;

  friend class Overlay;

  DISALLOW_COPY_AND_ASSIGN(OverlayImpl);
};

Ref<Overlay> Overlay::Create(Ref<Window> window, uint32_t width,
                             uint32_t height, int x, int y) {
  return AdoptRef(*new OverlayImpl(window, width, height, x, y));
}

Overlay::~Overlay() {}


}  // namespace ultralight
