// Copyright 2018 Ultralight, Inc. All rights reserved.
#pragma once
#include <Ultralight/Defines.h>
#include <Ultralight/RefPtr.h>

namespace ultralight {

/**
 * Ref-counted byte buffer.
 */
class UExport Buffer : public RefCounted {
public:
  // Create a Buffer, a deep copy of data is made.
  static Ref<Buffer> Create(const void* data, size_t size);

  // Get raw data bytes. (owned by Buffer)
  virtual void* data() = 0;

  // Size in bytes.
  virtual size_t size() const = 0;

protected:
  Buffer();
  virtual ~Buffer();
  Buffer(const Buffer&);
  void operator=(const Buffer&);
};

}  // namespace ultralight
