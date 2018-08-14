// Copyright 2018 Ultralight, Inc. All rights reserved.
#pragma once
#include <Ultralight/Defines.h>

namespace ultralight {

/**
 * Generic mouse event class. Used with View::FireMouseEvent
 */
class MouseEvent {
public:
  // An enumeration of the different MouseEvent types.
  enum Type {
    // Type: Mouse moved
    kType_MouseMoved,

    // Type: Mouse button pressed
    kType_MouseDown,

    // Type: Mouse button released
    kType_MouseUp,
  };

  // An enumeration of the different button types.
  enum Button {
    kButton_None = 0,
    kButton_Left,
    kButton_Middle,
    kButton_Right,
  };

  // The type of this MouseEvent
  Type type;

  // The current x-position of the mouse, relative to the View
  int x;

  // The current y-position of the mouse, relative to the View
  int y;

  // The button that was pressed/released, if any.
  Button button;
};

}  // namespace ultralight
