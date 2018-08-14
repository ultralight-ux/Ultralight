// Copyright 2018 Ultralight, Inc. All rights reserved.
#pragma once
#include <Ultralight/Defines.h>
#include <Ultralight/RefPtr.h>
#include <Ultralight/View.h>

namespace ultralight {

/**
 * Renderer for creating, updating, and rendering Views. Only one instance
 * should be created per application.
 */
class UExport Renderer : public RefCounted {
public:
  // Create the Renderer
  static Ref<Renderer> Create();

  // Create a View with certain width and height
  virtual Ref<View> CreateView(uint32_t width, uint32_t height, bool transparent) = 0;

  // Update timers and dispatch internal callbacks
  virtual void Update() = 0;

  // Render all active views to display lists and dispatch calls to GPUDriver
  virtual void Render() = 0;

protected:
  virtual ~Renderer();
};

} // namespace ultralight
