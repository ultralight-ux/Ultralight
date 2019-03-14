///
/// @file Renderer.h
///
/// @brief The header for the Renderer class.
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
#include <Ultralight/View.h>

namespace ultralight {

///
/// @brief  The core of Ultralight. You should initialize it after setting up
///         your Platform config and drivers.
///
/// This singleton class manages the lifetime of all Views (@see View) and
/// coordinates all painting, rendering, network requests, and event dispatch.
///
/// @note  You don't have to create this instance directly if you use the
///        AppCore API. The App class will automatically create a Renderer and
///        perform all rendering within its run loop. @see App::Create
///
class UExport Renderer : public RefCounted {
public:
  ///
  /// Create the Renderer singleton. You should set up all your Platform config,
  /// file-system, and drivers before calling this function. @see Platform
  ///
  /// @note  You should only create one Renderer per application lifetime.
  ///
  /// @return  Returns a ref-pointer to a new Renderer instance. You should
  ///          assign it to either a Ref<Renderer> (non-nullable) or
  ///          RefPtr<Renderer> (nullable).
  ///
  static Ref<Renderer> Create();

  ///
  /// Create a new View.
  ///
  /// @param  width   The initial width, in device coordinates.
  /// 
  /// @param  height  The initial height, in device coordinates.
  ///
  /// @param  transparent  Whether or not the view background is transparent.
  ///
  /// @return  Returns a ref-pointer to a new View instance. You should assign
  ///          it to either a Ref<View> (non-nullable) or RefPtr<View>
  ///          (nullable).
  ///
  /// @note  The device coordinates are scaled to pixels by multiplying them
  ///        with the current DPI scale (@see Config::device_scale_hint) and
  ///        rounding to the nearest integer value.
  ///
  virtual Ref<View> CreateView(uint32_t width, uint32_t height,
	                           bool transparent) = 0;

  ///
  /// Update timers and dispatch internal callbacks. You should call this often
  /// from your main application loop.
  ///
  virtual void Update() = 0;

  ///
  /// Render all active views to display lists and dispatch calls to GPUDriver.
  ///
  /// @note  If you're using the default, offscreen GL driver, this updates the
  ///        internal bitmap of each View (@see View::bitmap).
  ///
  virtual void Render() = 0;

  ///
  /// Attempt to release as much memory as possible. Don't call this from any
  /// callbacks or driver code.
  ///
  virtual void PurgeMemory() = 0;

protected:
  virtual ~Renderer();
};

} // namespace ultralight
