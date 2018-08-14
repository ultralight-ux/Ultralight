// Copyright 2018 Ultralight, Inc. All rights reserved.
#pragma once
#include <Ultralight/Defines.h>
#include <Ultralight/String16.h>
#include <Ultralight/Buffer.h>

namespace ultralight {

/**
* Platform FontLoader interface, used to load font data by font description.
*
* This is intended to be implemented by embedders and defined at application
* start via Platform::set_font_loader
*/
class UExport FontLoader {
public:
  virtual ~FontLoader();

  // Last resort fallback font family name.
  virtual String16 fallback_font() const = 0;

  /**
   * Load font with a certain font description.
   *
   * @param  family  Font family name.
   * @param  weight  Font weight. 
   * @param  italic  Whether or not italic is requested.
   * @param  size    Font size.
   * 
   * @return Return a buffer of bytes containing the loaded font file contents.
   */
  virtual Ref<Buffer> Load(const String16& family, int weight, bool italic, float size) = 0;
};

}  // namespace ultralight
