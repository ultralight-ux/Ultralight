///
/// @file Platform.h
///
/// @brief The header for the Platform singleton.
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

namespace ultralight {

struct Config;
class GPUDriver;
class FontLoader;
class FileSystem;

///
/// @brief  Platform singleton to configure Ultralight and provide user-defined
///         implementations for various platform operations.
///
/// @note  All of these settings and user-defined interfaces should be set
///        BEFORE creating the Renderer.
///
///        A default GPUDriver and FontLoader are provided, you must provide
///        your own FileSystem interface if you want to load local files.
///
///        For more info about the defaults @see DefaultGPUDriver and
///        @see DefaultFontLoader.
///
class UExport Platform {
 public:
  ///
  /// Get the Platform singleton
  ///
  static Platform& instance();

  virtual ~Platform();

  ///
  /// Set the Config
  ///
  virtual void set_config(const Config& config) = 0;

  ///
  /// Get the Config
  ///
  virtual const Config& config() const = 0;

  ///
  /// Set the GPU Driver (will handle all rendering)
  ///
  /// @param  gpu_driver  A user-defined GPUDriver implementation, ownership
  ///                     remains with the caller.
  ///
  /// @note  If you never call this, the default driver will be used instead.
  ///        @see DefaultGPUDriver
  ///
  virtual void set_gpu_driver(GPUDriver* gpu_driver) = 0;

  ///
  /// Get the GPU Driver
  ///
  virtual GPUDriver* gpu_driver() const = 0;

  ///
  /// Set the Font Loader (will be used to map font families to actual fonts)
  ///
  /// @param  font_loader  A user-defined FontLoader implementation, ownership
  ///                      remains with the caller.
  ///
  /// @note  If you never call this, the default font loader will be used
  ///        instead. @see DefaultFontLoader
  ///
  virtual void set_font_loader(FontLoader* font_loader) = 0;

  ///
  /// Get the Font Loader
  ///
  virtual FontLoader* font_loader() const = 0;

  ///
  /// Set the File System (will be used for all file system operations)
  ///
  /// @param  file_system  A user-defined FileSystem implementation, ownership
  ///                      remains with the caller.
  ///
  virtual void set_file_system(FileSystem* file_system) = 0;

  ///
  /// Get the File System
  ///
  virtual FileSystem* file_system() const = 0;
};

///
/// The default GPU driver is an OpenGL driver that paints each View to an
/// offscreen bitmap (@see View::bitmap). This is lazily-initialized.
///
UExport GPUDriver* DefaultGPUDriver();

///
/// The default Font Loader uses the native font loader API for the platform.
/// 
/// @note  Windows and macOS font loading is implemented but Linux is still in
///        progress-- currently just loads an embedded Roboto font.
///
UExport FontLoader* DefaultFontLoader();

}  // namespace ultralight
