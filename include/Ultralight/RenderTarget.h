///
/// @file RenderTarget.h
///
/// @brief The header for the RenderTarget struct.
///
/// @author
///
/// This file is a part of Ultralight, a fast, lightweight, HTML UI engine
///
/// Website: <http://ultralig.ht>
///
/// Copyright (C) 2019 Ultralight, Inc. All rights reserved.
///
#pragma once
#include <Ultralight/Defines.h>
#include <Ultralight/Bitmap.h>
#include <Ultralight/Geometry.h>

namespace ultralight {

#pragma pack(push, 1)

///
/// @brief  Rendering details for a View, to be used with your own GPUDriver
///
/// When using your own GPUDriver, each View is rendered to an offscreen
/// texture that you must display on a quad in your engine. This struct
/// provides all the details you need to render the View texture in your
/// engine.
///
struct UExport RenderTarget {
  ///
  /// Whether this target is empty (null texture)
  ///
  bool is_empty;

  ///
  /// The viewport width (in device coordinates).
  ///
  uint32_t width;

  ///
  /// The viewport height (in device coordinates).
  ///
  uint32_t height;

  ///
  /// The GPUDriver-specific texture ID (you should bind the texture using
  /// your implementation of GPUDriver::BindTexture before drawing a quad).
  ///
  uint32_t texture_id;

  ///
  /// The texture width (in pixels). This may be padded.
  ///
  uint32_t texture_width;

  ///
  /// The texture height (in pixels). This may be padded.
  ///
  uint32_t texture_height;

  ///
  /// The pixel format of the texture.
  ///
  BitmapFormat texture_format;

  ///
  /// UV coordinates of the texture (this is needed because the texture may
  /// be padded).
  ///
  Rect uv_coords;

  ///
  /// The GPUDriver-specific render buffer ID.
  ///
  uint32_t render_buffer_id;

  RenderTarget();
};

#pragma pack(pop)

}  // namespace ultralight
