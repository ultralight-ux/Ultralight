#pragma once
#include <vector>
#include <Ultralight/View.h>
#include <Ultralight/Renderer.h>
#include <Ultralight/platform/GPUDriver.h>

namespace framework {

class Overlay {
public:
  Overlay(ultralight::Ref<ultralight::Renderer> renderer, ultralight::GPUDriver* driver,
    int width, int height, int x, int y, float scale);
  virtual ~Overlay();

  ultralight::Ref<ultralight::View> view() { return view_; }

  int width() const { return width_; }
  int height() const { return height_; }
  int x() const { return x_; }
  int y() const { return y_; }

  void MoveTo(int x, int y) { x_ = x; y_ = y; needs_update_ = true; }
  void MoveBy(int dx, int dy) { x_ += dx; y_ += dy; needs_update_ = true; }

  bool Contains(int x, int y) const { return x >= x_ && y >= y_ && x < x_ + width_ && y < y_ + height_; }

  virtual void Draw();

  virtual void FireKeyEvent(const ultralight::KeyEvent& evt);

  virtual void FireMouseEvent(const ultralight::MouseEvent& evt);

  virtual void FireScrollEvent(const ultralight::ScrollEvent& evt);

  virtual void Resize(int width, int height);

protected:
  void UpdateGeometry();

  int width_;
  int height_;
  int x_;
  int y_;
  float scale_;
  bool has_focus_ = false;
  bool has_hover_ = false;
  ultralight::Ref<ultralight::View> view_;
  ultralight::GPUDriver* driver_;
  std::vector<ultralight::Vertex_2f_4ub_2f_2f_28f> vertices_;
  std::vector<ultralight::IndexType> indices_;
  bool needs_update_;
  uint32_t geometry_id_;
  ultralight::GPUState gpu_state_;
};

}  // namespace framework
