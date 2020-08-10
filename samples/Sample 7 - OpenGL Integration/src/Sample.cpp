#include "Sample.h"
#include "GLTextureSurface.h"
#include "WebTile.h"
#include <AppCore/Platform.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include <cmath>
#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <chrono>

// The vertices of our 3D quad
const GLfloat GVertices[] = {
  -1.0f, -1.0f, 0.0f,
   1.0f, -1.0f, 0.0f,
  -1.0f,  1.0f, 0.0f,
   1.0f,  1.0f, 0.0f,
};

// The UV texture coordinates of our 3D quad
const GLshort GTextures[] = {
  0, 1,
  1, 1,
  0, 0,
  1, 0,
};

extern "C" {

// Called by GLFW when an error is encountered.
static void GLFW_error_callback(int error, const char* description) {
  fprintf(stderr, "GLFW Error: %s\n", description);
}

}

Sample::Sample() {
  glfwSetErrorCallback(GLFW_error_callback);

  if (!glfwInit())
    exit(EXIT_FAILURE);

  const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
  int monitor_width = mode->width;
  int monitor_height = mode->height;

  // Make initial window size 80% of monitor's dimensions
  width_ = static_cast<int>(monitor_width * 0.8);
  height_ = static_cast<int>(monitor_height * 0.8);

  // Clamp initial window width to 1920 pixels
  if (width_ > 1920) width_ = 1920;

  window_.reset(new Window(width_, height_, false));
  window_->MoveToCenter();
  window_->set_listener(this);
  window_->SetTitle("Ultralight Sample 7 - OpenGL Integration");

  width_ = window_->width();
  height_ = window_->height();
  
  glOrtho(0, width_, 0, height_, -1, 1);
  glEnable(GL_TEXTURE_2D);

  Config config;
  config.device_scale = window_->scale();
  config.scroll_timer_delay = 1.0 / (mode->refreshRate);
  config.animation_timer_delay = 1.0 / (mode->refreshRate);

  ///
  /// We need to tell config where our resources are so it can load our
  /// bundled certificate chain and make HTTPS requests.
  ///
  config.resource_path = "./resources/";

  ///
  /// Make sure the GPU renderer is disabled so we can render to an offscreen
  /// pixel buffer surface.
  ///
  config.use_gpu_renderer = false;

  ///
  /// Pass our configuration to the Platform singleton so that the library
  /// can use it.
  ///
  /// The Platform singleton can be used to define various platform-specific
  /// properties and handlers such as file system access, font loaders, and
  /// the gpu driver.
  ///
  Platform::instance().set_config(config);

  ///
  /// Use AppCore's font loader singleton to load fonts from the OS.
  ///
  /// You could replace this with your own to provide your own fonts.
  ///
  Platform::instance().set_font_loader(GetPlatformFontLoader());

  ///
  /// Use AppCore's file system singleton to load file:/// URLs from the OS.
  ///
  /// You could replace this with your own to provide your own file loader
  /// (useful if you need to bundle encrypted / compressed HTML assets).
  ///
  Platform::instance().set_file_system(GetPlatformFileSystem("./assets"));

  ///
  /// Register our Sample instance as a logger so we can handle the
  /// library's LogMessage() event below in case we encounter an error.
  ///
  Platform::instance().set_logger(this);

  ///
  /// Use a custom factory to create Surfaces backed by an OpenGL texture.
  ///
  surface_factory_.reset(new GLTextureSurfaceFactory());
  Platform::instance().set_surface_factory(surface_factory_.get());

  ///
  /// Create our Renderer (you should only create this once per application).
  /// 
  /// The Renderer singleton maintains the lifetime of the library and
  /// is required before creating any Views. It should outlive any Views.
  ///
  /// You should set up the Platform singleton before creating this.
  ///
  renderer_ = Renderer::Create();

  addWebTileWithURL("file:///welcome.html", width_, height_);

  // Set our first WebTile as active
  active_web_tile_ = 0;
  web_tiles_[0]->view()->Focus();
  double curTime = glfwGetTime();
  zoom_direction_ = true;
  zoom_start_ = curTime;
  zoom_end_ = curTime + ZOOMTIME;
}

Sample::~Sample() {
  web_tiles_.clear();

  renderer_ = nullptr;

  glfwTerminate();
}

