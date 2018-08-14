// Copyright 2018 Ultralight, Inc. All rights reserved.
#pragma once
#include <Ultralight/Defines.h>
#include <Ultralight/Vector.h>
#include <Ultralight/String16.h>

namespace ultralight {

/**
 * String container class, natively UTF-16 with conversion to/from UTF-8
 */
class UExport String {
public:
  // Create empty string
  String();

  // Create from null-terminated ASCII string
  String(const char* str);

  // Create from UTF-8 string with certain length
  String(const char* str, size_t len);

  // Create from UTF-16 string with certain length
  String(const Char16* str, size_t len);

  // Create from existing String16
  String(const String16& str);

  // Copy constructor
  String(const String& other);

  // Destructor
  ~String();

  // Assign string from another, copy is made
  String& operator=(const String& other);

  // Append string with another
  String& operator+=(const String& other);
  
  // Concatenation operator
  inline friend String operator+(String lhs, const String& rhs) { lhs += rhs; return lhs; }

  // Get native UTF-16 string
  String16& utf16() { return str_; }
  const String16& utf16() const { return str_; }

  // Convert to UTF-8 string
  Vector<char> utf8() const;

  // Check if string is empty or not
  bool empty() const { return utf16().empty(); }

private:
  String16 str_;
};


}  // namespace ultralight

