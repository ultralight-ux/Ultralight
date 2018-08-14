// Copyright 2018 Ultralight, Inc. All rights reserved.
#pragma once
#include <Ultralight/Defines.h>
#include <Ultralight/RefPtr.h>
#include <Ultralight/Geometry.h>

namespace ultralight {

/**
 * Affine Matrix convenience class
 */
struct UExport Matrix {
  float data[6];

  void SetIdentity();
  void Set(const Matrix& other);
  void Set(float a, float b, float c, float d, float e, float f);

  // Accessors
  bool IsIdentity() const;
  bool IsIdentityOrTranslation() const;

  // Returns true if is identity, translation, or non-negative uniform scale
  bool IsSimple() const;

  // Transformations
  void Translate(float x, float y);
  void Scale(float x, float y);
  void Rotate(float theta);
  void Rotate(float x, float y);
  void Transform(const Matrix& other);

  // Inverse, (may return false if matrix is not invertible)
  bool GetInverse(Matrix& result) const;

  // Apply To Geometry Helpers
  Point Apply(const Point& p) const;
  Rect Apply(const Rect& r) const;

  // Hashing
  uint32_t Hash() const;
};

/**
 * 4x4 Matrix convenience class
 */
struct UExport Matrix4x4 {
  float data[16];

  void SetIdentity();
};

Matrix4x4 UExport ConvertAffineTo4x4(const Matrix& mat);

bool UExport operator==(const Matrix& a, const Matrix& b);
bool UExport operator!=(const Matrix& a, const Matrix& b);

bool UExport operator==(const Matrix4x4& a, const Matrix4x4& b);
bool UExport operator!=(const Matrix4x4& a, const Matrix4x4& b);

}  // namespace ultralight
