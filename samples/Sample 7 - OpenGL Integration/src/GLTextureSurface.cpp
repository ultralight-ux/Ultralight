#include "GLTextureSurface.h"
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <sstream>
#include <string>

///
/// Custom Surface implementation that allows Ultralight to paint directly
/// into an OpenGL PBO (pixel buffer object).
///
/// PBOs in OpenGL allow us to get a pointer to a block of GPU-controlled
/// memory for lower-latency uploads to a texture.
///
/// For more info: <http://www.songho.ca/opengl/gl_pbo.html>
///
class GLPBOTextureSurface : public GLTextureSurface {
public:
  GLPBOTextureSurface(uint32_t width, uint32_t height) {
    Resize(width, height);
  }

  virtual ~GLPBOTextureSurface() {
    ///
    /// Destroy our PBO and texture.
    ///
    if (pbo_id_) {
      glDeleteBuffers(1, &pbo_id_);
      pbo_id_ = 0;
      glDeleteTextures(1, &texture_id_);
      texture_id_ = 0;
    }
  }

  virtual uint32_t width() const override { return width_; }

  virtual uint32_t height() const override { return height_; }

  virtual uint32_t row_bytes() const override { return row_bytes_; }

  virtual size_t size() const override { return size_; }

  virtual void* LockPixels() override { 
    ///
    /// Map our PBO to system memory so Ultralight can draw to it.
    ///
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo_id_);
    void* result = glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_READ_WRITE);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
    return result;
  }

  virtual void UnlockPixels() override { 
    ///
    /// Unmap our PBO.
    ///
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo_id_);
    glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER); 
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
  }

  virtual void Resize(uint32_t width, uint32_t height) override {
    if (pbo_id_ && width_ == width && height_ == height)
      return;

    ///
    /// Destroy any existing PBO and texture.
    ///
    if (pbo_id_) {
      glDeleteBuffers(1, &pbo_id_);
      pbo_id_ = 0;
      glDeleteTextures(1, &texture_id_);
      texture_id_ = 0;
    }

    width_ = width;
    height_ = height;
    row_bytes_ = width_ * 4;
    size_ = row_bytes_ * height_;

    ///
    /// Create our PBO (pixel buffer object), with a size of 'size_'
    ///
    glGenBuffers(1, &pbo_id_);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo_id_);
    glBufferData(GL_PIXEL_UNPACK_BUFFER, size_, 0, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

    ///
    /// Create our Texture object.
    ///
    glGenTextures(1, &texture_id_);
    glBindTexture(GL_TEXTURE_2D, texture_id_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glBindTexture(GL_TEXTURE_2D, 0);
  }

  virtual GLuint GetTextureAndSyncIfNeeded() {
    ///
    /// This is called when our application wants to draw this Surface to
    /// an OpenGL quad. (We return an OpenGL texture handle)
    ///
    /// We take this opportunity to upload the PBO to the texture if the
    /// pixels have changed since the last call (indicated by dirty_bounds()
    /// being non-empty)
    ///
    if (!dirty_bounds().IsEmpty()) {
      ///
      /// Update our Texture from our PBO (pixel buffer object)
      ///
      glBindTexture(GL_TEXTURE_2D, texture_id_);
      glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo_id_);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width_, height_,
        0, GL_BGRA, GL_UNSIGNED_BYTE, 0);
      glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
      glBindTexture(GL_TEXTURE_2D, 0);

      ///
      /// Clear our Surface's dirty bounds to indicate we've handled any
      /// pending modifications to our pixels.
      ///
      ClearDirtyBounds();
    }

    return texture_id_;
  }

protected:
  GLuint texture_id_;
  GLuint pbo_id_ = 0;
  uint32_t width_;
  uint32_t height_;
  uint32_t row_bytes_;
  uint32_t size_;
};

GLTextureSurfaceFactory::GLTextureSurfaceFactory() {
}

GLTextureSurfaceFactory::~GLTextureSurfaceFactory() {
}

ultralight::Surface* GLTextureSurfaceFactory::CreateSurface(uint32_t width, uint32_t height) {
  ///
  /// Called by Ultralight when it wants to create a Surface.
  ///
  return new GLPBOTextureSurface(width, height);
}

void GLTextureSurfaceFactory::DestroySurface(ultralight::Surface* surface) {
  ///
  /// Called by Ultralight when it wants to destroy a Surface.
  ///
  delete static_cast<GLPBOTextureSurface*>(surface);
}



//////////////////////////////////////////////////////////////////////////////

///
/// Optional bitmap-based GLTextureSurface.
///
/// We don't actually use this in this Sample but it's here for reference.
///
class GLBitmapTextureSurface : public GLTextureSurface {
public:
  GLBitmapTextureSurface(uint32_t width, uint32_t height) {
    Resize(width, height);
  }

  virtual ~GLBitmapTextureSurface() {
    if (bitmap_) {
      bitmap_ = nullptr;
      glDeleteTextures(1, &texture_id_);
    }
  }

  virtual uint32_t width() const override { return bitmap_->width(); }

  virtual uint32_t height() const override { return bitmap_->height(); }

  virtual uint32_t row_bytes() const override { return bitmap_->row_bytes(); }

  virtual size_t size() const override { return bitmap_->size(); }

  virtual void* LockPixels() override { return bitmap_->LockPixels(); }

  virtual void UnlockPixels() override { return bitmap_->UnlockPixels(); }

  virtual void Resize(uint32_t width, uint32_t height) override {
    if (bitmap_ && bitmap_->width() == width && bitmap_->height() == height)
      return;

    if (bitmap_) {
      bitmap_ = nullptr;
      glDeleteTextures(1, &texture_id_);
    }

    bitmap_ = ultralight::Bitmap::Create(width, height, ultralight::kBitmapFormat_BGRA8_UNORM_SRGB);

    glGenTextures(1, &texture_id_);
    glBindTexture(GL_TEXTURE_2D, texture_id_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  }

  virtual GLuint GetTextureAndSyncIfNeeded() {
    if (!dirty_bounds().IsEmpty()) {
      // Update texture from bitmap
      void* pixels = bitmap_->LockPixels();
      glBindTexture(GL_TEXTURE_2D, texture_id_);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bitmap_->width(), bitmap_->height(),
        0, GL_BGRA, GL_UNSIGNED_BYTE, pixels);
      bitmap_->UnlockPixels();

      // Clear dirty bounds
      ClearDirtyBounds();
    }

    return texture_id_;
  }

protected:
  GLuint texture_id_;
  ultralight::RefPtr<ultralight::Bitmap> bitmap_;
};