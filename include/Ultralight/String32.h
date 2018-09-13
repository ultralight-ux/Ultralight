// Copyright 2018 Ultralight, Inc. All rights reserved.
#pragma once
#include <Ultralight/Defines.h>
#include <stddef.h>

namespace ultralight {

/**
* UTF-32 String container class.
*/
class UExport String32 {
public:
  // Make an empty String32
  String32();

  // Make a String32 from raw UTF-32 string with certain length
  String32(const char32_t* c_str, size_t len);

  // Make a deep copy of String32
  String32(const String32& other);

  ~String32();

  // Assign a String32 to this one, deep copy is made
  String32& operator=(const String32& other);

  // Append a String32 to this one.
  String32& operator+=(const String32& other);

  // Concatenation operator
  inline friend String32 operator+(String32 lhs, const String32& rhs) { lhs += rhs; return lhs; }

  // Get raw UTF-32 data
  char32_t* data() { return data_; }

  // Get raw UTF-32 data (const)
  const char32_t* data() const { return data_; }

  // Get length in characters.
  size_t length() const { return length_; }

  // Get size in characters (synonym for length)
  size_t size() const { return length_; }

  // Check if string is empty.
  bool empty() const { return !data_ || length_ == 0; }

  // Get character at specific position
  char32_t& operator[](size_t pos) { return data_[pos]; }

  // Get character at specific position (const)
  const char32_t& operator[](size_t pos) const { return data_[pos]; }

private:
  char32_t* data_;
  size_t length_;
};

}  // namespace ultralight
