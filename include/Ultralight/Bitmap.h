// Copyright 2018 Ultralight, Inc. All rights reserved.
#pragma once
#include <Ultralight/Defines.h>
#include <Ultralight/RefPtr.h>
#include <Ultralight/Geometry.h>

namespace ultralight {

#pragma pack(push, 1)

enum UExport BitmapFormat {
  kBitmapFormat_A8,
  kBitmapFormat_RGBA8
};

#define GetBytesPerPixel(x) (x == kBitmapFormat_A8? 1 : 4)

/**
 * Ref-Counted Bitmap container with basic blitting and conversion routines.
 */
class UExport Bitmap : public RefCounted {
 public:
  // Make empty Bitmap
  static Ref<Bitmap> Create();

  // Make Bitmap with certain configuration
  static Ref<Bitmap> Create(uint32_t width, uint32_t height, BitmapFormat format);

  // Make Bitmap with existing pixels and configuration
  static Ref<Bitmap> Create(uint32_t width, uint32_t height, BitmapFormat format,
         uint32_t row_bytes, const void* pixels, size_t size, bool owns_pixels = true);

  // Make a deep copy
  static Ref<Bitmap> Create(const Bitmap& bitmap);

  // Accessors
  virtual uint32_t width() const = 0;
  virtual uint32_t height() const = 0;
  virtual IntRect bounds() const = 0;
  virtual BitmapFormat format() const = 0;
  virtual uint32_t bpp() const = 0;
  virtual uint32_t row_bytes() const = 0;
  virtual size_t size() const = 0;
  virtual void* pixels() = 0;
  virtual const void* pixels() const = 0;
  virtual bool owns_pixels() const = 0;

  // Member Functions
  virtual bool IsEmpty() const = 0;
  virtual void Erase() = 0;
  virtual void Set(Ref<Bitmap> bitmap) = 0;

  /** 
   * Draws src bitmap to this bitmap.
   *
   * @param src_rect    The source rectangle, relative to src bitmap.
   * @param dest_rect   The destination rectangle, relative to this bitmap.
   * @param src         The source bitmap.
   * @param pad_repeat  Whether or not we should pad the drawn bitmap by 1 pixel
   *                    of repeated edge pixels from the source bitmap.
   */
  virtual bool DrawBitmap(IntRect src_rect, IntRect dest_rect,
    Ref<Bitmap> src, bool pad_repeat) = 0;

  // Write this bitmap out to a PNG image.
  virtual bool WritePNG(const char* path) = 0;

protected:
  Bitmap();
  virtual ~Bitmap();
  Bitmap(const Bitmap&);
  void operator=(const Bitmap&);
};

#pragma pack(pop)

}  // namespace ultralight
