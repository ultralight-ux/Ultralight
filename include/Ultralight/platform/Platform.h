// Copyright 2018 Ultralight, Inc. All rights reserved.
#pragma once
#include <Ultralight/Defines.h>

namespace ultralight {

struct Config;
class GPUDriver;
class FontLoader;
class FileSystem;

/**
 * This singleton allows users to configure the library and provide
 * platform-specific implementations of common platform tasks.
 *
 * @note  At a minimum, users should set the Config and provide a GPUDriver
 *        and FontLoader. FileSystem is optional.
 */
class UExport Platform {
 public:
  // Get the Platform singleton
  static Platform& instance();

  virtual ~Platform();

  // Set the Config
  virtual void set_config(const Config& config) = 0;
  virtual const Config& config() const = 0;

  // Set the GPUDriver implementation (owned by caller)
  virtual void set_gpu_driver(GPUDriver* gpu_driver) = 0;
  virtual GPUDriver* gpu_driver() const = 0;

  // Set the FontLoader implementation (owned by caller)
  virtual void set_font_loader(FontLoader* font_loader) = 0;
  virtual FontLoader* font_loader() const = 0;

  // Set the FileSystem implementation (owned by caller)
  virtual void set_file_system(FileSystem* file_system) = 0;
  virtual FileSystem* file_system() const = 0;
};

}  // namespace ultralight