void Sample::addWebTileWithURL(const std::string& url, int width,
                                    int height) {
  WebTile* tile = new WebTile(renderer_, width, height);

  tile->view()->LoadURL(url.c_str());
  tile->view()->set_view_listener(this);
  tile->view()->set_load_listener(this);

  web_tiles_.push_back(std::unique_ptr<WebTile>(tile));
}

void Sample::Run() {
  ///
  /// Our main run loop tries to conserve CPU usage by sleeping in 4ms bursts
  /// between each paint.
  ///
  /// We use glfwWaitEventsTimeout() to perform the sleep, which also allows
  /// us the ability to wake up early if the OS sends us an event.
  ///
  std::chrono::milliseconds interval_ms(4);
  std::chrono::steady_clock::time_point next_paint = std::chrono::steady_clock::now();
  
  while (!glfwWindowShouldClose(window_->handle())) {
    ///
    /// Query the system clock to see how many milliseconds are left until
    /// the next scheduled paint.
    ///
    long long timeout_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
      next_paint - std::chrono::steady_clock::now()).count();
    unsigned long timeout = timeout_ms <= 0 ? 0 : (unsigned long)timeout_ms;

    ///
    /// Use glfwWaitEventsTimeout() if we still have time left before the next
    /// paint. (Will use OS primitives to sleep and wait for OS input events)
    ///
    /// Otherwise, call glfwPollEvents() immediately and continue.
    ///
    if (timeout > 0)
      glfwWaitEventsTimeout(timeout / 1000.0);
    else
      glfwPollEvents();

    ///
    /// Allow Ultralight to update internal timers, JavaScript callbacks, and
    /// other resource callbacks.
    ///
    renderer_->Update();

    if (should_quit_)
      return;

    ///
    /// Update our timeout by querying the system clock again.
    ///
    timeout_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
      next_paint - std::chrono::steady_clock::now()).count();

    ///
    /// Perform paint if we have reached the scheduled time.
    ///
    if (timeout_ms <= 0) {
      ///
      /// Re-paint the Surfaces of all Views as needed.
      ///
      renderer_->Render();

      if (is_animating_)
        driveAnimation();

      ///
      /// Draw the WebTile overlays with OpenGL.
      ///
      draw();

      ///
      /// Schedule the next paint.
      ///
      next_paint = std::chrono::steady_clock::now() + interval_ms;
    }
  }
}

