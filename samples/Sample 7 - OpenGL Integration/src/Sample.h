#pragma once

#include <vector>
#include <string>
#include <memory>
#include <Ultralight/platform/Logger.h>
#include "GLTextureSurface.h"
#include "Window.h"

///
/// Constants for configuring the animation.
///
#define SPREADIMAGE         0.1     // The amount of spread between WebTiles
#define FLANKSPREAD         0.4     // How much a WebTile moves way from center
#define FRICTION            10.0    // Friction while "flowing" through WebTiles
#define MAXSPEED            7.0     // Throttle maximum speed to this value
#define ZOOMTIME            0.3     // Speed to zoom in/out of a WebTile
#define TRANSPARENT         1       // Whether or not we should use transparency

class WebTile;

using namespace ultralight;

///
/// Sample is responsible for all high-level logic.
///
class Sample : public WindowListener,
               public ViewListener,
               public LoadListener,
               public Logger {
 public:
  Sample();
  ~Sample();

  void addWebTileWithURL(const std::string& url, int width, int height);

  void Run();

  void draw();

  void drawTile(int index, double off, double zoom);

  void updateAnimationAtTime(double elapsed);

  void endAnimation();

  void driveAnimation();

  void startAnimation(double speed);

  void animateTo(int index);

  void handleDragBegin(int x, int y);

  void handleDragMove(int x, int y);

  void handleDragEnd(int x, int y);

  /////////////////////////////////////////////////////////////////////////////
  /// Inherited from WindowListener:                                        ///
  /////////////////////////////////////////////////////////////////////////////

  virtual void OnClose() override;

  virtual void OnResize(uint32_t width, uint32_t height) override;

  virtual void OnChangeFocus(bool focused) override;

  virtual void OnKeyEvent(const KeyEvent& evt) override;

  virtual void OnMouseEvent(const MouseEvent& evt) override;

  virtual void OnScrollEvent(const ScrollEvent& evt) override;

  /////////////////////////////////////////////////////////////////////////////
  /// Inherited from ViewListener:                                          ///
  /////////////////////////////////////////////////////////////////////////////

  virtual void OnChangeCursor(ultralight::View* caller,
                              Cursor cursor) override;

  virtual RefPtr<View> OnCreateChildView(ultralight::View* caller,
                                         const String& opener_url,
                                         const String& target_url,
                                         bool is_popup,
                                         const IntRect& popup_rect) override;

  /////////////////////////////////////////////////////////////////////////////
  /// Inherited from LoadListener:                                          ///
  /////////////////////////////////////////////////////////////////////////////

  virtual void OnFailLoading(ultralight::View* caller,
                             uint64_t frame_id,
                             bool is_main_frame,
                             const String& url,
                             const String& description,
                             const String& error_domain,
                             int error_code) override;

  /////////////////////////////////////////////////////////////////////////////
  /// Inherited from Logger:                                                ///
  /////////////////////////////////////////////////////////////////////////////

  virtual void LogMessage(LogLevel log_level,
                          const String16& message) override;

 protected:
  bool should_quit_ = false;
  bool is_animating_ = false;
  bool is_dragging_ = false;
  bool is_active_web_tile_focused_ = false;
  bool zoom_direction_;
  double offset_ = 0;
  double start_time_;
  double start_off_;
  double start_pos_;
  double start_speed_;
  double run_delta_;
  double last_pos_;
  double zoom_start_ = 1;
  double zoom_end_;
  int active_web_tile_ = -1;
  int width_, height_;
  std::vector<std::unique_ptr<WebTile>> web_tiles_;
  GLfloat color_[16];
  RefPtr<Renderer> renderer_;
  std::unique_ptr<GLTextureSurfaceFactory> surface_factory_;
  std::unique_ptr<Window> window_;
};
