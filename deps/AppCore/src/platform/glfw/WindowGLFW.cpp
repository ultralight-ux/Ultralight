#include <Framework/Window.h>
#include "PlatformGLFW.h"
#include <Ultralight/KeyCodes.h>
#include <cmath>

static int GLFWModsToUltralightMods(int mods);
static int GLFWKeyCodeToUltralightKeyCode(int key);

extern "C" {

static void WindowGLFW_error_callback(int error, const char* description)
{
  fprintf(stderr, "GLFW Error: %s\n", description);
}

static void WindowGLFW_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  framework::Window* win = static_cast<framework::Window*>(glfwGetWindowUserPointer(window));
  if (win->listener()) {
    ultralight::KeyEvent evt;
    evt.type = action == GLFW_PRESS || action == GLFW_REPEAT ?
      ultralight::KeyEvent::kType_RawKeyDown : ultralight::KeyEvent::kType_KeyUp;
    evt.virtual_key_code = GLFWKeyCodeToUltralightKeyCode(key);
    evt.native_key_code = scancode;
    ultralight::GetKeyIdentifierFromVirtualKeyCode(evt.virtual_key_code, evt.key_identifier);
    evt.modifiers = GLFWModsToUltralightMods(mods);
    win->listener()->OnKeyEvent(evt);

    if (evt.type == ultralight::KeyEvent::kType_RawKeyDown &&
      (key == GLFW_KEY_ENTER || key == GLFW_KEY_TAB)) {
      // We have to synthesize the Char Event for these keys.
      ultralight::KeyEvent evt;
      evt.type = ultralight::KeyEvent::kType_Char;
      ultralight::String text = key == GLFW_KEY_ENTER ? ultralight::String("\r") : ultralight::String("\t");
      evt.text = text;
      evt.unmodified_text = text;
      win->listener()->OnKeyEvent(evt);
    }
  }
}

static void WindowGLFW_char_callback(GLFWwindow* window, unsigned int codepoint)
{
  framework::Window* win = static_cast<framework::Window*>(glfwGetWindowUserPointer(window));
  if (win->listener()) {
    ultralight::KeyEvent evt;
    evt.type = ultralight::KeyEvent::kType_Char;

    ultralight::String text = ultralight::String32((const char32_t*)&codepoint, 1);

    evt.text = text;
    evt.unmodified_text = text;

    win->listener()->OnKeyEvent(evt);
  }
}

static void WindowGLFW_cursor_pos_callback(GLFWwindow* window, double xpos, double ypos)
{
  framework::Window* win = static_cast<framework::Window*>(glfwGetWindowUserPointer(window));
  if (win->listener()) {
    ultralight::MouseEvent evt;
    evt.type = ultralight::MouseEvent::kType_MouseMoved;
    evt.x = win->PixelsToDevice((int)xpos);
    evt.y = win->PixelsToDevice((int)ypos);
    evt.button = ultralight::MouseEvent::kButton_None;
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
      evt.button = ultralight::MouseEvent::kButton_Left;
    else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS)
      evt.button = ultralight::MouseEvent::kButton_Middle;
    else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
      evt.button = ultralight::MouseEvent::kButton_Right;
    win->listener()->OnMouseEvent(evt);
  }
}

static void WindowGLFW_mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
  framework::Window* win = static_cast<framework::Window*>(glfwGetWindowUserPointer(window));
  if (win->listener()) {
    ultralight::MouseEvent evt;
    evt.type = action == GLFW_PRESS ? ultralight::MouseEvent::kType_MouseDown : ultralight::MouseEvent::kType_MouseUp;
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    evt.x = win->PixelsToDevice((int)xpos);
    evt.y = win->PixelsToDevice((int)ypos);
    evt.button = ultralight::MouseEvent::kButton_None;
    switch (button) {
    case GLFW_MOUSE_BUTTON_LEFT:
      evt.button = ultralight::MouseEvent::kButton_Left;
      break;
    case GLFW_MOUSE_BUTTON_MIDDLE:
      evt.button = ultralight::MouseEvent::kButton_Middle;
      break;
    case GLFW_MOUSE_BUTTON_RIGHT:
      evt.button = ultralight::MouseEvent::kButton_Right;
      break;
    }
    win->listener()->OnMouseEvent(evt);
  }
}

static void WindowGLFW_scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
  framework::Window* win = static_cast<framework::Window*>(glfwGetWindowUserPointer(window));
  if (win->listener()) {
    ultralight::ScrollEvent evt;
    evt.type = ultralight::ScrollEvent::kType_ScrollByPixel;
    evt.delta_x = win->PixelsToDevice((int)xoffset * 32);
    evt.delta_y = win->PixelsToDevice((int)yoffset * 32);
    win->listener()->OnScrollEvent(evt);
  }
}

