///
/// @file Config.h
///
/// @brief The header for the Config struct.
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
#include <Ultralight/String16.h>

namespace ultralight {

///
/// The winding order for front-facing triangles.
///
/// @note  In most 3D engines, there is the concept that triangles have a
///        a "front" and a "back". All the front-facing triangles (eg, those
///        that are facing the camera) are rendered, and all back-facing
///        triangles are culled (ignored). The winding-order of the triangle's
///        vertices is used to determine which side is front and back. You
///        should tell Ultralight which winding-order your 3D engine uses.
///
enum FaceWinding {
  ///
  /// Clockwise Winding (Direct3D, etc.)
  ///
  kFaceWinding_Clockwise,

  ///
  /// Counter-Clockwise Winding (OpenGL, etc.)
  ///
  kFaceWinding_CounterClockwise,
};

///
/// @brief  Configurations settings for Ultralight.         
///
/// This is intended to be implemented by users and defined before creating the
/// Renderer. @see Platform::set_config.
///
struct UExport Config {
  ///
  /// The winding order for front-facing triangles. @see FaceWinding
  ///
  FaceWinding face_winding = kFaceWinding_CounterClockwise;

  ///
  /// Whether or not images should be enabled.
  ///
  bool enable_images = true;

  ///
  /// Whether or not JavaScript should be enabled
  ///
  bool enable_javascript = true;

  ///
  /// When using the default, offscreen GPU driver, whether or not we
  /// should use BGRA byte order (instead of RGBA). @see View::bitmap
  ///
  bool use_bgra_for_offscreen_rendering = false;

  ///
  /// The amount that the application DPI has been scaled (200% = 2.0).
  /// Used for scaling device coordinates to pixels and oversampling raster
  /// shapes.
  ///
  double device_scale_hint = 1.0;

  ///
  /// Default font-family to use.
  ///
  String16 font_family_standard = "Times New Roman";

  ///
  /// Default font-family to use for fixed fonts. (pre/code)
  ///
  String16 font_family_fixed = "Courier New";

  ///
  /// Default font-family to use for serif fonts.
  ///
  String16 font_family_serif = "Times New Roman";

  ///
  /// Default font-family to use for sans-serif fonts.
  ///
  String16 font_family_sans_serif = "Arial";

  ///
  /// Default user-agent string.
  ///
  String16 user_agent = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) "
                          "AppleWebKit/602.1 (KHTML, like Gecko) "
                          "Ultralight/0.9.2 Safari/602.1";

  ///
  /// Default user stylesheet. You should set this to your own custom CSS
  /// string to define default styles for various DOM elements, scrollbars,
  /// and platform input widgets.
  ///
  String16 user_stylesheet;
};

}  // namespace ultralight