void Sample::draw() {
  double curTime = glfwGetTime();
  double zoom = 0;

  if (zoom_start_ > 0) {
    if (curTime < zoom_end_) {
      zoom = (curTime - zoom_start_) / (zoom_end_ - zoom_start_);

      if (!zoom_direction_)
        zoom = 1.0 - zoom;
    } else {
      zoom_start_ = -1;
      zoom_end_ = 0;

      is_active_web_tile_focused_ = zoom_direction_;
    }
  }

  if (is_active_web_tile_focused_) {
    GLTextureSurface* surface = web_tiles_[active_web_tile_]->surface();

    if (surface) {
      int tileWidth = surface->width();
      int tileHeight = surface->height();

      glViewport(0,0,width_,height_);
      glOrtho(0, width_, 0, height_, -1, 1);
      glClear(GL_COLOR_BUFFER_BIT);
      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      glScalef(1,1,1);
      glBindTexture(GL_TEXTURE_2D, surface->GetTextureAndSyncIfNeeded());
      glColor4f(1,1,1,1);
      glBegin(GL_QUADS);
      glTexCoord2f(0,1);
      glVertex3f(0, 0, 0.0f);
      glTexCoord2f(1,1);
      glVertex3f((GLfloat)tileWidth, 0, 0.0f);
      glTexCoord2f(1,0);
      glVertex3f((GLfloat)tileWidth, (GLfloat)tileHeight, 0.0f);
      glTexCoord2f(0,0);
      glVertex3f(0, (GLfloat)tileHeight, 0.0f);
      glEnd();
      glBindTexture(GL_TEXTURE_2D, 0);
      window_->PresentFrame();
    }
  } else {
    glViewport(0,0,width_,height_);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glClearColor(0,0,0,0);
    glVertexPointer(3,GL_FLOAT,0,GVertices);
    glEnableClientState(GL_VERTEX_ARRAY);
    glTexCoordPointer(2, GL_SHORT, 0, GTextures);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glColorPointer(4, GL_FLOAT, 0, color_);
    glEnableClientState(GL_COLOR_ARRAY);
    glEnable(GL_TEXTURE_2D);
#if TRANSPARENT
    glEnable (GL_BLEND);
    glBlendFunc (GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
#endif
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glScalef(1,1,1);
    glTranslatef(0, 0, 0.5);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    int i, len = (int)web_tiles_.size();
    int mid = (int)floor(offset_ + 0.5);
    int iStartPos = mid - (int)web_tiles_.size();

    if (iStartPos < 0)
      iStartPos = 0;

    for (i = iStartPos; i < mid; ++i)
      drawTile(i, i-offset_, 0);

    int iEndPos = mid + (int)web_tiles_.size();

    if (iEndPos >= len)
      iEndPos = len - 1;

    for (i = iEndPos; i > mid; --i)
      drawTile(i, i - offset_, 0);

    drawTile(mid, mid - offset_, zoom);

    window_->PresentFrame();
  }
}

void Sample::drawTile(int index, double off, double zoom) {
  GLTextureSurface* surface = web_tiles_[index]->surface();

  if (!surface)
    return;

  GLfloat m[16];
  memset(m,0,sizeof(m));
  m[10] = 1;
  m[15] = 1;
  m[0] = 1;
  m[5] = 1;

  double trans = off * SPREADIMAGE;
  double f = off * FLANKSPREAD;

  if (f < -FLANKSPREAD)
    f = -FLANKSPREAD;

  else if (f > FLANKSPREAD)
    f = FLANKSPREAD;

  m[3] = (GLfloat)-f;
  m[0] = 1 - (GLfloat)fabs(f);

  double sc = 0.45 * (1 - fabs(f));
  sc = (1 - zoom) * sc + 1 * zoom;

  trans += f * 1.1;

  for (int i = 0; i < 16; i++)
    color_[i] = 1.0;

  if (f >= 0) {
    color_[0] = color_[1] = color_[2] = 1 -
                                      (GLfloat)(f / FLANKSPREAD);
    color_[8] = color_[9] = color_[10] = 1 -
                                      (GLfloat)(f / FLANKSPREAD);
  } else {
    color_[4] = color_[5] = color_[6] = 1 -
                                      (GLfloat)(-f / FLANKSPREAD);
    color_[12] = color_[13] = color_[14] = 1 -
                                      (GLfloat)(-f / FLANKSPREAD);
  }


  glPushMatrix();
  glBindTexture(GL_TEXTURE_2D, surface->GetTextureAndSyncIfNeeded());
  glTranslatef((GLfloat)trans, 0, 0);
  glScalef((GLfloat)sc, (GLfloat)sc, 1);
  glMultMatrixf(m);
  glColorPointer(4, GL_FLOAT, 0, color_);
  glDrawArrays(GL_TRIANGLE_STRIP,0,4);

  // Draw reflection:

  glTranslatef(0,-2,0);
  glScalef(1,-1,1);

  const double darkness = 4.0;

  for (int i = 0; i < 16; i += 4) {
    color_[i] = color_[i + 1] = color_[i + 2] = 
      (GLfloat)(1.0 / darkness + 0.05);
  }

  if (f >= 0) {
    color_[0] = color_[1] = color_[2] =
                           (GLfloat)((1- (f / FLANKSPREAD)) / darkness + 0.05);

  } else {
    color_[4] = color_[5] = color_[6] =
                           (GLfloat)((1-(-f / FLANKSPREAD)) / darkness + 0.05);

  }

  color_[8] = color_[9] = color_[10] = 0;
  color_[12] = color_[13] = color_[14] = 0;

  glDrawArrays(GL_TRIANGLE_STRIP,0,4);
  glBindTexture(GL_TEXTURE_2D, 0);

  glPopMatrix();
}

void Sample::updateAnimationAtTime(double elapsed) {
  int max = (int)web_tiles_.size() - 1;

  if (elapsed > run_delta_)
    elapsed = run_delta_;

  double delta = fabs(start_speed_) * elapsed - FRICTION * elapsed *
                 elapsed / 2;

  if (start_speed_ < 0)
    delta = -delta;

  offset_ = start_off_ + delta;

  if (offset_ > max)
    offset_ = max;

  if (offset_ < 0)
    offset_ = 0;
}

void Sample::endAnimation() {
  if (is_animating_) {
    int max = (int)web_tiles_.size() - 1;
    offset_ = floor(offset_ + 0.5);

    if (offset_ > max)
      offset_ = max;

    if (offset_ < 0)
      offset_ = 0;

    is_animating_ = false;
  }
}

void Sample::driveAnimation() {
  double elapsed = glfwGetTime() - start_time_;

  if (elapsed >= run_delta_)
    endAnimation();
  else
    updateAnimationAtTime(elapsed);
}

void Sample::startAnimation(double speed) {
  if (is_animating_)
    endAnimation();

  // Adjust speed to make this land on an even location
  double delta = speed * speed / (FRICTION * 2);
  if (speed < 0)
    delta = -delta;

  double nearest = start_off_ + delta;
  nearest = floor(nearest + 0.5);
  start_speed_ = sqrt(fabs(nearest - start_off_) * FRICTION * 2);

  if (nearest < start_off_)
    start_speed_ = -start_speed_;

  run_delta_ = fabs(start_speed_ / FRICTION);
  start_time_ = glfwGetTime();

  is_animating_ = true;

  int lastActiveWebTile = active_web_tile_;

  active_web_tile_ = (int)nearest;

  if (active_web_tile_ >= (int)web_tiles_.size())
    active_web_tile_ = (int)web_tiles_.size() - 1;
  else if (active_web_tile_ < 0)
    active_web_tile_ = 0;

  if (active_web_tile_ != lastActiveWebTile) {
    web_tiles_[lastActiveWebTile]->view()->Unfocus();
    //web_tiles_[lastActiveWebTile]->view()->PauseRendering();
    web_tiles_[active_web_tile_]->view()->Focus();
    //web_tiles_[active_web_tile_]->view()->ResumeRendering();
  }
}

void Sample::animateTo(int index) {
  if (index == offset_)
    return;

  if (is_active_web_tile_focused_) {
    double curTime = glfwGetTime();
    zoom_direction_ = false;
    zoom_start_ = curTime;
    zoom_end_ = curTime + ZOOMTIME;
    is_active_web_tile_focused_ = false;
  }

  start_off_ = offset_;
  offset_ = index;

  int dist = (int)offset_ - (int)start_off_;

  double speed = sqrt(abs(dist) * 2 * FRICTION);

  if (dist < 0)
    speed = -speed;

  startAnimation(speed);
}


void Sample::handleDragBegin(int x, int y) {
  is_dragging_ = true;

  start_pos_ = (x / (double)width_) * 10 - 5;
  start_off_ = offset_;

  is_dragging_ = true;

  start_time_ = glfwGetTime();
  last_pos_ = start_pos_;

  endAnimation();
}

void Sample::handleDragMove(int x, int y) {
  double pos = (x / (double)width_) * 10 - 5;

  int max = (int)web_tiles_.size() - 1;

  offset_ = start_off_ + (start_pos_ - pos);

  if (offset_ > max)
    offset_ = max;

  if (offset_ < 0)
    offset_ = 0;

  double time = glfwGetTime();

  if (time - start_time_ > 0.2) {
    start_time_ = time;
    last_pos_ = pos;
  }
}

void Sample::handleDragEnd(int x, int y) {
  double pos = (x / (double)width_) * 10 - 5;

  if (is_dragging_) {
    // Start animation to nearest
    start_off_ += (start_pos_ - pos);
    offset_ = start_off_;

    double time = glfwGetTime();
    double speed = (last_pos_ - pos)/((time - start_time_) + 0.00001);

    if (speed > MAXSPEED)
      speed = MAXSPEED;

    if (speed < -MAXSPEED)
      speed = -MAXSPEED;

    startAnimation(speed);
  }

  is_dragging_ = false;
}

void Sample::OnClose() {
  should_quit_ = true;
}

void Sample::OnResize(uint32_t width, uint32_t height) {
  if (width == width_ && height == height_)
    return;

  width_ = width;
  height_ = height;  

  for (auto& i : web_tiles_) {
    i->view()->Resize(width, height);
  }

  glViewport(0,0,width_,height_);
}

void Sample::OnChangeFocus(bool focused) {
}

void Sample::OnKeyEvent(const KeyEvent& evt) {
  if (evt.type == evt.kType_RawKeyDown) {
    if (evt.virtual_key_code == KeyCodes::GK_ESCAPE) {
      should_quit_ = true;
      return;
    }
    else if (evt.virtual_key_code == KeyCodes::GK_OEM_3 /* Backquote */) {
      if (zoom_start_ > 0)
        return;

      if (is_active_web_tile_focused_) {
        double curTime = glfwGetTime();
        zoom_direction_ = false;
        zoom_start_ = curTime;
        zoom_end_ = curTime + ZOOMTIME;
        is_active_web_tile_focused_ = false;
      }
      else {
        double curTime = glfwGetTime();
        zoom_direction_ = true;
        zoom_start_ = curTime;
        zoom_end_ = curTime + ZOOMTIME;
      }

      return;
    }
    else if (evt.modifiers & KeyEvent::kMod_AltKey) {
      if (evt.virtual_key_code == KeyCodes::GK_LEFT) {
        web_tiles_[active_web_tile_]->view()->GoBack();
        return;
      }
      else if (evt.virtual_key_code == KeyCodes::GK_RIGHT) {
        web_tiles_[active_web_tile_]->view()->GoForward();
        return;
      }
      else if (evt.virtual_key_code == KeyCodes::GK_T) {
        web_tiles_[active_web_tile_]->ToggleTransparency();
        return;
      }
      else if (evt.virtual_key_code == KeyCodes::GK_X) {
        if (web_tiles_.size() > 1) {
          for (auto i = web_tiles_.begin(); i != web_tiles_.end(); ++i) {
            if (*i == web_tiles_[active_web_tile_]) {
              web_tiles_.erase(i);
              break;
            }
          }

          if (active_web_tile_ > 0) {
            active_web_tile_--;
            start_off_ = offset_ + 1;
            animateTo(active_web_tile_);
          }
          else {
            start_off_ = offset_ - 1;
            animateTo(active_web_tile_);
          }

          web_tiles_[active_web_tile_]->view()->Focus();
          //web_tiles_[active_web_tile_]->view()->ResumeRendering();

          return;
        }
      }
      else if (evt.virtual_key_code == KeyCodes::GK_D) {
        addWebTileWithURL("http://www.duckduckgo.com", width_, height_);

        animateTo((int)web_tiles_.size() - 1);

        return;
      }
    }
  }

  web_tiles_[active_web_tile_]->view()->FireKeyEvent(evt);
}

void Sample::OnMouseEvent(const MouseEvent& evt) {
  switch (evt.type) {
  case MouseEvent::kType_MouseMoved: {
    if (is_active_web_tile_focused_)
      web_tiles_[active_web_tile_]->view()->FireMouseEvent(evt);

    if (is_dragging_)
      handleDragMove(evt.x, evt.y);

    break;
  }
  case MouseEvent::kType_MouseDown: {
    if (is_active_web_tile_focused_)
      web_tiles_[active_web_tile_]->view()->FireMouseEvent(evt);
    else if (evt.button == MouseEvent::kButton_Left)
      handleDragBegin(evt.x, evt.y);
    break;
  }
  case MouseEvent::kType_MouseUp: {
    if (is_active_web_tile_focused_)
      web_tiles_[active_web_tile_]->view()->FireMouseEvent(evt);
    else if (evt.button == MouseEvent::kButton_Left)
      handleDragEnd(evt.x, evt.y);
    break;
  }
  }
}

void Sample::OnScrollEvent(const ScrollEvent& evt) {
  web_tiles_[active_web_tile_]->view()->FireScrollEvent(evt);
}

void Sample::OnChangeCursor(ultralight::View* caller,
  Cursor cursor) {
  window_->SetCursor(cursor);
}

RefPtr<View> Sample::OnCreateChildView(ultralight::View* caller,
  const String& opener_url,
  const String& target_url,
  bool is_popup,
  const IntRect& popup_rect) {
  RefPtr<View> new_view = renderer_->CreateView(width_, height_, false, nullptr);
  WebTile* new_tile = new WebTile(new_view);
  new_view->set_view_listener(this);
  new_view->set_load_listener(this);

  web_tiles_.push_back(std::unique_ptr<WebTile>(new_tile));
  animateTo((int)web_tiles_.size() - 1);

  return new_view;
}

void Sample::OnFailLoading(ultralight::View* caller,
  uint64_t frame_id,
  bool is_main_frame,
  const String& url,
  const String& description,
  const String& error_domain,
  int error_code) {
}

void Sample::LogMessage(LogLevel log_level,
  const String16& message) {
  String msg(message);
  std::cout << msg.utf8().data() << std::endl;
}
