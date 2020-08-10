#pragma once
#include <Ultralight/platform/Surface.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class GLTextureSurface : public ultralight::Surface {
public:
  virtual ~GLTextureSurface() {}

  virtual GLuint GetTextureAndSyncIfNeeded() = 0;
};

class GLTextureSurfaceFactory : public ultralight::SurfaceFactory {
public:
  GLTextureSurfaceFactory();

  virtual ~GLTextureSurfaceFactory();

  virtual ultralight::Surface* CreateSurface(uint32_t width, uint32_t height) override;

  virtual void DestroySurface(ultralight::Surface* surface) override;
};
