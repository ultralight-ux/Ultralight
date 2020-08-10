#include "GLTextureSurface.h"
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <sstream>
#include <string>

#if _WIN32
#include <Windows.h>
#define FATAL(x) { std::stringstream str; \
  str << "[ERROR] " << __FUNCSIG__ << " @ Line " << __LINE__ << ":\n\t" << x << std::endl; \
  OutputDebugStringA(str.str().c_str()); \
  std::cerr << str.str() << std::endl; \
  __debugbreak(); std::cin.get(); exit(-1); }
#else
#define FATAL(x) { std::cerr << "[ERROR] " << __PRETTY_FUNCTION__ << " @ Line " << __LINE__ << ":\n\t" << x << std::endl; \
  std::cin.get(); exit(-1); }
#endif

inline char const* glErrorString(GLenum const err) noexcept
{
  switch (err)
  {
    // OpenGL 2.0+ Errors:
  case GL_NO_ERROR: return "GL_NO_ERROR";
  case GL_INVALID_ENUM: return "GL_INVALID_ENUM";
  case GL_INVALID_VALUE: return "GL_INVALID_VALUE";
  case GL_INVALID_OPERATION: return "GL_INVALID_OPERATION";
  case GL_STACK_OVERFLOW: return "GL_STACK_OVERFLOW";
  case GL_STACK_UNDERFLOW: return "GL_STACK_UNDERFLOW";
  case GL_OUT_OF_MEMORY: return "GL_OUT_OF_MEMORY";
    // OpenGL 3.0+ Errors
  case GL_INVALID_FRAMEBUFFER_OPERATION: return "GL_INVALID_FRAMEBUFFER_OPERATION";
  default: return "UNKNOWN ERROR";
}
}

#ifdef _DEBUG
#define CHECK_GL()  {if (GLenum err = glGetError()) FATAL(glErrorString(err)) }                                                     
#else
#define CHECK_GL()
#endif   

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

class GLPBOTextureSurface : public GLTextureSurface {
public:
  GLPBOTextureSurface(uint32_t width, uint32_t height) {
    Resize(width, height);
  }

  virtual ~GLPBOTextureSurface() {
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
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo_id_);
    void* result = glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_READ_WRITE);
    CHECK_GL();
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
    return result;
  }

  virtual void UnlockPixels() override { 
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo_id_);
    glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER); 
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
  }

  virtual void Resize(uint32_t width, uint32_t height) override {
    if (pbo_id_ && width_ == width && height_ == height)
      return;

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

    glGenBuffers(1, &pbo_id_);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo_id_);
    glBufferData(GL_PIXEL_UNPACK_BUFFER, size_, 0, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

    glGenTextures(1, &texture_id_);
    glBindTexture(GL_TEXTURE_2D, texture_id_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glBindTexture(GL_TEXTURE_2D, 0);
  }

  virtual GLuint GetTextureAndSyncIfNeeded() {
    if (!dirty_bounds().IsEmpty()) {
      // Update texture from pbo
      glBindTexture(GL_TEXTURE_2D, texture_id_);
      glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo_id_);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width_, height_,
        0, GL_BGRA, GL_UNSIGNED_BYTE, 0);
      CHECK_GL();
      glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
      glBindTexture(GL_TEXTURE_2D, 0);

      // Clear dirty bounds
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
  return new GLPBOTextureSurface(width, height);
}

void GLTextureSurfaceFactory::DestroySurface(ultralight::Surface* surface) {
  delete static_cast<GLPBOTextureSurface*>(surface);
}
