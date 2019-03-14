///
/// @file Listener.h
///
/// @brief The header for View listener interfaces.
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
#include <Ultralight/String.h>

namespace ultralight {

class View;

///
/// MessageSource types, @see ViewListener::OnAddConsoleMessage
///
enum MessageSource {
  kMessageSource_XML = 0,
  kMessageSource_JS,
  kMessageSource_Network,
  kMessageSource_ConsoleAPI,
  kMessageSource_Storage,
  kMessageSource_AppCache,
  kMessageSource_Rendering,
  kMessageSource_CSS,
  kMessageSource_Security,
  kMessageSource_ContentBlocker,
  kMessageSource_Other,
};

///
/// MessageLevel types, @see ViewListener::OnAddConsoleMessage
///
enum MessageLevel {
  kMessageLevel_Log = 1,
  kMessageLevel_Warning = 2,
  kMessageLevel_Error = 3,
  kMessageLevel_Debug = 4,
  kMessageLevel_Info = 5,
};

///
/// Cursor types, @see ViewListener::OnChangeCursor
///
enum Cursor {
  kCursor_Pointer = 0,
  kCursor_Cross,
  kCursor_Hand,
  kCursor_IBeam,
  kCursor_Wait,
  kCursor_Help,
  kCursor_EastResize,
  kCursor_NorthResize,
  kCursor_NorthEastResize,
  kCursor_NorthWestResize,
  kCursor_SouthResize,
  kCursor_SouthEastResize,
  kCursor_SouthWestResize,
  kCursor_WestResize,
  kCursor_NorthSouthResize,
  kCursor_EastWestResize,
  kCursor_NorthEastSouthWestResize,
  kCursor_NorthWestSouthEastResize,
  kCursor_ColumnResize,
  kCursor_RowResize,
  kCursor_MiddlePanning,
  kCursor_EastPanning,
  kCursor_NorthPanning,
  kCursor_NorthEastPanning,
  kCursor_NorthWestPanning,
  kCursor_SouthPanning,
  kCursor_SouthEastPanning,
  kCursor_SouthWestPanning,
  kCursor_WestPanning,
  kCursor_Move,
  kCursor_VerticalText,
  kCursor_Cell,
  kCursor_ContextMenu,
  kCursor_Alias,
  kCursor_Progress,
  kCursor_NoDrop,
  kCursor_Copy,
  kCursor_None,
  kCursor_NotAllowed,
  kCursor_ZoomIn,
  kCursor_ZoomOut,
  kCursor_Grab,
  kCursor_Grabbing,
  kCursor_Custom
};

///
/// @brief  Interface for View-related events
///
/// @note   For more info @see View::set_view_listener
///
class UExport ViewListener {
public:
  virtual ~ViewListener() {}

  ///
  /// Called when the page title changes
  ///
  virtual void OnChangeTitle(ultralight::View* caller,
                             const String& title) {}

  ///
  /// Called when the page URL changes
  ///
  virtual void OnChangeURL(ultralight::View* caller,
                           const String& url) {}

  ///
  /// Called when the tooltip changes (usually as result of a mouse hover)
  ///
  virtual void OnChangeTooltip(ultralight::View* caller,
                               const String& tooltip) {}

  ///
  /// Called when the mouse cursor changes
  ///
  virtual void OnChangeCursor(ultralight::View* caller,
                              Cursor cursor) {}

  ///
  /// Called when a message is added to the console (useful for errors / debug)
  ///
  virtual void OnAddConsoleMessage(ultralight::View* caller,
                                   MessageSource source,
                                   MessageLevel level,
                                   const String& message,
                                   uint32_t line_number,
                                   uint32_t column_number,
                                   const String& source_id) {}
};

///
/// @brief  Interface for Load-related events
///
/// @note   For more info @see View::set_load_listener
///
class UExport LoadListener {
public:
  virtual ~LoadListener() {}

  ///
  /// Called when the page begins loading new URL into main frame
  ///
  virtual void OnBeginLoading(ultralight::View* caller) {}

  ///
  /// Called when the page finishes loading URL into main frame
  ///
  virtual void OnFinishLoading(ultralight::View* caller) {}

  ///
  /// Called when the history (back/forward state) is modified
  ///
  virtual void OnUpdateHistory(ultralight::View* caller) {}

  ///
  /// Called when all JavaScript has been parsed and the document is ready.
  /// This is the best time to make any initial JavaScript calls to your page.
  ///
  virtual void OnDOMReady(ultralight::View* caller) {}
};

}  // namespace ultralight
