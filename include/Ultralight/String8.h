// Copyright 2018 Ultralight, Inc. All rights reserved.
#pragma once
#include <Ultralight/Defines.h>
#include <stddef.h>

namespace ultralight {

/**
* UTF-8 String container class.
*/
class UExport String8 {
public:
  // Make an empty String8
  String8();

  // Make a String8 from raw, null-terminated UTF-8 string
  String8(const char* c_str);

  // Make a String8 from raw UTF-8 string with certain length
  String8(const char* c_str, size_t len);

  // Make a deep copy of String8
  String8(const String8& other);

  ~String8();

  // Assign a String8 to this one, deep copy is made
  String8& operator=(const String8& other);

  // Append a String8 to this one.
  String8& operator+=(const String8& other);

  // Concatenation operator
  inline friend String8 operator+(String8 lhs, const String8& rhs) { lhs += rhs; return lhs; }

  // Get raw UTF-8 data
  char* data() { return data_; }

  // Get raw UTF-8 data (const)
  const char* data() const { return data_; }

  // Get length in characters.
  size_t length() const { return length_; }

  // Get size in characters (synonym for length)
  size_t size() const { return length_; }

  // Check if string is empty.
  bool empty() const { return !data_ || length_ == 0; }

  // Get character at specific position
  char& operator[](size_t pos) { return data_[pos]; }

  // Get character at specific position (const)
  const char& operator[](size_t pos) const { return data_[pos]; }

private:
  char* data_;
  size_t length_;
};

}  // namespace ultralight
