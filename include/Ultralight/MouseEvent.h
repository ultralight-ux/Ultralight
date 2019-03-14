///
/// @file MouseEvent.h
///
/// @brief The header for the MouseEvent class.
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
/// @brief  A generic mouse event.
///
/// @note   @see View::FireMouseEvent
///
class MouseEvent {
public:
  ///
  /// The various MouseEvent types.
  ///
  enum Type {
	///
    /// Mouse moved type
	///
    kType_MouseMoved,

	///
    /// Mouse button pressed type
	///
    kType_MouseDown,

	///
    /// Mouse button released type
	///
    kType_MouseUp,
  };

  ///
  /// The various mouse button types.
  ///
  enum Button {
    kButton_None = 0,
    kButton_Left,
    kButton_Middle,
    kButton_Right,
  };

  ///
  /// The type of this MouseEvent
  ///
  Type type;

  ///
  /// The current x-position of the mouse, relative to the View
  ///
  int x;

  ///
  /// The current y-position of the mouse, relative to the View
  ///
  int y;

  ///
  /// The mouse button that was pressed/released, if any.
  ///
  Button button;
};

}  // namespace ultralight
