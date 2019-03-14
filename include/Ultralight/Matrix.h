///
/// @file Matrix.h
///
/// @brief The header for Matrix helpers
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
#include <Ultralight/Geometry.h>

namespace ultralight {

///
/// Affine Matrix helper
///
struct UExport Matrix {
  ///
  /// Raw affine matrix as an array
  ///
  float data[6];

  ///
  /// Set to identity matrix
  ///
  void SetIdentity();

  ///
  /// Set to another matrix
  ///
  void Set(const Matrix& other);
  
  ///
  /// Set from raw affine members
  ///
  void Set(float a, float b, float c, float d, float e, float f);

  ///
  /// Whether or not this is an identity matrix
  ///
  bool IsIdentity() const;

  ///
  /// Whether or not this is an identity matrix or translation
  ///
  bool IsIdentityOrTranslation() const;

  ///
  /// Whether or not this is an identity, translation, or non-negative
  /// uniform scale
  ///
  bool IsSimple() const;

  ///
  /// Translate by x and y
  ///
  void Translate(float x, float y);

  ///
  /// Scale by x and y
  ///
  void Scale(float x, float y);

  /// 
  /// Rotate matrix by theta (in degrees)
  ///
  void Rotate(float theta);

  ///
  /// Rotate matrix by x and y
  ///
  void Rotate(float x, float y);

  ///
  /// Transform (multiply) by another Matrix
  ///
  void Transform(const Matrix& other);

  ///
  /// Get the inverse of this matrix. May return false if not invertible.
  ///
  bool GetInverse(Matrix& result) const;

  ///
  /// Transform point by this matrix and get the result.
  ///
  Point Apply(const Point& p) const;

  ///
  /// Transform rect by this matrix and get the result as an axis-aligned rect.
  ///
  Rect Apply(const Rect& r) const;

  ///
  /// Get an integer hash of this matrix's members.
  ///
  uint32_t Hash() const;
};

///
/// 4x4 Matrix Helper
///
struct UExport Matrix4x4 {
  ///
  /// Raw 4x4 matrix as an array
  ///
  float data[16];

  ///
  /// Set to identity matrix.
  ///
  void SetIdentity();
};

///
/// Convert affine matrix to a 4x4 matrix.
///
Matrix4x4 UExport ConvertAffineTo4x4(const Matrix& mat);

bool UExport operator==(const Matrix& a, const Matrix& b);
bool UExport operator!=(const Matrix& a, const Matrix& b);

bool UExport operator==(const Matrix4x4& a, const Matrix4x4& b);
bool UExport operator!=(const Matrix4x4& a, const Matrix4x4& b);

}  // namespace ultralight
