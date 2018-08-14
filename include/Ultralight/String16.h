// Copyright 2018 Ultralight, Inc. All rights reserved.
#pragma once
#include <Ultralight/Defines.h>
#include <stddef.h>

namespace ultralight {

namespace detail {
  template<int> struct selector;
  template<> struct selector<4> { typedef char16_t Char16; };
  template<> struct selector<2> { typedef wchar_t Char16; };
}  // namespace detail

#ifdef DISABLE_NATIVE_WCHAR_T
// Force Char16 type to use char16_t, used on Windows when native wchar_t support is disabled.
typedef char16_t Char16;
#else
// We use wchar_t if size == 2, otherwise use char16_t
typedef detail::selector<sizeof(wchar_t)>::Char16 Char16;
#endif

/**
 * UTF-16 String container class.
 */
class UExport String16 {
public:
  // Make an empty String16
  String16();

  // Make a String16 from null-terminated C-string
  String16(const char* c_str);

  // Make a String16 from C-string with certain length
  String16(const char* c_str, size_t len);

  // Make a String16 from raw Char16 buffer with certain length
  String16(const Char16* str, size_t len);

  // Make a String16 from raw unsigned short buffer with certain length. Useful on Windows when
  // native support for wchar_t is disabled (eg, /Zc:wchar_t-).
  String16(const unsigned short* str, size_t len);

  // Make a deep copy of String16
  String16(const String16& other);

  ~String16();

  // Assign a String16 to this one, deep copy is made
  String16& operator=(const String16& other);

  // Append a String16 to this one.
  String16& operator+=(const String16& other);

  // Concatenation operator
  inline friend String16 operator+(String16 lhs, const String16& rhs) { lhs += rhs; return lhs; }

  // Get raw Char16 data
  const Char16* data() const { return data_; }

  // Get raw data (as unsigned short). This is useful on Windows if you compile without native
  // support for wchar_t (eg, /Zc:wchar_t-)
  const unsigned short* udata() const { return reinterpret_cast<const unsigned short*>(data_); }

  // Get length in characters.
  size_t length() const { return length_; }

  // Check if string is empty.
  bool empty() const { return !data_ || length_ == 0; }

private:
  Char16* data_;
  size_t length_;
};

}  // namespace ultralight
