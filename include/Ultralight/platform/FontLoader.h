///
/// @file FontLoader.h
///
/// @brief The header for the FontLoader interface.
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
#include <Ultralight/Buffer.h>

namespace ultralight {

///
/// @brief  Font Loader interface, used for all font lookup operations.
///
/// Every operating system has its own library of installed system fonts. The
/// FontLoader interface is used to lookup these fonts and fetch the actual
/// font data (raw TTF/OTF file data) given a certain font description.
///
/// You can override this interface to bundle your own fonts or override the
/// default system font loading behavior.
///
/// This is intended to be implemented by users and defined before creating the
/// Renderer. @see Platform::set_font_loader
///
class UExport FontLoader {
public:
  virtual ~FontLoader();

  ///
  /// Fallback font family name. Will be used if all other fonts fail to load.
  ///
  /// @note  This font should be GUARANTEED to exist (eg, FontLoader::Load
  ///        won't fail when passed this font family name).
  ///
  virtual String16 fallback_font() const = 0;

  ///
  /// Fallback font family name that can render the specified characters. This
  /// is mainly used to support CJK (Chinese, Japanese, Korean) text display.
  ///
  /// @param  characters  One or more UTF-16 characters. This is almost ALWAYS
  ///                     a single character.
  ///
  /// @param  weight      Font weight.
  ///
  /// @param  italic      Whether or not italic is requested.
  /// 
  /// @param  size        Font size.
  ///
  /// @return  Should return a font family name that can render the text.
  ///
  virtual String16 fallback_font_for_characters(const String16& characters,
                                                int weight, bool italic,
	                                            float size) const = 0;

  ///
  /// Get the actual font file data (TTF/OTF) for a given font description.
  /// 
  /// @param  family  Font family name.
  ///
  /// @param  weight  Font weight. 
  ///
  /// @param  italic  Whether or not italic is requested.
  ///
  /// @param  size    Font size.
  ///
  /// @return  A buffer of bytes containing raw font file contents of the
  ///          font matching the given description.
  ///
  /// @note    As an example of usage, when given the family "Arial", this
  ///          function should return the raw file contents for "ARIAL.TTF"
  ///
  virtual Ref<Buffer> Load(const String16& family, int weight, bool italic,
                           float size) = 0;
};

}  // namespace ultralight
