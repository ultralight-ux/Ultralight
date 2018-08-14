// Copyright 2018 Ultralight, Inc. All rights reserved.
#pragma once
#include <Ultralight/Defines.h>

namespace ultralight {

/**
 * Generic scroll event. Used with View::FireScrollEvent
 */
class ScrollEvent {
public:
  // The scroll event granularity type
  enum Type {
    kType_ScrollByPixel,
    kType_ScrollByPage,
  };

  // Scroll granularity type
  Type type;

  // Horizontal scroll amount
  int delta_x;

  // Vertical scroll amount
  int delta_y;
};

}  // namespace ultralight
