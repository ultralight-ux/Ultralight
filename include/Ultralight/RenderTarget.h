// Copyright 2018 Ultralight, Inc. All rights reserved.
#pragma once
#include <Ultralight/Defines.h>
#include <Ultralight/Bitmap.h>

namespace ultralight {

#pragma pack(push, 1)

/**
 * Render target description, used by View::render_target.
 */
struct UExport RenderTarget {
  bool is_empty;
  uint32_t width;
  uint32_t height;
  uint32_t texture_id;
  uint32_t texture_width;
  uint32_t texture_height;
  BitmapFormat texture_format;
  uint32_t render_buffer_id;

  RenderTarget();
};

#pragma pack(pop)

}  // namespace ultralight
