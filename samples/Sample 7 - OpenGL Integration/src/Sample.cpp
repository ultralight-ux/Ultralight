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

static void GLFW_error_callback(int error, const char* description) {
  fprintf(stderr, "GLFW Error: %s\n", description);
}

}

Sample::Sample() : 
  shouldQuit(false),
  isAnimating(false),
  isDragging(false),
  isActiveWebTileFocused(false),
  offset(0),
  zoomStart(-1),
  numTiles(0),
  activeWebTile(-1) {
  glfwSetErrorCallback(GLFW_error_callback);

  if (!glfwInit())
    exit(EXIT_FAILURE);

  const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
  int monitor_width = mode->width;
  int monitor_height = mode->height;

  WIDTH = static_cast<int>(monitor_width * 0.8);
  HEIGHT = static_cast<int>(monitor_height * 0.8);

  if (WIDTH > 1800)
    WIDTH = 1800;
  if (HEIGHT > 1200)
    HEIGHT = 1200;

  window_.reset(new Window(WIDTH, HEIGHT, false));
  window_->MoveToCenter();
  window_->set_listener(this);
  window_->SetTitle("Ultralight Sample 7 - OpenGL Integration");

  WIDTH = window_->width();
  HEIGHT = window_->height();
  
  glOrtho(0, WIDTH, 0, HEIGHT, -1, 1);
  glEnable(GL_TEXTURE_2D);

  Config config;
  config.device_scale = window_->scale();
  config.scroll_timer_delay = 1.0 / 90.0;

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

  addWebTileWithURL("file:///welcome.html", WIDTH, HEIGHT);

  // Set our first WebTile as active
  activeWebTile = 0;
  webTiles[0]->view()->Focus();
  double curTime = glfwGetTime();
  zoomDirection = true;
  zoomStart = curTime;
  zoomEnd = curTime + ZOOMTIME;
}

Sample::~Sample() {
  webTiles.clear();

  renderer_ = nullptr;

  glfwTerminate();
}

void Sample::addWebTileWithURL(const std::string& url, int width,
                                    int height) {
  WebTile* tile = new WebTile(renderer_, width, height);

  tile->view()->LoadURL(url.c_str());
  tile->view()->set_view_listener(this);
  tile->view()->set_load_listener(this);

  webTiles.push_back(std::unique_ptr<WebTile>(tile));
}

void Sample::Run() {
  std::chrono::milliseconds interval_ms(4);
  std::chrono::steady_clock::time_point next_paint = std::chrono::steady_clock::now();
  
  while (!glfwWindowShouldClose(window_->handle())) {
    long long timeout_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
      next_paint - std::chrono::steady_clock::now()).count();
    unsigned long timeout = timeout_ms <= 0 ? 0 : (unsigned long)timeout_ms;

    if (timeout > 0)
      glfwWaitEventsTimeout(timeout / 1000.0);
    else
      glfwPollEvents();

    renderer_->Update();

    if (shouldQuit)
      return;

    timeout_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
      next_paint - std::chrono::steady_clock::now()).count();

    if (timeout_ms <= 0) {
      renderer_->Render();

      if (isAnimating)
        driveAnimation();

      draw();

      next_paint = std::chrono::steady_clock::now() + interval_ms;
    }
  }
}

void Sample::draw() {
  double curTime = glfwGetTime();
  double zoom = 0;

  if (zoomStart > 0) {
    if (curTime < zoomEnd) {
      zoom = (curTime - zoomStart) / (zoomEnd - zoomStart);

      if (!zoomDirection)
        zoom = 1.0 - zoom;
    } else {
      zoomStart = -1;
      zoomEnd = 0;

      isActiveWebTileFocused = zoomDirection;
    }
  }

  if (isActiveWebTileFocused) {
    GLTextureSurface* surface = webTiles[activeWebTile]->surface();

    if (surface) {
      int tileWidth = surface->width();
      int tileHeight = surface->height();

      glOrtho(0, WIDTH, 0, HEIGHT, -1, 1);
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
      window_->PresentFrame();
    }
  } else {
    glViewport(0,0,WIDTH,HEIGHT);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glClearColor(0,0,0,0);
    glVertexPointer(3,GL_FLOAT,0,GVertices);
    glEnableClientState(GL_VERTEX_ARRAY);
    glTexCoordPointer(2, GL_SHORT, 0, GTextures);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glColorPointer(4, GL_FLOAT, 0, customColor);
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

    int i, len = webTiles.size();
    int mid = (int)floor(offset + 0.5);
    int iStartPos = mid - webTiles.size();

    if (iStartPos < 0)
      iStartPos = 0;

    for (i = iStartPos; i < mid; ++i)
      drawTile(i, i-offset, 0);

    int iEndPos = mid + webTiles.size();

    if (iEndPos >= len)
      iEndPos = len - 1;

    for (i = iEndPos; i > mid; --i)
      drawTile(i, i - offset, 0);

    drawTile(mid, mid - offset, zoom);

    window_->PresentFrame();
  }
}