static void WindowGLFW_window_size_callback(GLFWwindow* window, int width, int height)
{
  framework::Window* win = static_cast<framework::Window*>(glfwGetWindowUserPointer(window));
  if (win->listener()) {
    win->listener()->OnResize(win->PixelsToDevice(width), win->PixelsToDevice(height));
  }
}

} // extern "C"

namespace framework {

class WindowGLFW : public Window {
protected:
  PlatformWindowHandle window_;
  WindowListener* listener_ = nullptr;
  GLFWcursor* cursor_ibeam_;
  GLFWcursor* cursor_crosshair_;
  GLFWcursor* cursor_hand_;
  GLFWcursor* cursor_hresize_;
  GLFWcursor* cursor_vresize_;
public:
  WindowGLFW(const PlatformWindowConfig& config) {
    glfwSetErrorCallback(WindowGLFW_error_callback);

    if (!glfwInit())
      exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif

    float xscale, yscale;
    glfwGetMonitorContentScale(glfwGetPrimaryMonitor(), &xscale, &yscale);
    
    GLFWwindow* win = glfwCreateWindow(static_cast<int>(config.width * xscale), 
                                       static_cast<int>(config.height * xscale), 
                                       config.title, NULL, NULL);
    window_ = win;
    if (!window_)
    {
      glfwTerminate();
      exit(EXIT_FAILURE);
    }

    glfwSetWindowUserPointer(window_, (Window*)this);

    // Bind callbacks
    glfwSetKeyCallback(window_, WindowGLFW_key_callback);
    glfwSetCharCallback(window_, WindowGLFW_char_callback);
    glfwSetCursorPosCallback(window_, WindowGLFW_cursor_pos_callback);
    glfwSetMouseButtonCallback(window_, WindowGLFW_mouse_button_callback);
    glfwSetScrollCallback(window_, WindowGLFW_scroll_callback);
    glfwSetWindowSizeCallback(window_, WindowGLFW_window_size_callback);

    // Create standard cursors
    cursor_ibeam_ = glfwCreateStandardCursor(GLFW_IBEAM_CURSOR);
    cursor_crosshair_ = glfwCreateStandardCursor(GLFW_CROSSHAIR_CURSOR);
    cursor_hand_ = glfwCreateStandardCursor(GLFW_HAND_CURSOR);
    cursor_hresize_ = glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);
    cursor_vresize_ = glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR);
  }

  virtual ~WindowGLFW() {
    glfwSetKeyCallback(window_, nullptr);
    glfwSetCharCallback(window_, nullptr);
    glfwSetCursorPosCallback(window_, nullptr);
    glfwSetMouseButtonCallback(window_, nullptr);
    glfwSetScrollCallback(window_, nullptr);
    glfwSetWindowSizeCallback(window_, nullptr);

    glfwDestroyCursor(cursor_ibeam_);
    glfwDestroyCursor(cursor_crosshair_);
    glfwDestroyCursor(cursor_hand_);
    glfwDestroyCursor(cursor_hresize_);
    glfwDestroyCursor(cursor_vresize_);

    glfwDestroyWindow(window_);
  }
  
  virtual void set_listener(WindowListener* listener) override { listener_ = listener; }

  virtual WindowListener* listener() override { return listener_; }

  virtual int width() const override {
    // Returns width in device coordinates
    int width, height;
    glfwGetWindowSize(window_, &width, &height);
    return PixelsToDevice(width);
  }

  virtual int height() const override {
    // Return height in device coordinates
    int width, height;
    glfwGetWindowSize(window_, &width, &height);
    return PixelsToDevice(height);
  }

  virtual bool is_fullscreen() const override {
    // Window is fullscreen if a monitor is set
    return glfwGetWindowMonitor(window_) != nullptr;
  }

  virtual PlatformWindowHandle handle() const override { return window_; }

  virtual double scale() const override {
    float xscale, yscale;
    glfwGetWindowContentScale(window_, &xscale, &yscale);
    return (double)xscale;
  }

  virtual int DeviceToPixels(int val) const override {
    return (int)round(val * scale());
  }

  virtual int PixelsToDevice(int val) const override {
    return (int)round(val / scale());
  }

  virtual void SetTitle(const char* title) override {
    glfwSetWindowTitle(window_, title);
  }

  virtual void SetCursor(ultralight::Cursor cursor) override {
    switch (cursor) {
    case ultralight::kCursor_Cross:
      glfwSetCursor(window_, cursor_crosshair_);
      break;
    case ultralight::kCursor_Hand:
      glfwSetCursor(window_, cursor_hand_);
      break;
    case ultralight::kCursor_IBeam:
      glfwSetCursor(window_, cursor_ibeam_);
      break;
    case ultralight::kCursor_EastWestResize:
      glfwSetCursor(window_, cursor_hresize_);
      break;
    case ultralight::kCursor_NorthSouthResize:
      glfwSetCursor(window_, cursor_vresize_);
      break;
    default:
      glfwSetCursor(window_, nullptr);
    }
  }

  virtual void Close() override {
    glfwSetWindowShouldClose(window_, 1);
  }
};

