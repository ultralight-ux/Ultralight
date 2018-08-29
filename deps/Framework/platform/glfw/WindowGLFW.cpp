#include <Framework/Window.h>
#include "PlatformGLFW.h"

extern "C" {

static void WindowGLFW_error_callback(int error, const char* description)
{
	fprintf(stderr, "GLFW Error: %s\n", description);
}

static void WindowGLFW_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
}

static void WindowGLFW_char_callback(GLFWwindow* window, unsigned int codepoint)
{
}

static void WindowGLFW_charmods_callback(GLFWwindow* window, unsigned int codepoint, int mods)
{
}

static void WindowGLFW_cursor_pos_callback(GLFWwindow* window, double xpos, double ypos)
{
  framework::Window* win = static_cast<framework::Window*>(glfwGetWindowUserPointer(window));
  if (win->listener()) {
    ultralight::MouseEvent evt;
    evt.type = ultralight::MouseEvent::kType_MouseMoved;
    evt.x = win->PixelsToDevice(xpos);
    evt.y = win->PixelsToDevice(ypos);
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
    evt.x = win->PixelsToDevice(xpos);
    evt.y = win->PixelsToDevice(ypos);
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
    evt.delta_x = win->PixelsToDevice(xoffset * 32);
    evt.delta_y = win->PixelsToDevice(yoffset * 32);
    win->listener()->OnScrollEvent(evt);
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

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    float xscale, yscale;
    glfwGetMonitorContentScale(glfwGetPrimaryMonitor(), &xscale, &yscale);
    
    GLFWwindow* win = glfwCreateWindow(config.width * xscale, config.height * xscale, "Simple example", NULL, NULL);
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
		glfwSetCharModsCallback(window_, WindowGLFW_charmods_callback);
		glfwSetCursorPosCallback(window_, WindowGLFW_cursor_pos_callback);
		glfwSetMouseButtonCallback(window_, WindowGLFW_mouse_button_callback);
    glfwSetScrollCallback(window_, WindowGLFW_scroll_callback);

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
    glfwSetCharModsCallback(window_, nullptr);
    glfwSetCursorPosCallback(window_, nullptr);
    glfwSetMouseButtonCallback(window_, nullptr);
    glfwSetScrollCallback(window_, nullptr);

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
	return std::make_unique<WindowGLFW>(config);
}

}