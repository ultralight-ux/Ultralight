///
/// @file String.h
///
/// @brief The header for the String class.
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
#include <Ultralight/String8.h>
#include <Ultralight/String16.h>
#include <Ultralight/String32.h>

namespace ultralight {

///
/// @brief  UTF-16 String container with conversions for UTF-8 and UTF-32.
///
/// @note   Internally, all strings are represented as UTF-16.
///
class UExport String {
public:
  ///
  /// Create empty string
  ///
  String();

  ///
  /// Create from null-terminated, ASCII C-string
  ///
  String(const char* str);

  ///
  /// Create from raw, UTF-8 string with certain length
  ///
  String(const char* str, size_t len);

  ///
  /// Create from existing String8 (UTF-8).
  ///
  String(const String8& str);

  ///
  /// Create from raw UTF-16 string with certain length
  ///
  String(const Char16* str, size_t len);

  ///
  /// Create from existing String16 (UTF-16)
  ///
  String(const String16& str);

  ///
  /// Create from existing String32 (UTF-32)
  ///
  String(const String32& str);

  ///
  /// Copy constructor
  ///
  String(const String& other);

  ///
  /// Destructor
  ///
  ~String();

  ///
  /// Assign string from another, copy is made
  ///
  String& operator=(const String& other);

  ///
  /// Append string with another
  ///
  String& operator+=(const String& other);
  
  ///
  /// Concatenation operator
  ///
  inline friend String operator+(String lhs, const String& rhs) { lhs += rhs; return lhs; }

  ///
  /// Get native UTF-16 string
  ///
  String16& utf16() { return str_; }

  ///
  /// Get native UTF-16 string
  ///
  const String16& utf16() const { return str_; }

  ///
  /// Convert to UTF-8 string
  ///
  String8 utf8() const;

  ///
  /// Convert to UTF-32 string
  ///
  String32 utf32() const;

  ///
  /// Check if string is empty or not
  ///
  bool empty() const { return utf16().empty(); }

private:
  String16 str_;
};


}  // namespace ultralight

