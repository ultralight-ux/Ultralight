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
  bool shouldQuit, isAnimating, isDragging, isActiveWebTileFocused,
       zoomDirection;
  double offset, startTime, startOff, startPos, startSpeed, runDelta, lastPos,
         zoomStart, zoomEnd;
  int numTiles;
  std::vector<std::unique_ptr<WebTile>> webTiles;
  GLfloat customColor[16];
  int activeWebTile;
  int WIDTH, HEIGHT;
  RefPtr<Renderer> renderer_;
  std::unique_ptr<GLTextureSurfaceFactory> surface_factory_;
  std::unique_ptr<Window> window_;
};
