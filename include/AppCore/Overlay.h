///
/// @file Overlay.h
///
/// @brief The header for the Overlay class.
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
#include "Window.h"
#include <Ultralight/View.h>

namespace ultralight {

///
/// Web-content overlay. Displays a web-page within an area of the main window.
///
/// @note  Each Overlay is essentially a View and an on-screen quad. You should
///        create the Overlay then load content into the underlying View.
///
class AExport Overlay : public RefCounted {
public:
  ///
  /// Create a new Overlay.
  ///
  /// @param  width   The width in device coordinates.
  ///
  /// @param  height  The height in device coordinates.
  ///
  /// @param  x       The x-position (offset from the left of the Window), in
  ///                 device coordinates.
  ///
  /// @param  y       The y-position (offset from the top of the Window), in
  ///                 device coordinates.
  ///
  static Ref<Overlay> Create(int width, int height, int x, int y);

  ///
  /// Get the underlying View.
  ///
  virtual ultralight::Ref<ultralight::View> view() = 0;

  ///
  /// Get the width (in device coordinates).
  ///
  virtual int width() const = 0;

  ///
  /// Get the height (in device coordinates).
  ///
  virtual int height() const = 0;
  
  ///
  /// Get the x-position (offset from the left of the Window), in device
  /// coordinates.
  ///
  virtual int x() const = 0;

  ///
  /// Get the y-position (offset from the top of the Window), in device
  /// coordinates.
  ///
  virtual int y() const = 0;

  ///
  /// Move the overlay to a new position (in device coordinates).
  ///
  virtual void MoveTo(int x, int y) = 0;

  ///
  /// Resize the overlay (and underlying View), dimensions should be
  /// specified in device coordinates.
  ///
  virtual void Resize(int width, int height) = 0;

protected:
  virtual ~Overlay();
  virtual void Draw() = 0;
  friend class OverlayManager;
};

}  // namespace framework
