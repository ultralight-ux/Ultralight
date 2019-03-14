///
/// @file ScrollEvent.h
///
/// @brief The header for the ScrollEvent class.
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

namespace ultralight {

///
/// @brief  A generic scroll event.
///
/// @note   @see View::FireScrollEvent
///
class ScrollEvent {
public:
  ///
  /// The scroll event granularity type
  ///
  enum Type {
	  /// The delta value is interpreted as number of pixels
    kType_ScrollByPixel,

	  /// The delta value is interpreted as number of pages
    kType_ScrollByPage,
  };

  ///
  /// Scroll granularity type
  ///
  Type type;

  ///
  /// Horizontal scroll amount
  ///
  int delta_x;

  ///
  /// Vertical scroll amount
  ///
  int delta_y;
};

}  // namespace ultralight