void Sample::drawTile(int index, double off, double zoom) {
  GLTextureSurface* surface = webTiles[index]->surface();

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

  m[3] = -f;
  m[0] = 1-fabs(f);

  double sc = 0.45 * (1 - fabs(f));
  sc = (1 - zoom) * sc + 1 * zoom;

  trans += f * 1.1;

  for (int i = 0; i < 16; i++)
    customColor[i] = 1.0;

  if (f >= 0) {
    customColor[0] = customColor[1] = customColor[2] = 1 -
                                      (f / FLANKSPREAD);
    customColor[8] = customColor[9] = customColor[10] = 1 -
                                      (f / FLANKSPREAD);
  } else {
    customColor[4] = customColor[5] = customColor[6] = 1 -
                                      (-f / FLANKSPREAD);
    customColor[12] = customColor[13] = customColor[14] = 1 -
                                        (-f / FLANKSPREAD);
  }


  glPushMatrix();
  glBindTexture(GL_TEXTURE_2D, surface->GetTextureAndSyncIfNeeded());
  glTranslatef(trans, 0, 0);
  glScalef(sc, sc, 1);
  glMultMatrixf(m);
  glColorPointer(4, GL_FLOAT, 0, customColor);
  glDrawArrays(GL_TRIANGLE_STRIP,0,4);

  // Draw reflection:

  glTranslatef(0,-2,0);
  glScalef(1,-1,1);

  const double darkness = 4.0;

  for (int i = 0; i < 16; i += 4) {
    customColor[i] = 1.0 / darkness + 0.05;
    customColor[i + 1] = 1.0 / darkness + 0.05;
    customColor[i + 2] = 1.0 / darkness + 0.05;
  }

  if (f >= 0) {
    customColor[0] = customColor[1] = customColor[2] =
                                        (1- (f / FLANKSPREAD)) / darkness + 0.05;

  } else {
    customColor[4] = customColor[5] = customColor[6] =
                                        (1-(-f / FLANKSPREAD)) / darkness + 0.05;

  }

  customColor[8] = customColor[9] = customColor[10] = 0;
  customColor[12] = customColor[13] = customColor[14] = 0;

  glDrawArrays(GL_TRIANGLE_STRIP,0,4);

  glPopMatrix();
}

void Sample::updateAnimationAtTime(double elapsed) {
  int max = webTiles.size() - 1;

  if (elapsed > runDelta)
    elapsed = runDelta;

  double delta = fabs(startSpeed) * elapsed - FRICTION * elapsed *
                 elapsed / 2;

  if (startSpeed < 0)
    delta = -delta;

  offset = startOff + delta;

  if (offset > max)
    offset = max;

  if (offset < 0)
    offset = 0;
}

void Sample::endAnimation() {
  if (isAnimating) {
    int max = webTiles.size() - 1;
    offset = floor(offset + 0.5);

    if (offset > max)
      offset = max;

    if (offset < 0)
      offset = 0;

    isAnimating = false;
  }
}

void Sample::driveAnimation() {
  double elapsed = glfwGetTime() - startTime;

  if (elapsed >= runDelta)
    endAnimation();
  else
    updateAnimationAtTime(elapsed);
}

void Sample::startAnimation(double speed) {
  if (isAnimating)
    endAnimation();

  // Adjust speed to make this land on an even location
  double delta = speed * speed / (FRICTION * 2);
  if (speed < 0)
    delta = -delta;

  double nearest = startOff + delta;
  nearest = floor(nearest + 0.5);
  startSpeed = sqrt(fabs(nearest - startOff) * FRICTION * 2);

  if (nearest < startOff)
    startSpeed = -startSpeed;

  runDelta = fabs(startSpeed / FRICTION);
  startTime = glfwGetTime();

  isAnimating = true;

  int lastActiveWebTile = activeWebTile;

  activeWebTile = (int)nearest;

  if (activeWebTile >= (int)webTiles.size())
    activeWebTile = webTiles.size() - 1;
  else if (activeWebTile < 0)
    activeWebTile = 0;

  if (activeWebTile != lastActiveWebTile) {
    webTiles[lastActiveWebTile]->view()->Unfocus();
    //webTiles[lastActiveWebTile]->view()->PauseRendering();
    webTiles[activeWebTile]->view()->Focus();
    //webTiles[activeWebTile]->view()->ResumeRendering();
  }
}

void Sample::animateTo(int index) {
  if (index == offset)
    return;

  if (isActiveWebTileFocused) {
    double curTime = glfwGetTime();
    zoomDirection = false;
    zoomStart = curTime;
    zoomEnd = curTime + ZOOMTIME;
    isActiveWebTileFocused = false;
  }

  startOff = offset;
  offset = index;

  int dist = (int)offset - (int)startOff;

  double speed = sqrt(abs(dist) * 2 * FRICTION);

  if (dist < 0)
    speed = -speed;

  startAnimation(speed);
}


void Sample::handleDragBegin(int x, int y) {
  isDragging = true;

  startPos = (x / (double)WIDTH) * 10 - 5;
  startOff = offset;

  isDragging = true;

  startTime = glfwGetTime();
  lastPos = startPos;

  endAnimation();
}

