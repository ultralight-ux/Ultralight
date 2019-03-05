#pragma once
#include "Defines.h"

namespace ultralight {

class AExport Monitor {
public:
  virtual ~Monitor() {}

  virtual double scale() const = 0;

  virtual int width() const = 0;

  virtual int height() const = 0;
};

}  // namespace ultralight
