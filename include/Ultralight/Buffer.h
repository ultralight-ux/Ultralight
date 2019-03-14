///
/// @file Buffer.h
///
/// @brief The header for the Buffer class.
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

namespace ultralight {

///
/// A fixed-size byte container for passing data around.
///
class UExport Buffer : public RefCounted {
public:
  ///
  /// Create a Buffer, a copy of data is made.
  ///
  static Ref<Buffer> Create(const void* data, size_t size);

  ///
  /// Get a pointer to raw byte data.
  ///
  virtual void* data() = 0;

  ///
  /// Get the size in bytes.
  ///
  virtual size_t size() const = 0;

protected:
  Buffer();
  virtual ~Buffer();
  Buffer(const Buffer&);
  void operator=(const Buffer&);
};

}  // namespace ultralight