void Sample::handleDragMove(int x, int y) {
  double pos = (x / (double)WIDTH) * 10 - 5;

  int max = webTiles.size()-1;

  offset = startOff + (startPos - pos);

  if (offset > max)
    offset = max;

  if (offset < 0)
    offset = 0;

  double time = glfwGetTime();

  if (time - startTime > 0.2) {
    startTime = time;
    lastPos = pos;
  }
}

void Sample::handleDragEnd(int x, int y) {
  double pos = (x / (double)WIDTH) * 10 - 5;

  if (isDragging) {
    // Start animation to nearest
    startOff += (startPos - pos);
    offset = startOff;

    double time = glfwGetTime();
    double speed = (lastPos - pos)/((time - startTime) + 0.00001);

    if (speed > MAXSPEED)
      speed = MAXSPEED;

    if (speed < -MAXSPEED)
      speed = -MAXSPEED;

    startAnimation(speed);
  }

  isDragging = false;
}

void Sample::OnClose() {
  shouldQuit = true;
}

void Sample::OnResize(uint32_t width, uint32_t height) {
  if (width == WIDTH && height == HEIGHT)
    return;

  for (auto& i : webTiles) {
    i->view()->Resize(width, height);
    WIDTH = width;
    HEIGHT = height;    
  }
}

void Sample::OnChangeFocus(bool focused) {
}

void Sample::OnKeyEvent(const KeyEvent& evt) {
  if (evt.type == evt.kType_RawKeyDown) {
    if (evt.virtual_key_code == KeyCodes::GK_ESCAPE) {
      shouldQuit = true;
      return;
    }
    else if (evt.virtual_key_code == KeyCodes::GK_OEM_3 /* Backquote */) {
      if (zoomStart > 0)
        return;

      if (isActiveWebTileFocused) {
        double curTime = glfwGetTime();
        zoomDirection = false;
        zoomStart = curTime;
        zoomEnd = curTime + ZOOMTIME;
        isActiveWebTileFocused = false;
      }
      else {
        double curTime = glfwGetTime();
        zoomDirection = true;
        zoomStart = curTime;
        zoomEnd = curTime + ZOOMTIME;
      }

      return;
    }
    else if (evt.modifiers & KeyEvent::kMod_AltKey) {
      if (evt.virtual_key_code == KeyCodes::GK_LEFT) {
        webTiles[activeWebTile]->view()->GoBack();
        return;
      }
      else if (evt.virtual_key_code == KeyCodes::GK_RIGHT) {
        webTiles[activeWebTile]->view()->GoForward();
        return;
      }
      else if (evt.virtual_key_code == KeyCodes::GK_T) {
        webTiles[activeWebTile]->ToggleTransparency();
        return;
      }
      else if (evt.virtual_key_code == KeyCodes::GK_X) {
        if (webTiles.size() > 1) {
          for (auto i = webTiles.begin(); i != webTiles.end(); ++i) {
            if (*i == webTiles[activeWebTile]) {
              webTiles.erase(i);
              break;
            }
          }

          if (activeWebTile > 0) {
            activeWebTile--;
            startOff = offset + 1;
            animateTo(activeWebTile);
          }
          else {
            startOff = offset - 1;
            animateTo(activeWebTile);
          }

          webTiles[activeWebTile]->view()->Focus();
          //webTiles[activeWebTile]->view()->ResumeRendering();

          return;
        }
      }
      else if (evt.virtual_key_code == KeyCodes::GK_G) {
        addWebTileWithURL("http://www.duckduckgo.com", WIDTH, HEIGHT);

        animateTo(webTiles.size() - 1);

        return;
      }
    }
  }

  webTiles[activeWebTile]->view()->FireKeyEvent(evt);
}

void Sample::OnMouseEvent(const MouseEvent& evt) {
  switch (evt.type) {
  case MouseEvent::kType_MouseMoved: {
    if (isActiveWebTileFocused)
      webTiles[activeWebTile]->view()->FireMouseEvent(evt);

    if (isDragging)
      handleDragMove(evt.x, evt.y);

    break;
  }
  case MouseEvent::kType_MouseDown: {
    if (isActiveWebTileFocused)
      webTiles[activeWebTile]->view()->FireMouseEvent(evt);
    else if (evt.button == MouseEvent::kButton_Left)
      handleDragBegin(evt.x, evt.y);
    break;
  }
  case MouseEvent::kType_MouseUp: {
    if (isActiveWebTileFocused)
      webTiles[activeWebTile]->view()->FireMouseEvent(evt);
    else if (evt.button == MouseEvent::kButton_Left)
      handleDragEnd(evt.x, evt.y);
    break;
  }
  }
}

void Sample::OnScrollEvent(const ScrollEvent& evt) {
  webTiles[activeWebTile]->view()->FireScrollEvent(evt);
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
  RefPtr<View> new_view = renderer_->CreateView(WIDTH, HEIGHT, false, nullptr);
  WebTile* new_tile = new WebTile(new_view);
  new_view->set_view_listener(this);
  new_view->set_load_listener(this);

  webTiles.push_back(std::unique_ptr<WebTile>(new_tile));
  animateTo(webTiles.size() - 1);

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