std::unique_ptr<Window> Window::Create(const PlatformWindowConfig& config) {
  return std::unique_ptr<Window>(new WindowGLFW(config));
}

}  // namespace framework

using namespace ultralight;
using namespace ultralight::KeyCodes;

int GLFWModsToUltralightMods(int mods) {
  int result = 0;
  if (mods & GLFW_MOD_ALT)
    result |= KeyEvent::kMod_AltKey;
  if (mods & GLFW_MOD_CONTROL)
    result |= KeyEvent::kMod_CtrlKey;
  if (mods & GLFW_MOD_SUPER)
    result |= KeyEvent::kMod_MetaKey;
  if (mods & GLFW_MOD_SHIFT)
    result |= KeyEvent::kMod_ShiftKey;
  return result;
}

int GLFWKeyCodeToUltralightKeyCode(int key) {
  switch (key) {
  case GLFW_KEY_SPACE: return GK_SPACE;
  case GLFW_KEY_APOSTROPHE: return GK_OEM_7;
  case GLFW_KEY_COMMA: return GK_OEM_COMMA;
  case GLFW_KEY_MINUS: return GK_OEM_MINUS;
  case GLFW_KEY_PERIOD: return GK_OEM_PERIOD;
  case GLFW_KEY_SLASH: return GK_OEM_2;
  case GLFW_KEY_0: return GK_0;
  case GLFW_KEY_1: return GK_1;
  case GLFW_KEY_2: return GK_2;
  case GLFW_KEY_3: return GK_3;
  case GLFW_KEY_4: return GK_4;
  case GLFW_KEY_5: return GK_5;
  case GLFW_KEY_6: return GK_6;
  case GLFW_KEY_7: return GK_7;
  case GLFW_KEY_8: return GK_8;
  case GLFW_KEY_9: return GK_9;
  case GLFW_KEY_SEMICOLON: return GK_OEM_1;
  case GLFW_KEY_EQUAL: return GK_OEM_PLUS;
  case GLFW_KEY_A: return GK_A;
  case GLFW_KEY_B: return GK_B;
  case GLFW_KEY_C: return GK_C;
  case GLFW_KEY_D: return GK_D;
  case GLFW_KEY_E: return GK_E;
  case GLFW_KEY_F: return GK_F;
  case GLFW_KEY_G: return GK_G;
  case GLFW_KEY_H: return GK_H;
  case GLFW_KEY_I: return GK_I;
  case GLFW_KEY_J: return GK_J;
  case GLFW_KEY_K: return GK_K;
  case GLFW_KEY_L: return GK_L;
  case GLFW_KEY_M: return GK_M;
  case GLFW_KEY_N: return GK_N;
  case GLFW_KEY_O: return GK_O;
  case GLFW_KEY_P: return GK_P;
  case GLFW_KEY_Q: return GK_Q;
  case GLFW_KEY_R: return GK_R;
  case GLFW_KEY_S: return GK_S;
  case GLFW_KEY_T: return GK_T;
  case GLFW_KEY_U: return GK_U;
  case GLFW_KEY_V: return GK_V;
  case GLFW_KEY_W: return GK_W;
  case GLFW_KEY_X: return GK_X;
  case GLFW_KEY_Y: return GK_Y;
  case GLFW_KEY_Z: return GK_Z;
  case GLFW_KEY_LEFT_BRACKET: return GK_OEM_4;
  case GLFW_KEY_BACKSLASH: return GK_OEM_5;
  case GLFW_KEY_RIGHT_BRACKET: return GK_OEM_6;
  case GLFW_KEY_GRAVE_ACCENT: return GK_OEM_3;
  case GLFW_KEY_WORLD_1: return GK_UNKNOWN;
  case GLFW_KEY_WORLD_2: return GK_UNKNOWN;
  case GLFW_KEY_ESCAPE: return GK_ESCAPE;
  case GLFW_KEY_ENTER: return GK_RETURN;
  case GLFW_KEY_TAB: return GK_TAB;
  case GLFW_KEY_BACKSPACE: return GK_BACK;
  case GLFW_KEY_INSERT: return GK_INSERT;
  case GLFW_KEY_DELETE: return GK_DELETE;
  case GLFW_KEY_RIGHT: return GK_RIGHT;
  case GLFW_KEY_LEFT: return GK_LEFT;
  case GLFW_KEY_DOWN: return GK_DOWN;
  case GLFW_KEY_UP: return GK_UP;
  case GLFW_KEY_PAGE_UP: return GK_PRIOR;
  case GLFW_KEY_PAGE_DOWN: return GK_NEXT;
  case GLFW_KEY_HOME: return GK_HOME;
  case GLFW_KEY_END: return GK_END;
  case GLFW_KEY_CAPS_LOCK: return GK_CAPITAL;
  case GLFW_KEY_SCROLL_LOCK: return GK_SCROLL;
  case GLFW_KEY_NUM_LOCK: return GK_NUMLOCK;
  case GLFW_KEY_PRINT_SCREEN: return GK_SNAPSHOT;
  case GLFW_KEY_PAUSE: return GK_PAUSE;
  case GLFW_KEY_F1: return GK_F1;
  case GLFW_KEY_F2: return GK_F2;
  case GLFW_KEY_F3: return GK_F3;
  case GLFW_KEY_F4: return GK_F4;
  case GLFW_KEY_F5: return GK_F5;
  case GLFW_KEY_F6: return GK_F6;
  case GLFW_KEY_F7: return GK_F7;
  case GLFW_KEY_F8: return GK_F8;
  case GLFW_KEY_F9: return GK_F9;
  case GLFW_KEY_F10: return GK_F10;
  case GLFW_KEY_F11: return GK_F11;
  case GLFW_KEY_F12: return GK_F12;
  case GLFW_KEY_F13: return GK_F13;
  case GLFW_KEY_F14: return GK_F14;
  case GLFW_KEY_F15: return GK_F15;
  case GLFW_KEY_F16: return GK_F16;
  case GLFW_KEY_F17: return GK_F17;
  case GLFW_KEY_F18: return GK_F18;
  case GLFW_KEY_F19: return GK_F19;
  case GLFW_KEY_F20: return GK_F20;
  case GLFW_KEY_F21: return GK_F21;
  case GLFW_KEY_F22: return GK_F22;
  case GLFW_KEY_F23: return GK_F23;
  case GLFW_KEY_F24: return GK_F24;
  case GLFW_KEY_F25: return GK_UNKNOWN;
  case GLFW_KEY_KP_0: return GK_NUMPAD0;
  case GLFW_KEY_KP_1: return GK_NUMPAD1;
  case GLFW_KEY_KP_2: return GK_NUMPAD2;
  case GLFW_KEY_KP_3: return GK_NUMPAD3;
  case GLFW_KEY_KP_4: return GK_NUMPAD4;
  case GLFW_KEY_KP_5: return GK_NUMPAD5;
  case GLFW_KEY_KP_6: return GK_NUMPAD6;
  case GLFW_KEY_KP_7: return GK_NUMPAD7;
  case GLFW_KEY_KP_8: return GK_NUMPAD8;
  case GLFW_KEY_KP_9: return GK_NUMPAD9;
  case GLFW_KEY_KP_DECIMAL: return GK_DECIMAL;
  case GLFW_KEY_KP_DIVIDE: return GK_DIVIDE;
  case GLFW_KEY_KP_MULTIPLY: return GK_MULTIPLY;
  case GLFW_KEY_KP_SUBTRACT: return GK_SUBTRACT;
  case GLFW_KEY_KP_ADD: return GK_ADD;
  case GLFW_KEY_KP_ENTER: return GK_RETURN;
  case GLFW_KEY_KP_EQUAL: return GK_OEM_PLUS;
  case GLFW_KEY_LEFT_SHIFT: return GK_SHIFT;
  case GLFW_KEY_LEFT_CONTROL: return GK_CONTROL;
  case GLFW_KEY_LEFT_ALT: return GK_MENU;
  case GLFW_KEY_LEFT_SUPER: return GK_LWIN;
  case GLFW_KEY_RIGHT_SHIFT: return GK_SHIFT;
  case GLFW_KEY_RIGHT_CONTROL: return GK_CONTROL;
  case GLFW_KEY_RIGHT_ALT: return GK_MENU;
  case GLFW_KEY_RIGHT_SUPER: return GK_RWIN;
  case GLFW_KEY_MENU: return GK_UNKNOWN;
  default: return GK_UNKNOWN;
  }
}
