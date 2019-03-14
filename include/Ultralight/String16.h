///
/// @file String16.h
///
/// @brief The header for the String16 class.
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
#include <Ultralight/RefPtr.h>
#include <stddef.h>

namespace ultralight {

namespace detail {
  template<int> struct selector;
  template<> struct selector<4> { typedef char16_t Char16; };
  template<> struct selector<2> { typedef wchar_t Char16; };
}

#ifdef DISABLE_NATIVE_WCHAR_T
// Force Char16 type to use char16_t, used on Windows when native wchar_t support is disabled.
typedef char16_t Char16;
#else
// We use wchar_t if size == 2, otherwise use char16_t
typedef detail::selector<sizeof(wchar_t)>::Char16 Char16;
#endif

///
/// @brief  A UTF-16 string container.
///
class UExport String16 {
public:
  // Make an empty String16
  String16();

  // Make a String16 from null-terminated ASCII C-string
  String16(const char* c_str);

  // Make a String16 from ASCII C-string with certain length
  String16(const char* c_str, size_t len);

  // Make a String16 from raw UTF-16 buffer with certain length
  String16(const Char16* str, size_t len);

  // Make a String16 from raw unsigned short UTF-16 buffer with certain length. Useful on Windows
  // when native support for wchar_t is disabled (eg, /Zc:wchar_t-).
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

  // Get raw UTF-16 data
  Char16* data() { return data_; }

  // Get raw UTF-16 data (const)
  const Char16* data() const { return data_; }

  // Get raw UTF-16 data as unsigned short. This is useful on Windows if you compile without native
  // support for wchar_t (eg, /Zc:wchar_t-)
  unsigned short* udata() { return reinterpret_cast<unsigned short*>(data_); }

  // Get raw UTF-16 data as unsigned short (const).
  const unsigned short* udata() const { return reinterpret_cast<const unsigned short*>(data_); }

  // Get length in characters.
  size_t length() const { return length_; }

  // Get size in characters (synonym for length)
  size_t size() const { return length_; }

  // Check if string is empty.
  bool empty() const { return !data_ || length_ == 0; }

  // Get character at specific position
  Char16& operator[](size_t pos) { return data_[pos]; }

  // Get character at specific position (const)
  const Char16& operator[](size_t pos) const { return data_[pos]; }

private:
  Char16* data_;
  size_t length_;
};

///
/// @brief  A UTF-16 string vector.
///
class UExport String16Vector : public RefCounted {
public:
  // Create an empty string vector
  static Ref<String16Vector> Create();

  // Create a string vector from an existing array (a deep copy is made)
  static Ref<String16Vector> Create(const String16* stringArray, size_t len);

  // Add an element to the back of the string vector
  virtual void push_back(const String16& val) = 0;

  // Get raw String16 vector array
  virtual String16* data() = 0;

  // Get the number of elements in vector
  virtual size_t size() const = 0;

protected:
  String16Vector();
  virtual ~String16Vector();
  String16Vector(const String16Vector&);
  void operator=(const String16Vector&);
};

}  // namespace ultralight
