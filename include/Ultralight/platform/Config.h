// Copyright 2018 Ultralight, Inc. All rights reserved.
#pragma once
#include <Ultralight/Defines.h>
#include <Ultralight/String16.h>

namespace ultralight {

/**
 * The winding order for front-facing triangles
 */
enum FaceWinding {
  // Clockwise Winding (Direct3D, etc.)
  kFaceWinding_Clockwise,

  // Counter-Clockwise Winding (OpenGL, etc.)
  kFaceWinding_CounterClockwise,
};

/**
 * Global config for the renderer. You should create an instance of this class
 * and pass it to Platform::set_config() before creating the Renderer or Views.
 */
struct UExport Config {
  // The winding order for front-facing triangles
  FaceWinding face_winding = kFaceWinding_CounterClockwise;

  // Whether images should be enabled.
  bool enable_images = true;

  // Whether JavaScript should be enabled
  bool enable_javascript = true;

  // Whether to use signed distance field (SDF) fonts or raster fonts.
  // Raster fonts are recommended for when the device scale is 1.0
  // because the glyph shapes are hinted to the pixel grid.
  bool use_distance_field_fonts = false;

  // Whether to use signed distance field (SDF) paths or raster paths.
  bool use_distance_field_paths = false;

  // When using the default, offscreen GPU driver, whether or not we
  // should use BGRA byte order (instead of RGBA). See View::bitmap().
  bool use_bgra_for_offscreen_rendering = false;

  // The amount that the application DPI has been scaled (200% = 2.0).
  // Used for oversampling raster shapes.
  double device_scale_hint = 1.0;

  // Default font-family to use.
  String16 font_family_standard = "Times New Roman";

  // Default font-family to use for fixed fonts. (pre/code)
  String16 font_family_fixed = "Courier New";

  // Default font-family to use for serif fonts.
  String16 font_family_serif = "Times New Roman";

  // Default font-family to use for sans-serif fonts.
  String16 font_family_sans_serif = "Arial";

  // Default user-agent string
  String16 user_agent = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/602.1 (KHTML, like Gecko) Ultralight/0.9.2 Safari/602.1";

  // Default user stylesheet (CSS)
  String16 user_stylesheet;
};

}  // namespace ultralight
